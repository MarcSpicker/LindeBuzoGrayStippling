#include "lbgstippling.h"
#include "voronoidiagram.h"

#include <iostream>

#include <QtMath>

// random jitter for split points
static Random jitterRandom(-0.001, 0.001);

LBGStippling::LBGStippling() {
  // default algorithm parameters
  m_initialPoints = 1;
  m_initialPointSize = 4.0f;

  m_adaptivePointSize = true;
  m_pointSizeMin = 2.0f;
  m_pointSizeMax = 4.0f;

  m_superSamplingFactor = 1;
  m_maxIterations = 50;

  m_hysteresis = 0.6f;
  m_adaptiveHysteresis = true;
  m_adaptiveHysteresisDelta = m_hysteresis / (m_maxIterations - 1);

  m_voro = new VoronoiDiagram();
  connect(m_voro, &VoronoiDiagram::passResult, this,
          &LBGStippling::nextIteration);
}

LBGStippling::~LBGStippling() { delete m_voro; }

void LBGStippling::init(const int w, const int h, const QImage &density) {
  m_width = w;
  m_height = h;
  m_density = density;
  m_iteration = 0;

  m_voro->init(w, h, density, m_superSamplingFactor);
}

float LBGStippling::getSplitValue_Upper(const float pointDiameter,
                                        const float hysteresis) {
  const float pointArea = M_PI * (pointDiameter * pointDiameter / 4.0f);
  return (1.0f + hysteresis / 2.0f) * pointArea * m_superSamplingFactor *
         m_superSamplingFactor;
}

float LBGStippling::getSplitValue_Lower(const float pointDiameter,
                                        const float hysteresis) {
  const float pointArea = M_PI * (pointDiameter * pointDiameter / 4.0f);
  return (1.0f - hysteresis / 2.0f) * pointArea * m_superSamplingFactor *
         m_superSamplingFactor;
}

void LBGStippling::start() {
  m_points.clear();
  m_sizes.clear();
  m_colors.clear();

  m_iteration = 0;

  Random rnd(0.01f, 0.99f);
  for (uint i = 0; i < m_initialPoints; ++i) {
    float x = rnd.next();
    float y = rnd.next();
    m_points.push_back(QVector2D(x, y));
    m_sizes.push_back(m_initialPointSize);
    m_colors.push_back(Qt::black);
  }

  emit displayPoints(m_points, m_sizes, m_colors);
  emit displayStatusMessage(m_iteration + 1, m_points.size(), 0, 0);

  m_voro->calculate(m_points);
}

void LBGStippling::nextIteration(const QVector<VoronoiCell> &cells) {
  if (cells.size() != m_points.size()) {
    std::cerr << "Number of Voronoi cells and points mismatch!" << std::endl;
    return;
  }

  // data from voronoi cells for relaxation (and optional point sizes)
  for (int i = 0; i < cells.size(); ++i) {
    const VoronoiCell &vc = cells[i];
    if (!vc.valid)
      continue;

    m_points[i] = vc.centroid;

    if (m_adaptivePointSize) {
      // point size based on average intensity
      float avgIntensitySqrt = std::sqrt(vc.moment00 / vc.area);
      m_sizes[i] = m_pointSizeMin * (1.0f - avgIntensitySqrt) +
                   m_pointSizeMax * avgIntensitySqrt;
    }
  }

  uint doneSplits = 0;
  uint doneMerges = 0;
  uint doneRetains = 0;

  QVector<QVector2D> lastPoints(m_points);
  QVector<float> lastSizes(m_sizes);

  m_points.clear();
  m_sizes.clear();
  m_colors.clear();

  float curHysteresis = m_hysteresis;
  if (m_adaptiveHysteresis)
    curHysteresis += m_iteration * m_adaptiveHysteresisDelta;

  for (int i = 0; i < lastPoints.size(); ++i) {
    const VoronoiCell &vc = cells[i];
    float totalDensity = vc.moment00;

    QVector2D &curPoint = lastPoints[i];
    float &curSize = lastSizes[i];

    if (totalDensity < getSplitValue_Lower(curSize, curHysteresis)) {
      // cell too small - merge
      ++doneMerges;
      continue;
    }

    if (totalDensity < getSplitValue_Upper(curSize, curHysteresis)) {
      // cell size within acceptable range - keep
      m_points.push_back(curPoint);
      m_sizes.push_back(curSize);
      m_colors.push_back(Qt::black);
      ++doneRetains;
      continue;
    }

    // cell too large - split
    float area = std::max(1.0f, vc.area);
    float circleRadius = std::sqrt(area / M_PI);
    QVector2D splitVector = QVector2D(0.5f * circleRadius, 0.0f);

    float a = vc.orientation;
    QVector2D splitVectorRotated = QVector2D(
        splitVector.x() * std::cos(a) - splitVector.y() * std::sin(a),
        splitVector.y() * std::cos(a) + splitVector.x() * std::sin(a));

    splitVectorRotated.setX(splitVectorRotated.x() / m_width);
    splitVectorRotated.setY(splitVectorRotated.y() / m_height);

    QVector2D splitSeed1 = curPoint - splitVectorRotated;
    QVector2D splitSeed2 = curPoint + splitVectorRotated;

    // check boundaries
    splitSeed1.setX(std::max(0.0f, std::min(splitSeed1.x(), 1.0f)));
    splitSeed1.setY(std::max(0.0f, std::min(splitSeed1.y(), 1.0f)));

    splitSeed2.setX(std::max(0.0f, std::min(splitSeed2.x(), 1.0f)));
    splitSeed2.setY(std::max(0.0f, std::min(splitSeed2.y(), 1.0f)));

    m_points.push_back(splitSeed1 +
                       QVector2D(jitterRandom.next(), jitterRandom.next()));
    m_sizes.push_back(curSize);
    m_colors.push_back(Qt::red);

    m_points.push_back(splitSeed2 +
                       QVector2D(jitterRandom.next(), jitterRandom.next()));
    m_sizes.push_back(curSize);
    m_colors.push_back(Qt::red);

    ++doneSplits;
  }

  emit displayPoints(m_points, m_sizes, m_colors);
  emit displayStatusMessage(m_iteration + 1, m_points.size(), doneSplits,
                            doneMerges);

  // no more changes - finished
  if (doneSplits == 0 && doneMerges == 0) {
    emit finished();
    return;
  }

  // maximum iterations reached - finished
  if (++m_iteration >= m_maxIterations) {
    emit finished();
    return;
  }

  // trigger next iteration
  m_voro->calculate(m_points);
}

void LBGStippling::setInitalPoints(const int numPoints) {
  m_initialPoints = numPoints;
}

void LBGStippling::setInitialPointSize(const double pointSize) {
  m_initialPointSize = pointSize;
}

void LBGStippling::setAdaptivePointSize(const bool enable) {
  m_adaptivePointSize = enable;
}

void LBGStippling::setMinimalPointSize(const double minSize) {
  m_pointSizeMin = minSize;
}

void LBGStippling::setMaximalPointSize(const double maxSize) {
  m_pointSizeMax = maxSize;
}

void LBGStippling::setSuperSampling(const int factor) {
  m_superSamplingFactor = factor;
  // re-initialize the voronoi diagram with the new supersampling factor
  m_voro->init(m_width, m_height, m_density, m_superSamplingFactor);
}

void LBGStippling::setMaxIterations(const int n) { m_maxIterations = n; }

void LBGStippling::setHysteresis(const double hysteresis) {
  m_hysteresis = hysteresis;
  m_adaptiveHysteresisDelta = m_hysteresis / (m_maxIterations - 1);
}

void LBGStippling::setAdaptiveHysteresis(const bool enable) {
  m_adaptiveHysteresis = enable;
}

void LBGStippling::inputImageChanged(const QString &path) {
  QImage img(path);
  init(img.width(), img.height(), img);
}
