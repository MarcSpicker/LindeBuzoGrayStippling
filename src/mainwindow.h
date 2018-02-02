#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

class StippleViewer;

class MainWindow : public QMainWindow {

  public:
    MainWindow();

  private:
    StippleViewer* m_stippleViewer;
    QStatusBar* m_statusBar;
};

#endif // MAINWINDOW_H
