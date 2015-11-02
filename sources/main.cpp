/***********************************************************************************************
 * Pudir  - A pusher of directory to Android
 *   Version: 1.0
 * Copyright 2015 eccard
 * MIT License
 ************************************************************************************************/
#include "headers/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
