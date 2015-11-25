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
#include <QPalette>
#include "headers/aboutdialog.h"
#include "headers/workerthread.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    settings = new QSettings("freesoft", "pudir");
    mProcess = new QProcess();
    mProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(mProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
//    connect(mProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
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
    if(settings->value("backup").toBool()){
        ui->label_4->setEnabled(true);
        ui->checkBox_backup->setChecked(true);
        ui->lineEdit_backup_dir->setEnabled(true);
        ui->lineEdit_backup_dir->setText(settings->value("backup_dir").toString());
    }else{
        ui->label_4->setEnabled(false);
        ui->lineEdit_backup_dir->setText(settings->value("backup_dir").toString());
        ui->lineEdit_backup_dir->setEnabled(false);

    }
    ui->btn_push->setEnabled(false);


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
    ui->plainTextEdit->clear();
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
    startWorkInAThread();
}

void MainWindow::on_pushButton_clicked()
{
    ui->plainTextEdit->clear();
}

void MainWindow::readyReadStandardOutput(){
    ui->plainTextEdit->appendPlainText(mProcess->readAllStandardOutput());
}

void MainWindow::readyReadStandardError(){
    ui->plainTextEdit->appendPlainText(mProcess->readAllStandardError());
}



void MainWindow::on_checkBox_backup_clicked()
{
    bool checked = ui->checkBox_backup->isChecked();
    settings->setValue("backup",checked);
    if (checked){
        ui->label_4->setEnabled(true);
        ui->lineEdit_backup_dir->setEnabled(true);
        ui->lineEdit_backup_dir->setText(settings->value("backup_dir").toString());
    }else{
        ui->label_4->setEnabled(false);
        ui->lineEdit_backup_dir->setEnabled(false);
    }
}

/*
class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QString &adbPath,QString &sourcePath,
                 QString &deviceid,QString &destinationDirec,
                 bool backup,QString &backupDir,QObject *parent):QThread(parent){
        this->adbPath = adbPath;
        this->sourcePath = sourcePath;
        this->deviceid = deviceid;
        this->destinationDirec = destinationDirec;
        this->backup = backup;
        this->backupDir = backupDir;
    }
private:
    QString adbPath;
    QString sourcePath;
    QString deviceid;
    QString destinationDirec;

    bool backup;
    QString backupDir;

    void run() {

        QString result="";
//         expensive or blocking operation

        emit resultReady(result);
    }
signals:
    void resultReady(const QString &ss);
};
*/
void MainWindow::startWorkInAThread()
{
    QString destinationDirec = ui->lineEdit_dest_dir->text();
    QString deviceid = ui->comboBox->currentText();
    QString backupDir;
    settings->setValue("destDir",destinationDirec);

    QString sourcePath = settings->value("sourcePath").toString();
    ui->plainTextEdit->clear();

    bool backup = ui->checkBox_backup->isChecked();
    if(backup){
        if(!ui->lineEdit_backup_dir->text().isEmpty())
            backupDir = ui->lineEdit_backup_dir->text();
        else{
            backupDir = destinationDirec+".backup";
            ui->lineEdit_backup_dir->setText(backupDir);
        }
        settings->setValue("backup_dir",backupDir);
    }

    WorkerThread *workerThread = new WorkerThread(adbPath,sourcePath,
                                                  deviceid,destinationDirec,
                                                  backup,backupDir);
    connect(workerThread, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
}
void MainWindow::handleResults(QString result){
    ui->plainTextEdit->appendPlainText(result);

}
