#include "lbgstippling.h"
#include "voronoidiagram.h"

#include <iostream>

#include <QtMath>

// random jitter for split points
static Random jitterRandom(-0.001, 0.001);

LBGStippling::LBGStippling() {
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
}

float LBGStippling::getSplitValue_Upper(const float pointDiameter,
                                        const float hysteresis) {
  const float pointArea = M_PI * (pointDiameter * pointDiameter / 4.0f);
  return (1.0f + hysteresis / 2.0f) * pointArea * m_params.superSamplingFactor *
         m_params.superSamplingFactor;
}

float LBGStippling::getSplitValue_Lower(const float pointDiameter,
                                        const float hysteresis) {
  const float pointArea = M_PI * (pointDiameter * pointDiameter / 4.0f);
  return (1.0f - hysteresis / 2.0f) * pointArea * m_params.superSamplingFactor *
         m_params.superSamplingFactor;
}

void LBGStippling::start(const StipplingParams &params) {
  m_params = params;
  m_voro->init(m_width, m_height, m_density, m_params.superSamplingFactor);
  m_adaptiveHysteresisDelta = params.hysteresis / (params.maxIterations - 1);

  m_points.clear();
  m_sizes.clear();
  m_colors.clear();

  m_iteration = 0;

  Random rnd(0.01, 0.99);
  for (int i = 0; i < m_params.initialPoints; ++i) {
    float x = rnd.next();
    float y = rnd.next();
    m_points.push_back(QVector2D(x, y));
    m_sizes.push_back(m_params.initialPointSize);
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

    if (m_params.adaptivePointSize) {
      // point size based on average intensity
      float avgIntensitySqrt = std::sqrt(vc.moment00 / vc.area);
      m_sizes[i] = m_params.pointSizeMin * (1.0f - avgIntensitySqrt) +
                   m_params.pointSizeMax * avgIntensitySqrt;
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

  float curHysteresis = m_params.hysteresis;
  if (m_params.adaptiveHysteresis)
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

  // no more changes or maximum iterations reached - finished
  if ((doneSplits == 0 && doneMerges == 0) ||
      (++m_iteration >= m_params.maxIterations)) {
    emit finished();
    return;
  }

  // trigger next iteration
  m_voro->calculate(m_points);
}

void LBGStippling::inputImageChanged(const QString &path) {
  QImage img(path);
  init(img.width(), img.height(), img);
}
