#include "stippleviewer.h"

#include <cassert>
#include <iostream>

#include <QCoreApplication>
#include <QSvgGenerator>

StippleViewer::StippleViewer(const QImage& img, QWidget* parent)
    : QGraphicsView(parent), m_image(img) {
    setInteractive(false);
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState);
    setFrameStyle(0);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setCacheMode(QGraphicsView::CacheBackground);

    setScene(new QGraphicsScene(this));
    this->scene()->setSceneRect(m_image.rect());
    this->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->scene()->addPixmap(QPixmap::fromImage(m_image));

    m_stippling = LBGStippling();
    m_stippling.setStatusCallback([this](const auto& status) {
        emit iterationStatus(status.iteration + 1, status.size, status.splits, status.merges);
    });

    m_stippling.setStippleCallback([this](const auto& stipples) { displayPoints(stipples); });
}

void StippleViewer::displayPoints(const std::vector<Stipple>& stipples) {
    this->scene()->clear();
    for (const auto& s : stipples) {
        auto x = s.pos.x() * m_image.width() - s.size / 2.0;
        auto y = s.pos.y() * m_image.height() - s.size / 2.0;
        this->scene()->addEllipse(x, y, s.size, s.size, Qt::NoPen, s.color);
    }
    // TODO: Fix event handling
    QCoreApplication::processEvents();
}

QPixmap StippleViewer::getImage() {
    QPixmap pixmap(this->scene()->sceneRect().size().toSize()); 
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    this->scene()->render(&painter);
    return pixmap;
}

void StippleViewer::saveImageSVG(const QString& path) {
    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(QSize(this->scene()->width(), this->scene()->height()));
    generator.setViewBox(this->scene()->sceneRect());
    generator.setTitle("Stippling Result");
    generator.setDescription("SVG File created by Weighted Linde-Buzo-Gray Stippling");
    QPainter painter;
    painter.begin(&generator);
    this->scene()->render(&painter);
    painter.end();
}

void StippleViewer::setInputImage(const QImage& img) {
    m_image = img;
    this->scene()->clear();
    this->scene()->addPixmap(QPixmap::fromImage(m_image));
    this->scene()->setSceneRect(m_image.rect());

    auto w = m_image.width();
    auto h = m_image.height();

    setMinimumSize(w, h);
    adjustSize();

    emit inputImageChanged();
}

void StippleViewer::stipple(const LBGStippling::Params& params) {
    // TODO: Handle return value
    m_stippling.stipple(m_image, params);
    finished();
}
