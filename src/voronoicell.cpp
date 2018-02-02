#include "voronoicell.h"
#include "voronoidiagram.h"

#include <cmath>

struct Moments {
    float moment00;
    float moment10;
    float moment01;
    float moment11;
    float moment20;
    float moment02;
};

std::vector<VoronoiCell> accumulateCells(const IndexMap& map, const QImage& density) {
    // compute voronoi cell moments
    std::vector<VoronoiCell> cells = std::vector<VoronoiCell>(map.count());
    std::vector<Moments> moments = std::vector<Moments>(map.count());

    for (size_t x = 0; x < map.width; ++x) {
        for (size_t y = 0; y < map.height; ++y) {
            size_t index = map.get(x, y);

            QRgb densityPixel = density.pixel(x, y);
            float density = std::max(1.0f - qGray(densityPixel) / 255.0f,
                                     std::numeric_limits<float>::epsilon());

            VoronoiCell& cell = cells[index];
            cell.area++;
            cell.sumDensity += density;

            Moments& m = moments[index];
            m.moment00 += density;
            m.moment10 += x * density;
            m.moment01 += y * density;
            m.moment11 += x * y * density;
            m.moment20 += x * x * density;
            m.moment02 += y * y * density;
        }
    }

    // compute cell quantities
    for (size_t i = 0; i < cells.size(); ++i) {
        VoronoiCell& cell = cells[i];
        if (cell.sumDensity <= 0.0f) 
            continue;

        auto[m00, m10, m01, m11, m20, m02] = moments[i];

        // centroid
        cell.centroid.setX(m10 / m00);
        cell.centroid.setY(m01 / m00);

        // orientation
        float x = m20 / m00 - cell.centroid.x() * cell.centroid.x();
        float y = 2.0f * (m11 / m00 - cell.centroid.x() * cell.centroid.y());
        float z = m02 / m00 - cell.centroid.y() * cell.centroid.y();
        cell.orientation = std::atan2(y, x - z) / 2.0f;

        cell.centroid.setX((cell.centroid.x() + 0.5f) / density.width());
        cell.centroid.setY((cell.centroid.y() + 0.5f) / density.height());
    }
    return cells;
}
