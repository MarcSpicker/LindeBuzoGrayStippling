#include "mainwindow.h"
#include "lbgstippling.h"
#include "settingswidget.h"
#include "stippleviewer.h"
#include "voronoidiagram.h"

#include <iostream>

MainWindow::MainWindow() {
  QImage density = QImage(":/input/input1.jpg");

  const int w = density.width();
  const int h = density.height();

  layout()->setSizeConstraint(QLayout::SetFixedSize);

  QWidget *stippleWidget = new QWidget(this);
  QVBoxLayout *stippleWidgetLayout = new QVBoxLayout(stippleWidget);
  stippleWidgetLayout->setSizeConstraint(QLayout::SetFixedSize);
  m_stippleViewer = new StippleViewer(this);
  m_stippleViewer->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_stippleViewer->init(density);
  stippleWidgetLayout->addWidget(m_stippleViewer);
  stippleWidget->setLayout(stippleWidgetLayout);

  setCentralWidget(stippleWidget);

  m_LBGStippling = new LBGStippling();
  connect(m_LBGStippling, &LBGStippling::displayPoints, m_stippleViewer,
          &StippleViewer::displayPoints);
  connect(m_LBGStippling, &LBGStippling::saveImagePNG, m_stippleViewer,
          &StippleViewer::saveImagePNG);
  connect(m_LBGStippling, &LBGStippling::saveImageSVG, m_stippleViewer,
          &StippleViewer::saveImageSVG);
  m_LBGStippling->init(w, h, density);

  QDockWidget *dockWidget = new QDockWidget("Algorithm Parameters", this);
  dockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
                          QDockWidget::DockWidgetMovable);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

  SettingsWidget *settingsWidget =
      new SettingsWidget(m_LBGStippling, m_stippleViewer, dockWidget);
  dockWidget->setWidget(settingsWidget);
  connect(dockWidget, SIGNAL(topLevelChanged(bool)), this,
          SLOT(resizeOnDocking(bool)));
  connect(settingsWidget, &SettingsWidget::openInputImage, m_stippleViewer,
          &StippleViewer::inputImageChanged);
  connect(settingsWidget, &SettingsWidget::openInputImage, m_LBGStippling,
          &LBGStippling::inputImageChanged);
  connect(settingsWidget, &SettingsWidget::openInputImage, this,
          &MainWindow::resizeOnInputChanged);

  m_statusBar = new QStatusBar(this);
  m_statusBar->setSizeGripEnabled(false);
  setStatusBar(m_statusBar);

  connect(m_LBGStippling, &LBGStippling::displayStatusMessage, this,
          &MainWindow::displayStatusMessage);

  resize(sizeHint());
}

MainWindow::~MainWindow() {
  delete m_stippleViewer;
  delete m_LBGStippling;
  delete m_statusBar;
}

void MainWindow::resizeOnDocking(bool value) {
  Q_UNUSED(value);
  resize(sizeHint());
}

void MainWindow::resizeOnInputChanged() {
  m_statusBar->clearMessage();
  resize(sizeHint());
}

void MainWindow::displayStatusMessage(const int iteration,
                                      const int numberPoints, const int splits,
                                      const int merges) {
  m_statusBar->showMessage(
      "Iteration: " + QString::number(iteration) + " | Number points: " +
      QString::number(numberPoints) + " | Splits: " + QString::number(splits) +
      " | Merges: " + QString::number(merges));
}
