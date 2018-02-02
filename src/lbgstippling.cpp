#include "lbgstippling.h"
#include "voronoicell.h"

#include <cassert>

#include <QtMath>

namespace Random {
std::random_device rd;
std::mt19937 gen(rd());
} // namespace Random

using Params = LBGStippling::Params;
using Status = LBGStippling::Status;

QVector<QVector2D> sites(const std::vector<Stipple>& stipples) {
    QVector<QVector2D> sites(stipples.size());
    std::transform(stipples.begin(), stipples.end(), sites.begin(),
                   [](const auto& s) { return s.pos; });
    return sites;
}

std::vector<Stipple> randomStipples(size_t n, float size) {
    std::uniform_real_distribution<float> dis(0.01f, 0.99f);
    std::vector<Stipple> stipples(n);
    std::generate(stipples.begin(), stipples.end(), [&]() {
        return Stipple{QVector2D(dis(Random::gen), dis(Random::gen)), size, Qt::black};
    });
    return stipples;
}

template <class T>
inline T pow2(T x) {
    return x * x;
}

QVector2D jitter(QVector2D s) {
    using namespace Random;
    std::uniform_real_distribution<float> jitter_dis(-0.001f, 0.001f);
    return s += QVector2D(jitter_dis(gen), jitter_dis(gen));
}

float getSplitValueUpper(float pointDiameter, float hysteresis, size_t superSampling) {
    const float pointArea = M_PI * pow2(pointDiameter / 2.0f);
    return (1.0f + hysteresis / 2.0f) * pointArea * pow2(superSampling);
}

float getSplitValueLower(float pointDiameter, float hysteresis, size_t superSampling) {
    const float pointArea = M_PI * pow2(pointDiameter / 2.0f);
    return (1.0f - hysteresis / 2.0f) * pointArea * pow2(superSampling);
}

float stippleSize(const VoronoiCell& cell, const Params& params) {
    if (params.adaptivePointSize) {
        float avgIntensitySqrt = std::sqrt(cell.sumDensity / cell.area);
        return params.pointSizeMin * (1.0f - avgIntensitySqrt) +
               params.pointSizeMax * avgIntensitySqrt;
    } else {
        return params.initialPointSize;
    }
}

float currentHysteresis(size_t i, const Params& params) {
    if (params.adaptiveHysteresis) {
        float delta = params.hysteresis / (params.maxIterations - 1);
        return params.hysteresis + i * delta;
    } else {
        return params.hysteresis;
    }
}

bool notFinished(const Status& status, const Params& params) {
    auto[iteration, size, splits, merges] = status;
    return !((splits == 0 && merges == 0) || (iteration == params.maxIterations));
}

LBGStippling::LBGStippling() {
    m_statusCallback = [](const Status&) {};
    m_stippleCallback = [](const std::vector<Stipple>&) {};
}

void LBGStippling::setStatusCallback(Report<Status> statusCB) {
    m_statusCallback = statusCB;
}

void LBGStippling::setStippleCallback(Report<std::vector<Stipple>> stippleCB) {
    m_stippleCallback = stippleCB;
}

std::vector<Stipple> LBGStippling::stipple(const QImage& density, const Params& params) const {
    QImage densityGray =
        density
            .scaledToWidth(params.superSamplingFactor * density.width(), Qt::SmoothTransformation)
            .convertToFormat(QImage::Format_Grayscale8);

    VoronoiDiagram voronoi(densityGray);

    std::vector<Stipple> stipples = randomStipples(params.initialPoints, params.initialPointSize);

    Status status = {0, 0, 1, 1};

    while (notFinished(status, params)) {
        status.splits = 0;
        status.merges = 0;
        auto indexMap = voronoi.calculate(sites(stipples));
        std::vector<VoronoiCell> cells = accumulateCells(indexMap, densityGray);

        assert(cells.size() == stipples.size());

        stipples.clear();

        float hysteresis = currentHysteresis(status.iteration, params);

        for (const auto& cell : cells) {
            float totalDensity = cell.sumDensity;
            float diameter = stippleSize(cell, params);

            if (totalDensity <
                    getSplitValueLower(diameter, hysteresis, params.superSamplingFactor) ||
                cell.area == 0.0f) {
                // cell too small - merge
                ++status.merges;
                continue;
            }

            if (totalDensity <
                getSplitValueUpper(diameter, hysteresis, params.superSamplingFactor)) {
                // cell size within acceptable range - keep
                stipples.push_back({cell.centroid, diameter, Qt::black});
                continue;
            }

            // cell too large - split
            float area = std::max(1.0f, cell.area);
            float circleRadius = std::sqrt(area / M_PI);
            QVector2D splitVector = QVector2D(0.5f * circleRadius, 0.0f);

            float a = cell.orientation;
            QVector2D splitVectorRotated =
                QVector2D(splitVector.x() * std::cos(a) - splitVector.y() * std::sin(a),
                          splitVector.y() * std::cos(a) + splitVector.x() * std::sin(a));

            splitVectorRotated.setX(splitVectorRotated.x() / densityGray.width());
            splitVectorRotated.setY(splitVectorRotated.y() / densityGray.height());

            QVector2D splitSeed1 = cell.centroid - splitVectorRotated;
            QVector2D splitSeed2 = cell.centroid + splitVectorRotated;

            // check boundaries
            splitSeed1.setX(std::max(0.0f, std::min(splitSeed1.x(), 1.0f)));
            splitSeed1.setY(std::max(0.0f, std::min(splitSeed1.y(), 1.0f)));

            splitSeed2.setX(std::max(0.0f, std::min(splitSeed2.x(), 1.0f)));
            splitSeed2.setY(std::max(0.0f, std::min(splitSeed2.y(), 1.0f)));

            stipples.push_back({jitter(splitSeed1), diameter, Qt::red});
            stipples.push_back({jitter(splitSeed2), diameter, Qt::red});

            ++status.splits;
        }
        status.size = stipples.size();
        m_stippleCallback(stipples);
        m_statusCallback(status);

        ++status.iteration;
    }
    return stipples;
}
