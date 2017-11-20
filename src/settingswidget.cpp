#include "settingswidget.h"
#include "lbgstippling.h"
#include "stippleviewer.h"

SettingsWidget::SettingsWidget(const LBGStippling *lbgStippling,
                               const StippleViewer *stippleViewer,
                               QWidget *parent)
    : QWidget(parent), m_LBGStippling(lbgStippling),
      m_stippleViewer(stippleViewer) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  setLayout(layout);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  // point related stuff
  QGroupBox *pointGroup = new QGroupBox("Point Settings:", this);

  QLabel *initialPointLabel = new QLabel("Initial points:", this);
  QSpinBox *spinInitialPoints = new QSpinBox(this);
  spinInitialPoints->setValue(1);
  spinInitialPoints->setRange(1, 10000);
  spinInitialPoints->setToolTip(
      "The number of points the algorithm starts with, must be at least one.");
  connect(spinInitialPoints, SIGNAL(valueChanged(int)), m_LBGStippling,
          SLOT(setInitalPoints(int)));

  QLabel *initialPointSizeLabel = new QLabel("Point Size:", this);
  QDoubleSpinBox *spinInitialPointSize = new QDoubleSpinBox(this);
  spinInitialPointSize->setValue(4.0f);
  spinInitialPointSize->setRange(1.0f, 20.0f);
  spinInitialPointSize->setSingleStep(0.1f);
  spinInitialPointSize->setToolTip("The point size used by the algorithm. This "
                                   "will only be used when adaptive point size "
                                   "is off.");
  connect(spinInitialPointSize, SIGNAL(valueChanged(double)), m_LBGStippling,
          SLOT(setInitialPointSize(double)));

  QCheckBox *adaptivePointSize =
      new QCheckBox("Use adaptive point size.", this);
  adaptivePointSize->setChecked(true);
  adaptivePointSize->setToolTip("If enabled the algorithm will automatically "
                                "determine the point size in the given minimum "
                                "and maximum range.");
  connect(adaptivePointSize, SIGNAL(clicked(bool)), m_LBGStippling,
          SLOT(setAdaptivePointSize(bool)));

  QLabel *minPointSize = new QLabel("Minimal Point Size:", this);
  QDoubleSpinBox *spinMinPointSize = new QDoubleSpinBox(this);
  spinMinPointSize->setValue(2.0f);
  spinMinPointSize->setRange(1.0f, 10.0f);
  spinMinPointSize->setSingleStep(0.1f);
  spinMinPointSize->setToolTip(
      "The minimum point size used when adaptive point size is enabled.");
  connect(spinMinPointSize, SIGNAL(valueChanged(double)), m_LBGStippling,
          SLOT(setMinimalPointSize(double)));

  QLabel *maxPointSize = new QLabel("Maximal Point Size:", this);
  QDoubleSpinBox *spinMaxPointSize = new QDoubleSpinBox(this);
  spinMaxPointSize->setValue(4.0f);
  spinMaxPointSize->setRange(1.0f, 10.0f);
  spinMaxPointSize->setSingleStep(0.1f);
  spinMaxPointSize->setToolTip(
      "The maximum point size used when adaptive point size is enabled.");
  connect(spinMaxPointSize, SIGNAL(valueChanged(double)), m_LBGStippling,
          SLOT(setMaximalPointSize(double)));

  QGridLayout *pointGroupLayout = new QGridLayout(pointGroup);
  pointGroup->setLayout(pointGroupLayout);
  pointGroupLayout->addWidget(initialPointLabel);
  pointGroupLayout->addWidget(spinInitialPoints);
  pointGroupLayout->addWidget(initialPointLabel, 0, 0);
  pointGroupLayout->addWidget(spinInitialPoints, 0, 1);
  pointGroupLayout->addWidget(initialPointSizeLabel, 1, 0);
  pointGroupLayout->addWidget(spinInitialPointSize, 1, 1);
  pointGroupLayout->addWidget(adaptivePointSize, 2, 0);
  pointGroupLayout->addWidget(minPointSize, 3, 0);
  pointGroupLayout->addWidget(spinMinPointSize, 3, 1);
  pointGroupLayout->addWidget(maxPointSize, 4, 0);
  pointGroupLayout->addWidget(spinMaxPointSize, 4, 1);

  layout->addWidget(pointGroup);

  // algo related stuff
  QGroupBox *algoGroup = new QGroupBox("Algorithm Settings:", this);

  QLabel *hysteresisLabel = new QLabel("Hysteresis:", this);
  QDoubleSpinBox *spinHysterresis = new QDoubleSpinBox(this);
  spinHysterresis->setValue(0.6f);
  spinHysterresis->setRange(0.1f, 3.0f);
  spinHysterresis->setSingleStep(0.1f);
  spinHysterresis->setToolTip("How close to 'perfect' the cell size has to be "
                              "in order not to be split:\n Lower values mean "
                              "slower convergence but higher quality results "
                              "and vice versa.");
  connect(spinHysterresis, SIGNAL(valueChanged(double)), m_LBGStippling,
          SLOT(setHysteresis(double)));

  QCheckBox *adaptiveHysteresis =
      new QCheckBox("Use adaptive hysteresis.", this);
  adaptiveHysteresis->setChecked(true);
  adaptiveHysteresis->setToolTip(
      "If enabled, the hysteresis will increase over time (linearly up to "
      "twice the hysteresis over the maximum number of iterations.");
  connect(adaptiveHysteresis, SIGNAL(clicked(bool)), m_LBGStippling,
          SLOT(setAdaptiveHysteresis(bool)));

  QLabel *maxIterLabel = new QLabel("Maximum Iterations:", this);
  QSpinBox *spinMaxIter = new QSpinBox(this);
  spinMaxIter->setValue(50);
  spinMaxIter->setRange(1, 1000);
  spinMaxIter->setToolTip("The maximum number of iterations until the "
                          "algorithm stops (may stop before if no further "
                          "changes).");
  connect(spinMaxIter, SIGNAL(valueChanged(int)), m_LBGStippling,
          SLOT(setMaxIterations(int)));

  QLabel *superSampleLabel = new QLabel("Super-Sampling Factor:", this);
  QSpinBox *spinSuperSample = new QSpinBox(this);
  spinSuperSample->setValue(1);
  spinSuperSample->setRange(1, 2);
  spinSuperSample->setToolTip("Increases the size and percision of the Voronoi "
                              "diagram, but makes the calculation slower.");
  connect(spinSuperSample, SIGNAL(valueChanged(int)), m_LBGStippling,
          SLOT(setSuperSampling(int)));

  QGridLayout *algoGroupLayout = new QGridLayout(algoGroup);
  algoGroup->setLayout(algoGroupLayout);
  algoGroupLayout->addWidget(hysteresisLabel, 0, 0);
  algoGroupLayout->addWidget(spinHysterresis, 0, 1);
  algoGroupLayout->addWidget(adaptiveHysteresis, 1, 0);
  algoGroupLayout->addWidget(maxIterLabel, 2, 0);
  algoGroupLayout->addWidget(spinMaxIter, 2, 1);
  algoGroupLayout->addWidget(superSampleLabel, 3, 0);
  algoGroupLayout->addWidget(spinSuperSample, 3, 1);

  layout->addWidget(algoGroup);

  // open button
  QGroupBox *openBox = new QGroupBox("Open file as input:", this);
  QVBoxLayout *openBoxLayout = new QVBoxLayout(openBox);
  openBox->setLayout(openBoxLayout);
  m_fileButton = new QPushButton("Select", this);
  openBoxLayout->addWidget(m_fileButton);
  connect(m_fileButton, SIGNAL(pressed()), this, SLOT(openInputFileCall()));

  layout->addWidget(openBox);

  // save buttons
  QGroupBox *saveGroup = new QGroupBox("Save as:", this);
  QHBoxLayout *saveLayout = new QHBoxLayout(saveGroup);
  m_savePNG = new QPushButton("PNG", this);
  m_savePNG->setEnabled(false);
  m_saveSVG = new QPushButton("SVG", this);
  m_saveSVG->setEnabled(false);
  saveLayout->addWidget(m_savePNG);
  saveLayout->addWidget(m_saveSVG);
  saveGroup->setLayout(saveLayout);

  connect(m_savePNG, SIGNAL(pressed()), this, SLOT(saveCallPNG()));
  connect(m_saveSVG, SIGNAL(pressed()), this, SLOT(saveCallSVG()));
  connect(m_LBGStippling, SIGNAL(finished()), this, SLOT(enableSaveButtons()));
  connect(this, &SettingsWidget::saveImagePNG, m_stippleViewer,
          &StippleViewer::saveImagePNG);
  connect(this, &SettingsWidget::saveImageSVG, m_stippleViewer,
          &StippleViewer::saveImageSVG);

  layout->addWidget(saveGroup);

  // start button
  QGroupBox *startGroup = new QGroupBox("Run Algorithm:", this);
  QVBoxLayout *startLayout = new QVBoxLayout(startGroup);

  m_startButton = new QPushButton("Start", this);
  connect(m_startButton, SIGNAL(released()), m_LBGStippling, SLOT(start()));
  connect(m_startButton, SIGNAL(released()), this, SLOT(disableStartButton()));
  connect(m_startButton, SIGNAL(released()), this, SLOT(disableSaveButtons()));
  connect(m_LBGStippling, SIGNAL(finished()), this, SLOT(enableStartButton()));

  connect(m_startButton, SIGNAL(pressed()), this, SLOT(disableSelectButton()));
  connect(m_LBGStippling, SIGNAL(finished()), this, SLOT(enableSelectButton()));

  startLayout->addWidget(m_startButton);

  m_progressBar = new QProgressBar(this);
  m_progressBar->setRange(0, 1);
  m_progressBar->setAlignment(Qt::AlignCenter);
  startLayout->addWidget(m_progressBar);
  startGroup->setLayout(startLayout);
  layout->addWidget(startGroup);

  layout->addStretch(1);
}

void SettingsWidget::enableStartButton() {
  m_startButton->setEnabled(true);
  m_progressBar->setRange(0, 1);
}

void SettingsWidget::disableStartButton() {
  m_startButton->setEnabled(false);
  m_progressBar->setRange(0, 0);
}

void SettingsWidget::enableSaveButtons() {
  m_savePNG->setEnabled(true);
  m_saveSVG->setEnabled(true);
}

void SettingsWidget::disableSaveButtons() {
  m_savePNG->setEnabled(false);
  m_saveSVG->setEnabled(false);
}

void SettingsWidget::saveCallPNG() {
  QFileDialog dialog(this, tr("Save Image as PNG"), QString(),
                     tr("PNG Image (*.png)"));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("png");

  if (dialog.exec() == 0)
    return;

  QString path = dialog.selectedFiles().first();

  if (path.isEmpty())
    return;

  emit saveImagePNG(path);
}

void SettingsWidget::saveCallSVG() {
  QFileDialog dialog(this, tr("Save Image as SVG"), QString(),
                     tr("SVG Image (*.svg)"));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("svg");
  if (dialog.exec() == 0)
    return;

  QString path = dialog.selectedFiles().first();

  if (path.isEmpty())
    return;

  emit saveImageSVG(path);
}

void SettingsWidget::openInputFileCall() {
  QString path = QFileDialog::getOpenFileName(
      this, tr("Open Image"), QString(),
      tr("Image Files (*.png *.jpg *.bmp *.jpeg)"));

  if (path.isEmpty())
    return;

  disableSaveButtons();

  emit openInputImage(path);
}

void SettingsWidget::enableSelectButton() { m_fileButton->setEnabled(true); }

void SettingsWidget::disableSelectButton() { m_fileButton->setEnabled(false); }
