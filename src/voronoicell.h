#ifndef VORONOICELL_H
#define VORONOICELL_H

#include <QVector2D>

class IndexMap;

struct VoronoiCell {
    QVector2D centroid;
    float orientation;
    float area;
    float sumDensity;
};

std::vector<VoronoiCell> accumulateCells(const IndexMap& map, const QImage& density);

#endif // VORONOICELL_H
