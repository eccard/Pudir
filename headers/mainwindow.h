/***********************************************************************************************
 * Pudir  - A pusher of directory to Android
 *   Version: 1.0
 * Copyright 2015 eccard
 * MIT License
 ************************************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    QString adbPath;

private slots:
    void setSdkLocation();
    void about();
    void exit();

    void on_btn_refresh_clicked();

    void on_btn_setsrcdir_clicked();

    void on_btn_push_clicked();

private:
    void loadComp();
    void createActions();
    void createMenus();

    QMenu *optionsMenu;
    QActionGroup *alignmentGroup;
    QAction  *setSdkAct;
    QAction  *aboutAct;
    QAction  *exitAct;

};

#endif // MAINWINDOW_H
