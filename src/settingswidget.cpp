#include "settingswidget.h"
#include "stippleviewer.h"

SettingsWidget::SettingsWidget(StippleViewer* stippleViewer, QWidget* parent)
    : QWidget(parent), m_params(), m_stippleViewer(stippleViewer) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // point related stuff
    QGroupBox* pointGroup = new QGroupBox("Point Settings:", this);

    QLabel* initialPointLabel = new QLabel("Initial points:", this);
    QSpinBox* spinInitialPoints = new QSpinBox(this);
    spinInitialPoints->setRange(1, 100000);
    spinInitialPoints->setValue(m_params.initialPoints);
    spinInitialPoints->setToolTip(
        "The number of points the algorithm starts with, must be at least one.");
    connect(spinInitialPoints, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) { m_params.initialPoints = value; });

    QLabel* initialPointSizeLabel = new QLabel("Point Size:", this);
    QDoubleSpinBox* spinInitialPointSize = new QDoubleSpinBox(this);
    spinInitialPointSize->setRange(0.1f, 50.0f);
    spinInitialPointSize->setValue(m_params.initialPointSize);
    spinInitialPointSize->setSingleStep(0.1f);
    spinInitialPointSize->setToolTip("The point size used by the algorithm. This "
                                     "will only be used when adaptive point size "
                                     "is off.");
    connect(spinInitialPointSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { m_params.initialPointSize = value; });

    QCheckBox* adaptivePointSize = new QCheckBox("Use adaptive point size.", this);
    adaptivePointSize->setChecked(m_params.adaptivePointSize);
    adaptivePointSize->setToolTip("If enabled the algorithm will automatically "
                                  "determine the point size in the given minimum "
                                  "and maximum range.");
    connect(adaptivePointSize, &QCheckBox::clicked,
            [this](bool value) { m_params.adaptivePointSize = value; });

    QLabel* minPointSize = new QLabel("Minimal Point Size:", this);
    QDoubleSpinBox* spinMinPointSize = new QDoubleSpinBox(this);
    spinMinPointSize->setRange(0.1f, 50.0f);
    spinMinPointSize->setValue(m_params.pointSizeMin);
    spinMinPointSize->setSingleStep(0.1f);
    spinMinPointSize->setToolTip(
        "The minimum point size used when adaptive point size is enabled.");
    connect(spinMinPointSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { m_params.pointSizeMin = value; });

    QLabel* maxPointSize = new QLabel("Maximal Point Size:", this);
    QDoubleSpinBox* spinMaxPointSize = new QDoubleSpinBox(this);
    spinMaxPointSize->setRange(0.1f, 50.0f);
    spinMaxPointSize->setValue(m_params.pointSizeMax);
    spinMaxPointSize->setSingleStep(0.1f);
    spinMaxPointSize->setToolTip(
        "The maximum point size used when adaptive point size is enabled.");
    connect(spinMaxPointSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { m_params.pointSizeMax = value; });

    QGridLayout* pointGroupLayout = new QGridLayout(pointGroup);
    pointGroup->setLayout(pointGroupLayout);
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
    QGroupBox* algoGroup = new QGroupBox("Algorithm Settings:", this);

    QLabel* hysteresisLabel = new QLabel("Hysteresis:", this);
    QDoubleSpinBox* spinHysterresis = new QDoubleSpinBox(this);
    spinHysterresis->setRange(0.1f, 3.0f);
    spinHysterresis->setValue(m_params.hysteresis);
    spinHysterresis->setSingleStep(0.1f);
    spinHysterresis->setToolTip("How close to 'perfect' the cell size has to be "
                                "in order not to be split:\n Lower values mean "
                                "slower convergence but higher quality results "
                                "and vice versa.");
    connect(spinHysterresis, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) { m_params.hysteresis = value; });

    QCheckBox* adaptiveHysteresis = new QCheckBox("Use adaptive hysteresis.", this);
    adaptiveHysteresis->setChecked(m_params.adaptiveHysteresis);
    adaptiveHysteresis->setToolTip(
        "If enabled, the hysteresis will increase over time (linearly up to "
        "twice the hysteresis over the maximum number of iterations).");
    connect(adaptiveHysteresis, &QCheckBox::clicked,
            [this](bool value) { m_params.adaptiveHysteresis = value; });

    QLabel* maxIterLabel = new QLabel("Maximum Iterations:", this);
    QSpinBox* spinMaxIter = new QSpinBox(this);
    spinMaxIter->setRange(1, 1000);
    spinMaxIter->setValue(m_params.maxIterations);
    spinMaxIter->setToolTip("The maximum number of iterations until the "
                            "algorithm stops (may stop before if no further "
                            "changes).");
    connect(spinMaxIter, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) { m_params.maxIterations = value; });

    QLabel* superSampleLabel = new QLabel("Super-Sampling Factor:", this);
    QSpinBox* spinSuperSample = new QSpinBox(this);
    spinSuperSample->setRange(1, 3);
    spinSuperSample->setValue(m_params.superSamplingFactor);
    spinSuperSample->setToolTip("Increases the size and percision of the Voronoi "
                                "diagram, but makes the calculation slower.");
    connect(spinSuperSample, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) { m_params.superSamplingFactor = value; });

    QGridLayout* algoGroupLayout = new QGridLayout(algoGroup);
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
    QGroupBox* openBox = new QGroupBox("Open file as input:", this);
    QVBoxLayout* openBoxLayout = new QVBoxLayout(openBox);
    openBox->setLayout(openBoxLayout);
    QPushButton* fileButton = new QPushButton("Select", this);
    openBoxLayout->addWidget(fileButton);
    connect(fileButton, &QPushButton::pressed, [this]() {
        QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(),
                                                    tr("Image Files (*.png *.jpg *.bmp *.jpeg)"));

        if (path.isEmpty())
            return;

        disableSaveButtons();
        QImage img(path);
        m_stippleViewer->setInputImage(img);
    });

    layout->addWidget(openBox);

    // save buttons
    QGroupBox* saveGroup = new QGroupBox("Save as:", this);
    QHBoxLayout* saveLayout = new QHBoxLayout(saveGroup);
    m_savePNG = new QPushButton("PNG", this);
    m_savePNG->setEnabled(false);
    m_saveSVG = new QPushButton("SVG", this);
    m_saveSVG->setEnabled(false);
    saveLayout->addWidget(m_savePNG);
    saveLayout->addWidget(m_saveSVG);
    saveGroup->setLayout(saveLayout);

    connect(m_saveSVG, &QPushButton::pressed, [this]() {
        QFileDialog dialog(this, tr("Save Image as SVG"), QString(), tr("SVG Image (*.svg)"));
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("svg");
        if (dialog.exec() == 0)
            return;

        QString path = dialog.selectedFiles().first();

        if (path.isEmpty())
            return;

        m_stippleViewer->saveImageSVG(path);
    });
    connect(m_stippleViewer, &StippleViewer::finished, this, &SettingsWidget::enableSaveButtons);

    connect(m_savePNG, &QPushButton::pressed, [this]() {
        QFileDialog dialog(this, tr("Save Image as PNG"), QString(), tr("PNG Image (*.png)"));
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("png");

        if (dialog.exec() == 0)
            return;

        QString path = dialog.selectedFiles().first();

        if (path.isEmpty())
            return;

        QPixmap map = m_stippleViewer->getImage();
        map.save(path);
    });

    layout->addWidget(saveGroup);

    // start button
    QGroupBox* startGroup = new QGroupBox("Run Algorithm:", this);
    QVBoxLayout* startLayout = new QVBoxLayout(startGroup);

    QPushButton* startButton = new QPushButton("Start", this);
    startLayout->addWidget(startButton);

    connect(startButton, &QPushButton::released,
            [startButton]() { startButton->setEnabled(false); });
    connect(startButton, &QPushButton::released, [this]() { disableSaveButtons(); });
    connect(m_stippleViewer, &StippleViewer::finished,
            [startButton]() { startButton->setEnabled(true); });
    connect(startButton, &QPushButton::released, [fileButton]() { fileButton->setEnabled(false); });

    QProgressBar* progressBar = new QProgressBar(this);
    progressBar->setRange(0, 1);
    progressBar->setAlignment(Qt::AlignCenter);
    startLayout->addWidget(progressBar);
    startGroup->setLayout(startLayout);
    layout->addWidget(startGroup);

    connect(m_stippleViewer, &StippleViewer::finished, [fileButton, progressBar]() {
        fileButton->setEnabled(true);
        progressBar->setRange(0, 1);
    });
    connect(startButton, &QPushButton::released, [this, progressBar]() {
        progressBar->setRange(0, 0);
        m_stippleViewer->stipple(m_params);
    });

    layout->addStretch(1);
}

void SettingsWidget::enableSaveButtons() {
    m_savePNG->setEnabled(true);
    m_saveSVG->setEnabled(true);
}

void SettingsWidget::disableSaveButtons() {
    m_savePNG->setEnabled(false);
    m_saveSVG->setEnabled(false);
}
