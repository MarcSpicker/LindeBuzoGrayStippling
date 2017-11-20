#ifndef LBGSTIPPLING_H
#define LBGSTIPPLING_H

#include <random>

#include <QImage>
#include <QObject>
#include <QVector2D>
#include <QVector>

class VoronoiCell;
class VoronoiDiagram;

class Random {
public:
  Random(double low, double high) {
    rd = new std::random_device;
    gen = new std::mt19937((*rd)());
    dis = new std::uniform_real_distribution<double>(low, high);
  }
  ~Random() {
    delete dis;
    delete gen;
    delete rd;
  }

  double next() const { return (*dis)(*gen); }

private:
  std::random_device *rd;
  std::mt19937 *gen;
  std::uniform_real_distribution<double> *dis;
};

class LBGStippling : public QObject {

  Q_OBJECT

public:
  LBGStippling();
  ~LBGStippling();
  void init(const int w, const int h, const QImage &density);

private:
  float getSplitValue_Upper(const float pointDiameter, const float hysteresis);
  float getSplitValue_Lower(const float pointDiameter, const float hysteresis);

signals:
  void displayPoints(const QVector<QVector2D> &points,
                     const QVector<float> &sizes,
                     const QVector<QColor> &colors);
  void saveImagePNG(const QString &path);
  void saveImageSVG(const QString &path);
  void displayStatusMessage(const int iteration, const int numberPoints,
                            const int splits, const int merges);
  void finished();

public slots:
  void nextIteration(const QVector<VoronoiCell> &cells);
  void start();

  void setInitalPoints(const int numPoints);
  void setInitialPointSize(const double pointSize);
  void setAdaptivePointSize(const bool enable);
  void setMinimalPointSize(const double minSize);
  void setMaximalPointSize(const double maxSize);
  void setSuperSampling(const int factor);
  void setMaxIterations(const int n);
  void setHysteresis(const double hysteresis);
  void setAdaptiveHysteresis(const bool enable);

  void inputImageChanged(const QString &path);

private:
  // algorithm parameters
  uint m_initialPoints;
  double m_initialPointSize;

  bool m_adaptivePointSize;
  double m_pointSizeMin;
  double m_pointSizeMax;

  uint m_superSamplingFactor;
  uint m_maxIterations;

  double m_hysteresis;
  bool m_adaptiveHysteresis;
  double m_adaptiveHysteresisDelta;

  // internals
  int m_width;
  int m_height;
  QImage m_density;

  uint m_iteration;
  QVector<QVector2D> m_points;
  QVector<float> m_sizes;
  QVector<QColor> m_colors;

  VoronoiDiagram *m_voro;
};

#endif // LBGSTIPPLING_H
