#ifndef STIPPLEVIEWER_H
#define STIPPLEVIEWER_H

#include <QtWidgets>

class StippleViewer : public QGraphicsView {

public:
  StippleViewer(QWidget *parent);
  ~StippleViewer();
  void init(const QImage &img);

public slots:
  void displayPoints(const QVector<QVector2D> &points,
                     const QVector<float> &sizes,
                     const QVector<QColor> &colors);

  void saveImagePNG(const QString &path);
  void saveImageSVG(const QString &path);
  void inputImageChanged(const QString &path);

private:
  QWidget *m_parent;
  QGraphicsScene *m_scene;
  int m_width;
  int m_height;
};

#endif // STIPPLEVIEWER_H
