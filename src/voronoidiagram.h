#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H

#include <QImage>
#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

class VoronoiCell {

public:
  VoronoiCell()
      : centroid(QVector2D(0.0f, 0.0f)), orientation(0.0f), valid(false),
        area(0.0f), moment00(0.0f), moment10(0.0f), moment01(0.0f),
        moment11(0.0f), moment20(0.0f), moment02(0.0f) {}

  QVector2D centroid;
  float orientation;
  bool valid;

  float area;
  float moment00;
  float moment10;
  float moment01;
  float moment11;
  float moment20;
  float moment02;
};

Q_DECLARE_METATYPE(VoronoiCell)

class VoronoiThread;

class VoronoiDiagram : public QObject {

  Q_OBJECT

public:
  VoronoiDiagram();
  ~VoronoiDiagram();
  void init(const int width, const int height, QImage density,
            const int superSampling = 1);
  void calculate(const QVector<QVector2D> &points);

private:
  int m_width;
  int m_height;
  int m_superSampling;

  int m_coneVertices;

  bool m_initialized;
  bool m_running;

  VoronoiThread *m_thread;
  QOpenGLContext *m_context;
  QOffscreenSurface *m_surface;
  QOpenGLShaderProgram *m_shaderProgram;
  QOpenGLVertexArrayObject *m_vao;
  QOpenGLFramebufferObject *m_fbo;
  QOpenGLBuffer *m_coneVBO;
  QImage *m_densityMap;

  void initConeDrawingData();

signals:
  void passResult(const QVector<VoronoiCell> &cells);

public slots:
  void finished(const QVector<VoronoiCell> &cells);
};

#endif // VORONOIDIAGRAM_H
