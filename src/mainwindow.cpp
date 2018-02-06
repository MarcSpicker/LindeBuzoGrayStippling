#include "mainwindow.h"
#include "settingswidget.h"
#include "stippleviewer.h"

MainWindow::MainWindow() {
    QImage density = QImage(":/input/input1.jpg");

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    m_stippleViewer = new StippleViewer(density, this);
    m_stippleViewer->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setCentralWidget(m_stippleViewer);

    QDockWidget* dockWidget = new QDockWidget("Algorithm Parameters", this);
    dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    SettingsWidget* settingsWidget = new SettingsWidget(m_stippleViewer, dockWidget);
    dockWidget->setWidget(settingsWidget);
    connect(dockWidget, &QDockWidget::topLevelChanged, [this](bool) { resize(sizeHint()); });

    m_statusBar = new QStatusBar(this);
    m_statusBar->setSizeGripEnabled(false);
    setStatusBar(m_statusBar);

    connect(m_stippleViewer, &StippleViewer::iterationStatus,
            [this](int iteration, int numberPoints, int splits, int merges, float hysteresis) {
                m_statusBar->showMessage(
                    "Iteration: " + QString::number(iteration) +
                    " | Number points: " + QString::number(numberPoints) +
                    " | Current hysteresis: " + QString::number(hysteresis, 'f', 2) +
                    " | Splits: " + QString::number(splits) +
                    " | Merges: " + QString::number(merges));
            });
    connect(m_stippleViewer, &StippleViewer::inputImageChanged,
            [this]() { m_statusBar->clearMessage(); });
}
