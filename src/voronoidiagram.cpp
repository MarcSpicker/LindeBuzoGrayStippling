#include "voronoidiagram.h"

#include <cassert>
#include <cmath>

#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_3_3_Core>

#include "shader/Voronoi.frag.h"
#include "shader/Voronoi.vert.h"

const float pi = 3.14159265358979323846f;

////////////////////////////////////////////////////////////////////////////////
/// Cell Encoder

namespace CellEncoder {
QVector3D encode(const uint32_t index) {
    uint32_t r = (index >> 16) & 0xff;
    uint32_t g = (index >> 8) & 0xff;
    uint32_t b = (index >> 0) & 0xff;
    return QVector3D(r / 255.0f, g / 255.0f, b / 255.0f);
}

uint32_t decode(const uint8_t& r, const uint8_t& g, const uint8_t& b) {
    return 0x00000000 | (r << 16) | (g << 8) | b;
}
} // namespace CellEncoder

////////////////////////////////////////////////////////////////////////////////
/// Index Map

IndexMap::IndexMap(size_t w, size_t h, size_t count) : width(w), height(h), m_numEncoded(count) {
    m_data = std::vector<uint32_t>(w * h);
}

void IndexMap::set(size_t x, size_t y, uint32_t value) {
    m_data[y * width + x] = value;
}

uint32_t IndexMap::get(size_t x, size_t y) const {
    return m_data[y * width + x];
}

size_t IndexMap::count() const {
    return m_numEncoded;
}

////////////////////////////////////////////////////////////////////////////////
/// Voronoi Diagram

VoronoiDiagram::VoronoiDiagram(QImage& density) : m_densityMap(density) {
    m_context = new QOpenGLContext();
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    m_context->setFormat(format);
    m_context->create();

    m_surface = new QOffscreenSurface(Q_NULLPTR, m_context);
    m_surface->setFormat(m_context->format());
    m_surface->create();

    m_context->makeCurrent(m_surface);

    m_vao = new QOpenGLVertexArrayObject(m_context);
    m_vao->create();

    m_shaderProgram = new QOpenGLShaderProgram(m_context);
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, voronoiVertex.c_str());
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, voronoiFragment.c_str());
    m_shaderProgram->link();

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    m_fbo = new QOpenGLFramebufferObject(m_densityMap.width(), m_densityMap.height(), fboFormat);
    QVector<QVector3D> cones = createConeDrawingData(m_densityMap.size());

    m_vao->bind();

    QOpenGLBuffer coneVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    coneVBO.create();
    coneVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
    coneVBO.bind();
    coneVBO.allocate(cones.constData(), cones.size() * sizeof(QVector3D));
    coneVBO.release();

    m_shaderProgram->bind();

    coneVBO.bind();
    m_shaderProgram->enableAttributeArray(0);
    m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    coneVBO.release();

    m_shaderProgram->release();

    m_vao->release();
}

VoronoiDiagram::~VoronoiDiagram() {
    delete m_fbo;
    delete m_context;
}

IndexMap VoronoiDiagram::calculate(const QVector<QVector2D>& points) {
    assert(!points.empty());

    m_context->makeCurrent(m_surface);

    QOpenGLFunctions_3_3_Core* gl = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();

    m_vao->bind();

    m_shaderProgram->bind();

    QOpenGLBuffer vboPositions = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vboPositions.create();
    vboPositions.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vboPositions.bind();
    vboPositions.allocate(points.constData(), points.size() * sizeof(QVector2D));
    m_shaderProgram->enableAttributeArray(1);
    m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2);
    gl->glVertexAttribDivisor(1, 1);
    vboPositions.release();

    std::vector<QVector3D> colors(points.size());
    std::generate(colors.begin(),
                  colors.end(), [n = 0]() mutable { return CellEncoder::encode(n++); });

    QOpenGLBuffer vboColors = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vboColors.create();
    vboColors.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vboColors.bind();
    vboColors.allocate(colors.data(), colors.size() * sizeof(QVector3D));
    m_shaderProgram->enableAttributeArray(2);
    m_shaderProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3);
    gl->glVertexAttribDivisor(2, 1);
    vboColors.release();

    m_fbo->bind();

    gl->glViewport(0, 0, m_densityMap.width(), m_densityMap.height());

    gl->glDisable(GL_MULTISAMPLE);
    gl->glDisable(GL_DITHER);

    gl->glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);

    gl->glEnable(GL_DEPTH_TEST);

    gl->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_coneVertices, points.size());

    m_shaderProgram->release();

    m_vao->release();

    QImage voronoiDiagram = m_fbo->toImage();
    //  voronoiDiagram.save("voronoiDiagram.png");

    m_fbo->release();
    m_context->doneCurrent();

    IndexMap idxMap(m_fbo->width(), m_fbo->height(), points.size());

    for (int y = 0; y < m_fbo->height(); ++y) {
        for (int x = 0; x < m_fbo->width(); ++x) {
            QRgb voroPixel = voronoiDiagram.pixel(x, y);

            uint8_t r = qRed(voroPixel);
            uint8_t g = qGreen(voroPixel);
            uint8_t b = qBlue(voroPixel);

            size_t index = CellEncoder::decode(r, g, b);
            assert(index <= points.size());

            idxMap.set(x, y, index);
        }
    }
    return idxMap;
}

// Calculate the number of slices required to ensure the given max. meshing error.
// See "Fast Computation of Generalized Voronoi Diagram Using Graphics Hardware",
// Hoff et. al., Proc. of SIGGRAPH 99.

uint calcNumConeSlices(const float radius, const float maxError) {
    float alpha = 2.0f * std::acos((radius - maxError) / radius);
    return (unsigned)(2 * pi / alpha + 0.5f);
}

QVector<QVector3D> VoronoiDiagram::createConeDrawingData(const QSize& size) {
    float radius = std::sqrt(2.0f);
    float maxError = 1.0f / (size.width() > size.height() ? size.width() : size.height());
    uint numConeSlices = calcNumConeSlices(radius, maxError);

    float angleIncr = 2.0f * pi / numConeSlices;
    float height = 1.99f;

    QVector<QVector3D> conePoints;
    conePoints.push_back(QVector3D(0.0f, 0.0f, height));

    float aspect = (float)size.width() / size.height();

    for (uint i = 0; i < numConeSlices; ++i) {
        conePoints.push_back(QVector3D(radius * std::cos(i * angleIncr),
                                       aspect * radius * std::sin(i * angleIncr), height - radius));
    }

    conePoints.push_back(QVector3D(radius, 0.0f, height - radius));

    m_coneVertices = conePoints.size();
    return conePoints;
}
