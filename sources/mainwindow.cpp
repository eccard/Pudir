/***********************************************************************************************
 * Pudir  - A pusher of directory to Android
 *   Version: 1.0
 * Copyright 2015 eccard
 * MIT License
 ************************************************************************************************/
#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QStringListModel>
#include <QProcess>
#include <QPalette>
#include "headers/aboutdialog.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    settings = new QSettings("freesoft", "pudir");
    loadComp();


}
void MainWindow::loadComp(){
    QPalette p = ui->plainTextEdit->palette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::black);
    p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    ui->plainTextEdit->setPalette(p);

    ui->lineEdit_src_dir->setText(settings->value("sourcePath").toString());
    ui->lineEdit_dest_dir->setText(settings->value("destDir").toString());
    adbPath = settings->value("adbPath").toString();
    if(adbPath.isEmpty()){
        ui->btn_refresh->setEnabled(false);
        ui->btn_setsrcdir->setEnabled(false);
        ui->btn_push->setEnabled(false);
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Android SDK location not set");
        msgBox.setInformativeText("Please go to Option -> Set SDK");
        msgBox.exec();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    setSdkAct = new QAction(tr("&Set path Android SDK "),this);
    connect(setSdkAct, SIGNAL(triggered()), this, SLOT(setSdkLocation()));

    aboutAct = new QAction(tr("&About"),this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    exitAct = new QAction(tr("&Exit"),this);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(exit()));
}
void MainWindow::createMenus()
{
    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(setSdkAct);
    optionsMenu->addAction(aboutAct);
    optionsMenu->addAction(exitAct);
}

void MainWindow::setSdkLocation(){
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    qDebug()<< fileNames;

#ifdef __linux
    adbPath = fileNames.at(0)+"/platform-tools/adb";
#elif _WIN32
    adbPath = fileNames.at(0)+"/platform-tools/adb.exe";
#elif __APPLE__

#endif
    QMessageBox msgBox;

    QFile *qfile = new QFile(adbPath);
    if(qfile->exists()){
        adbPath = qfile->fileName();
        settings->setValue("adbPath",adbPath);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Ok");
        msgBox.exec();
        ui->btn_refresh->setEnabled(true);
        ui->btn_push->setEnabled(true);
        ui->btn_setsrcdir->setEnabled(true);
    }
    else{
        adbPath = "";
        settings->setValue("adbPath",adbPath);
        msgBox.setText("Wrong path");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        setSdkLocation();
    }
}

void MainWindow::about(){
    AboutDialog aboutDial;
    aboutDial.exec();
}

void MainWindow::exit(){
    close();
}



void MainWindow::on_btn_refresh_clicked()
{
    QStringList result;
    QProcess process;
    process.start(adbPath+" devices");
    process.waitForFinished(-1);
    QString sstdout = process.readAllStandardOutput();
    QString sstderr = process.readAllStandardError();
    ui->plainTextEdit->appendPlainText(sstdout);
    ui->plainTextEdit->appendPlainText(sstderr);
    qDebug() << "."+adbPath+" devices";
    qDebug() << sstdout;
    QString aux;
    if(!sstdout.isEmpty()){

        QStringList aux2 = sstdout.mid(sstdout.indexOf("\n")).split("\n");
        foreach (const QString &str, aux2) {
            if (str.contains("device")){
                int j = str.indexOf("\t");
                result += str.left(j);
            }
        }
        if(result.isEmpty())
            ui->btn_push->setEnabled(false);
        else
            ui->btn_push->setEnabled(true);

    }

    qDebug()<<"result"<<result;
     QStringListModel *typeModel = new QStringListModel(result, this);

    ui->comboBox->setModel(typeModel);
}

void MainWindow::on_btn_setsrcdir_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    if (!ui->lineEdit_src_dir->text().isEmpty())
        dialog.setDirectory(ui->lineEdit_src_dir->text());
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    if (!fileNames.isEmpty()){
        qDebug()<< fileNames;
        settings->setValue("sourcePath",fileNames.at(0));
        ui->lineEdit_src_dir->setText(fileNames.at(0));
    }


}

void MainWindow::on_btn_push_clicked()
{
    QString destinationDirec = ui->lineEdit_dest_dir->text();
    QString deviceid = ui->comboBox->currentText();
    settings->setValue("destDir",destinationDirec);


    QString sstdout,sstderr;
    QString sourcePath = settings->value("sourcePath").toString();

    QProcess process;
    process.start(adbPath+" -s "+deviceid +" shell rm -rf /sdcard/"+destinationDirec);
    process.waitForFinished(-1);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(process.readAllStandardOutput());
    ui->plainTextEdit->appendPlainText(process.readAllStandardError());

    process.start(adbPath+" -s "+deviceid +" shell mkdir -p /sdcard/"+destinationDirec+"/default");
    process.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(process.readAllStandardOutput());
    ui->plainTextEdit->appendPlainText(process.readAllStandardError());

    process.start(adbPath+" -s "+deviceid +" push \""+ sourcePath  +"\" /sdcard/"+destinationDirec+"/default");
    process.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(process.readAllStandardOutput());
    ui->plainTextEdit->appendPlainText(process.readAllStandardError());

    qDebug() << adbPath+" shell rm -r /sdcard/"+destinationDirec;
    qDebug() << adbPath+" shell shell mkdir -p /sdcard/"+destinationDirec+"/default";
    qDebug() << adbPath+" push \""+ sourcePath  +"\" /sdcard/"+destinationDirec+"/default";
    qDebug() << sstdout;
    qDebug() << sstderr;
}

void MainWindow::on_pushButton_clicked()
{
    ui->plainTextEdit->clear();
}
