#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QtWidgets>

class StippleViewer;
class LBGStippling;

class SettingsWidget : public QWidget {

  Q_OBJECT

public:
  explicit SettingsWidget(const LBGStippling *lbgStippling,
                          const StippleViewer *stippleViewer,
                          QWidget *parent = nullptr);

signals:
  void saveImagePNG(const QString &path);
  void saveImageSVG(const QString &path);
  void openInputImage(const QString &path);

public slots:
  void enableStartButton();
  void disableStartButton();

  void enableSaveButtons();
  void disableSaveButtons();

  void enableSelectButton();
  void disableSelectButton();

private slots:
  void saveCallPNG();
  void saveCallSVG();
  void openInputFileCall();

private:
  const LBGStippling *m_LBGStippling;
  const StippleViewer *m_stippleViewer;

  QPushButton *m_savePNG;
  QPushButton *m_saveSVG;

  QPushButton *m_fileButton;

  QPushButton *m_startButton;
  QProgressBar *m_progressBar;
};

#endif // SETTINGSWIDGET_H
