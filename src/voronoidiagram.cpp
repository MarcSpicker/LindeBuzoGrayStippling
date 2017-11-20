#include "voronoidiagram.h"
#include "voronoithread.h"

#include <cmath>
#include <iostream>

#include <QOpenGLFramebufferObjectFormat>
#include <QtMath>

VoronoiDiagram::VoronoiDiagram() : m_initialized(false), m_running(false) {
  m_context = new QOpenGLContext();
  QSurfaceFormat format;
  format.setMajorVersion(3);
  format.setMinorVersion(3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  m_context->setFormat(format);
  m_context->create();

  m_surface = new QOffscreenSurface();
  m_surface->setFormat(m_context->format());
  m_surface->create();

  m_context->makeCurrent(m_surface);

  m_vao = new QOpenGLVertexArrayObject();
  m_vao->create();

  m_shaderProgram = new QOpenGLShaderProgram(m_context);
  QOpenGLShader *vs = new QOpenGLShader(QOpenGLShader::Vertex);
  vs->compileSourceFile(":/shader/Voronoi.vert");
  QOpenGLShader *fs = new QOpenGLShader(QOpenGLShader::Fragment);
  fs->compileSourceFile(":/shader/Voronoi.frag");
  m_shaderProgram->addShader(vs);
  m_shaderProgram->addShader(fs);
  m_shaderProgram->link();

  delete vs;
  delete fs;
}

VoronoiDiagram::~VoronoiDiagram() {
  if (m_running) {
    m_thread->quit();
    m_thread->wait();

    delete m_thread;
  }

  if (m_initialized) {
    delete m_fbo;
    delete m_densityMap;
    m_coneVBO->destroy();
    delete m_coneVBO;
  }

  m_shaderProgram->deleteLater();
  m_vao->deleteLater();
  m_surface->deleteLater();
  m_context->deleteLater();
}

void VoronoiDiagram::init(const int width, const int height, QImage density,
                          const int superSampling) {
  m_width = width * superSampling;
  m_height = height * superSampling;
  m_superSampling = superSampling;

  if (m_initialized) {
    delete m_densityMap;
    delete m_fbo;
    m_coneVBO->destroy();
    delete m_coneVBO;
  }

  density = density.scaled(superSampling * density.size(), Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
  density = density.convertToFormat(QImage::Format_Grayscale8);
  m_densityMap = new QImage(density);

  m_context->makeCurrent(m_surface);

  QOpenGLFramebufferObjectFormat fboFormat;
  fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
  m_fbo = new QOpenGLFramebufferObject(m_width, m_height, fboFormat);
  initConeDrawingData();

  m_initialized = true;
}

void VoronoiDiagram::calculate(const QVector<QVector2D> &points) {
  if (!m_initialized) {
    std::cerr << "Voronoi diagram not initialized! You need to call >>> "
                 "init(int width, int height) <<< first!";
    return;
  }

  m_thread =
      new VoronoiThread(m_width, m_height, m_context, m_surface, m_vao, m_fbo,
                        m_shaderProgram, m_densityMap, points, m_coneVertices);

  connect(m_thread, &VoronoiThread::finished, this, &VoronoiDiagram::finished);
  m_running = true;

  m_thread->start();
}

void VoronoiDiagram::finished(const QVector<VoronoiCell> &cells) {
  m_thread->quit();
  m_thread->wait();

  delete m_thread;
  m_running = false;

  m_context->moveToThread(this->thread());

  emit passResult(cells);
}

void VoronoiDiagram::initConeDrawingData() {
  float radius = std::sqrt(2.0f);
  float maxError = 1.0f / (m_width > m_height ? m_width : m_height);
  float alpha = 2 * std::acos((radius - maxError) / radius);
  uint numConeSlices = (unsigned)(2 * M_PI / alpha + 0.5);

  float angleIncr = 2.0 * M_PI / numConeSlices;
  float height = 1.99f;

  QVector<QVector3D> conePoints;
  conePoints.push_back(QVector3D(0.0f, 0.0f, height));

  float aspect = (float)m_width / m_height;

  for (uint i = 0; i < numConeSlices; ++i) {
    conePoints.push_back(QVector3D(radius * std::cos(i * angleIncr),
                                   aspect * radius * std::sin(i * angleIncr),
                                   height - radius));
  }

  conePoints.push_back(QVector3D(radius, 0.0f, height - radius));

  m_coneVertices = conePoints.size();

  m_vao->bind();

  m_coneVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  m_coneVBO->create();
  m_coneVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_coneVBO->bind();
  m_coneVBO->allocate(conePoints.constData(),
                      conePoints.size() * sizeof(QVector3D));
  m_coneVBO->release();

  m_shaderProgram->bind();

  m_coneVBO->bind();
  m_shaderProgram->enableAttributeArray(0);
  m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);
  m_coneVBO->release();

  m_shaderProgram->release();

  m_vao->release();
}
