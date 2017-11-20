#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

class StippleViewer;
class LBGStippling;

class MainWindow : public QMainWindow {

  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

private slots:
  void resizeOnDocking(bool value);
  void resizeOnInputChanged();
  void displayStatusMessage(const int iteration, const int numberPoints,
                            const int splits, const int merges);

private:
  StippleViewer *m_stippleViewer;
  LBGStippling *m_LBGStippling;

  QStatusBar *m_statusBar;
};

#endif // MAINWINDOW_H
