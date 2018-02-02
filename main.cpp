/*
 *      This is an interactive demo application for the algorithm proposed in:
 *
 *      Weighted Linde-Buzo Gray Stippling
 *      Oliver Deussen, Marc Spicker, Qian Zheng
 *
 *      In: ACM Transactions on Graphics (Proceedings of SIGGRAPH Asia 2017)
 *      https://doi.org/10.1145/3130800.3130819
 *
 *     Copyright 2017 Marc Spicker (marc.spicker@googlemail.com)
 */

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Weighted Linde-Buzo-Gray Stippling");

    MainWindow window;
    window.show();

    return app.exec();
}
