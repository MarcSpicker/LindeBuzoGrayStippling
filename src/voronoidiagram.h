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
  int32_t width;
  int32_t height;

  IndexMap(int32_t w, int32_t h, int32_t count);
  void set(const int32_t x, const int32_t y, const uint32_t value);
  uint32_t get(int32_t x, const int32_t y) const;
  int32_t count() const;

 private:
  int32_t m_numEncoded;
  QVector<uint32_t> m_data;
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

#endif  // VORONOIDIAGRAM_H
