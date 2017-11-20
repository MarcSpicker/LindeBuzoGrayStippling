#include "stippleviewer.h"

#include <iostream>

#include <QSvgGenerator>

StippleViewer::StippleViewer(QWidget *parent)
    : QGraphicsView(parent), m_parent(parent) {
  setInteractive(false);
  setRenderHint(QPainter::Antialiasing, true);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setOptimizationFlag(QGraphicsView::DontSavePainterState);
  setFrameStyle(0);
  setAttribute(Qt::WA_TranslucentBackground, false);
  setCacheMode(QGraphicsView::CacheBackground);

  m_scene = new QGraphicsScene(m_parent);
  setScene(m_scene);
}

StippleViewer::~StippleViewer() { delete m_scene; }

void StippleViewer::init(const QImage &img) {
  m_width = img.width();
  m_height = img.height();

  delete m_scene;

  m_scene = new QGraphicsScene(m_parent);
  m_scene->setSceneRect(QRectF(0, 0, m_width, m_height));
  m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  m_scene->addPixmap(QPixmap::fromImage(img));
  setScene(m_scene);

  setMinimumSize(m_width, m_height);
  resize(m_width, m_height);
}

void StippleViewer::displayPoints(const QVector<QVector2D> &points,
                                  const QVector<float> &sizes,
                                  const QVector<QColor> &colors) {
  if (points.size() != sizes.size() || points.size() != colors.size()) {
    std::cerr << "Number of Stipple positions, sizes, and colors varies!"
              << std::endl;
    return;
  }

  delete m_scene;

  m_scene = new QGraphicsScene(m_parent);
  m_scene->setSceneRect(QRectF(0, 0, m_width, m_height));
  m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

  for (int i = 0; i < points.size(); ++i) {
    const QVector2D &v = points[i];
    const float &size = sizes[i];
    const QColor &color = colors[i];
    m_scene->addEllipse(v.x() * m_width, v.y() * m_height, size, size,
                        Qt::NoPen, color);
  }
  setScene(m_scene);
}

void StippleViewer::saveImagePNG(const QString &path) {
  QPixmap pixmap = grab();
  pixmap.save(path);
}

void StippleViewer::saveImageSVG(const QString &path) {
  QSvgGenerator generator;
  generator.setFileName(path);
  generator.setSize(QSize(m_scene->width(), m_scene->height()));
  generator.setViewBox(m_scene->sceneRect());
  generator.setTitle("Stippling Result");
  generator.setDescription(
      "SVG File created by Weighted Linde-Buzo-Gray Stippling");
  QPainter painter;
  painter.begin(&generator);
  m_scene->render(&painter);
  painter.end();
}

void StippleViewer::inputImageChanged(const QString &path) {
  init(QImage(path));
}
