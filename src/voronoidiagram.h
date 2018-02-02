#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H

#include <QImage>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

class IndexMap {
  public:
    size_t width;
    size_t height;

    IndexMap(size_t w, size_t h, size_t count);
    void set(size_t x, size_t y, uint32_t value);
    uint32_t get(size_t x, size_t y) const;
    size_t count() const;

  private:
    size_t m_numEncoded;
    std::vector<uint32_t> m_data;
};

class VoronoiDiagram {

  public:
    VoronoiDiagram(QImage& density);
    ~VoronoiDiagram();

    IndexMap calculate(const QVector<QVector2D>& points);

  private:
    int m_coneVertices;

    QOpenGLContext* m_context;
    QOffscreenSurface* m_surface;
    QOpenGLVertexArrayObject* m_vao;
    QOpenGLShaderProgram* m_shaderProgram;
    QOpenGLFramebufferObject* m_fbo;
    QImage m_densityMap;

    QVector<QVector3D> createConeDrawingData(const QSize& size);
};

#endif // VORONOIDIAGRAM_H
