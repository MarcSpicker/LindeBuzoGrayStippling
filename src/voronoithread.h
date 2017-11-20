#ifndef VORONOITHREAD_H
#define VORONOITHREAD_H

#include <QImage>
#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QThread>

class VoronoiCell;

class CellEncoder {

  int m_maxEncoded;

public:
  CellEncoder();
  void encode(int index, GLubyte &r, GLubyte &g, GLubyte &b) const;
  int decode(GLubyte &r, GLubyte &g, GLubyte &b, int &index) const;
  void setMaxEncoded(const int i);
  int getMaxEncoded();
};

class VoronoiThread : public QThread {

  Q_OBJECT

public:
  VoronoiThread(const int width, const int height, QOpenGLContext *ctx,
                QOffscreenSurface *surface, QOpenGLVertexArrayObject *vao,
                QOpenGLFramebufferObject *fbo, QOpenGLShaderProgram *shader,
                QImage *density, const QVector<QVector2D> &points,
                const int coneVertices);
  ~VoronoiThread();
  void run() override;

private:
  int m_width;
  int m_height;

  int m_coneVertices;
  int m_numberPoints;

  CellEncoder m_encoder;
  QImage *m_densityMap;

  QOffscreenSurface *m_surface;
  QOpenGLContext *m_context;
  QOpenGLVertexArrayObject *m_vao;
  QOpenGLFramebufferObject *m_fbo;
  QOpenGLShaderProgram *m_shaderProgram;
  QOpenGLBuffer *m_VBOpositions;
  QOpenGLBuffer *m_VBOcolors;

  QOpenGLFunctions_3_3_Core *m_gl;

  QVector<QVector3D> initializeColors(const int n);

signals:
  void finished(const QVector<VoronoiCell> &cells);
};

#endif // VORONOITHREAD_H
