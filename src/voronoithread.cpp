#include "voronoithread.h"
#include "voronoidiagram.h"

#include <cmath>
#include <iostream>

#include <QImage>
#include <QOpenGLFunctions>

CellEncoder::CellEncoder() { m_maxEncoded = 0; }

void CellEncoder::encode(int index, GLubyte &r, GLubyte &g, GLubyte &b) const {
  r = ((index >> 16) & 0xff);
  g = ((index >> 8) & 0xff);
  b = ((index >> 0) & 0xff);
}

int CellEncoder::decode(GLubyte &r, GLubyte &g, GLubyte &b, int &index) const {
  int r_ = (int)r;
  int g_ = (int)g;
  int b_ = (int)b;

  index = 0x00000000 | (r_ << 16) | (g_ << 8) | b_;

  if (index > m_maxEncoded) {
    return -1;
  } else
    return 1;
}

void CellEncoder::setMaxEncoded(const int i) { m_maxEncoded = i; }

int CellEncoder::getMaxEncoded() { return m_maxEncoded; }

VoronoiThread::VoronoiThread(const int width, const int height,
                             QOpenGLContext *ctx, QOffscreenSurface *surface,
                             QOpenGLVertexArrayObject *vao,
                             QOpenGLFramebufferObject *fbo,
                             QOpenGLShaderProgram *shader, QImage *density,
                             const QVector<QVector2D> &points,
                             const int coneVertices)
    : m_width(width), m_height(height), m_coneVertices(coneVertices),
      m_densityMap(density), m_surface(surface), m_context(ctx), m_vao(vao),
      m_fbo(fbo), m_shaderProgram(shader) {
  m_context->makeCurrent(m_surface);
  m_gl = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();

  m_encoder = CellEncoder();
  m_encoder.setMaxEncoded(points.size());

  m_numberPoints = points.size();

  QVector<QVector2D> pointsNormalized;
  for (const QVector2D &pt : points) {
    pointsNormalized.push_back(QVector2D(pt.x(), pt.y()));
  }

  m_VBOpositions = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  m_VBOpositions->create();
  m_VBOpositions->setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_VBOpositions->bind();
  m_VBOpositions->allocate(pointsNormalized.constData(),
                           pointsNormalized.size() * sizeof(QVector2D));
  m_VBOpositions->release();

  QVector<QVector3D> colors = initializeColors(points.size());

  m_VBOcolors = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  m_VBOcolors->create();
  m_VBOcolors->setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_VBOcolors->bind();
  m_VBOcolors->allocate(colors.constData(), colors.size() * sizeof(QVector3D));
  m_VBOcolors->release();

  m_vao->bind();

  m_shaderProgram->bind();

  m_VBOpositions->bind();
  m_shaderProgram->enableAttributeArray(1);
  m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2);
  m_gl->glVertexAttribDivisor(1, 1);
  m_VBOpositions->release();

  m_VBOcolors->bind();
  m_shaderProgram->enableAttributeArray(2);
  m_shaderProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3);
  m_gl->glVertexAttribDivisor(2, 1);
  m_VBOcolors->release();

  m_shaderProgram->release();

  m_vao->release();

  m_context->doneCurrent();
  m_context->moveToThread(this);
}

VoronoiThread::~VoronoiThread() {
  delete m_VBOpositions;
  delete m_VBOcolors;
}

QVector<QVector3D> VoronoiThread::initializeColors(const int n) {
  QVector<QVector3D> colors;
  GLubyte r, g, b;
  for (int i = 0; i < n; ++i) {
    m_encoder.encode(i, r, g, b);
    colors.push_back(QVector3D(r / 255.0f, g / 255.0f, b / 255.0f));
  }
  return colors;
}

void VoronoiThread::run() {
  if (!m_context->makeCurrent(m_surface))
    return;

  m_fbo->bind();

  m_gl->glViewport(0, 0, m_width, m_height);

  QMatrix4x4 view;
  view.setToIdentity();
  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);

  m_gl->glDisable(GL_BLEND);
  m_gl->glDisable(GL_POLYGON_SMOOTH);
  m_gl->glDisable(GL_MULTISAMPLE);
  m_gl->glDisable(GL_DITHER);

  m_gl->glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);

  m_gl->glEnable(GL_DEPTH_TEST);

  m_gl->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  m_gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_shaderProgram->bind();

  m_shaderProgram->setUniformValue("view", view);
  m_shaderProgram->setUniformValue("projection", projection);

  m_vao->bind();
  m_gl->glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_coneVertices,
                              m_numberPoints);
  m_vao->release();

  m_shaderProgram->release();

  QImage voronoiDiagram = m_fbo->toImage();
  //  voronoiDiagram.save("voronoiDiagram.png");

  m_fbo->release();

  m_VBOpositions->destroy();
  m_VBOcolors->destroy();

  m_context->doneCurrent();

  QVector<VoronoiCell> cells = QVector<VoronoiCell>(m_encoder.getMaxEncoded());

  GLubyte r, g, b;
  int index, result;
  int wrongIndices = 0;
  float density;

  QRgb densityPixel;
  QRgb voroPixel;

  const int w = m_fbo->width();
  const int h = m_fbo->height();

  // compute voronoi cell moments
  for (int y = 0; y < h; ++y) {

    for (int x = 0; x < w; ++x) {
      densityPixel = m_densityMap->pixel(x, y);
      voroPixel = voronoiDiagram.pixel(x, y);

      r = qRed(voroPixel);
      g = qGreen(voroPixel);
      b = qBlue(voroPixel);

      density = std::max(1.0f - qGray(densityPixel) / 255.0f,
                         std::numeric_limits<float>::epsilon());

      result = m_encoder.decode(r, g, b, index);
      if (result == -1) {
        // wrong index - skip
        ++wrongIndices;
        continue;
      }

      VoronoiCell &cell = cells[index];
      cell.area++;
      cell.moment00 += density;
      cell.moment10 += x * density;
      cell.moment01 += y * density;
      cell.moment11 += x * y * density;
      cell.moment20 += x * x * density;
      cell.moment02 += y * y * density;
    }
  }

  if (wrongIndices > 0) {
    std::cerr << "Found " << wrongIndices
              << " wrong indices in Voronoi Diagram!" << std::endl;
  }

  float x, y, z;

  // compute cell quantities
  for (VoronoiCell &cell : cells) {
    if (cell.moment00 <= 0.0f) {
      cell.valid = false;
      continue;
    }

    cell.valid = true;

    // centroid
    cell.centroid.setX(cell.moment10 / cell.moment00);
    cell.centroid.setY(cell.moment01 / cell.moment00);

    // orientation
    x = cell.moment20 / cell.moment00 - cell.centroid.x() * cell.centroid.x();
    y = 2 *
        (cell.moment11 / cell.moment00 - cell.centroid.x() * cell.centroid.y());
    z = cell.moment02 / cell.moment00 - cell.centroid.y() * cell.centroid.y();
    cell.orientation = std::atan2(y, x - z) / 2.0f;

    cell.centroid.setX((cell.centroid.x() + 0.5f) / m_width);
    cell.centroid.setY((cell.centroid.y() + 0.5f) / m_height);
  }

  emit finished(cells);
}
