#ifndef LBGSTIPPLING_H
#define LBGSTIPPLING_H

#include "voronoidiagram.h"

#include <QImage>
#include <QVector2D>

// TODO: Color is only used for debugging
struct Stipple {
  QVector2D pos;
  float size;
  QColor color;
};

class LBGStippling {
 public:
  struct Params {
    size_t initialPoints = 1;
    float initialPointSize = 4.0f;

    bool adaptivePointSize = true;
    float pointSizeMin = 2.0f;
    float pointSizeMax = 4.0f;

    size_t superSamplingFactor = 1;
    size_t maxIterations = 50;

    float hysteresis = 0.6f;
    float hysteresisDelta = 0.01f;
  };

  struct Status {
    size_t iteration;
    size_t size;
    size_t splits;
    size_t merges;
    float hysteresis;
  };

  template <class T>
  using Report = std::function<void(const T&)>;

  LBGStippling();

  std::vector<Stipple> stipple(const QImage& density,
                               const Params& params) const;

  // TODO: Rename and method chaining.
  void setStatusCallback(Report<Status> statusCB);
  void setStippleCallback(Report<std::vector<Stipple>> stippleCB);

 private:
  Report<Status> m_statusCallback;
  Report<std::vector<Stipple>> m_stippleCallback;
};

#endif  // LBGSTIPPLING_H
