#include "workerthread.h"
#include <QDebug>

WorkerThread::WorkerThread(QString adbPath,QString sourcePath,
                           QString deviceid,QString destinationDirec,
                           bool backup,QString backupDir)
{
    this->adbPath = adbPath;
    this->sourcePath = sourcePath;
    this->deviceid = deviceid;
    this->destinationDirec = destinationDirec;
    this->backup = backup;
    this->backupDir = backupDir;
    this->mProcess = new QProcess();
    mProcess->moveToThread(this);
    mProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(mProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadOutput()));
}

void WorkerThread::run(){
    QString result="";

    if(backup){
        emit resultReady(QString("Deleting backup folder ")+QString("/sdcard/"+backupDir+"\n"));
        mProcess->start(adbPath+" -s "+deviceid +" shell rm -rv /sdcard/"+backupDir);
        mProcess->waitForFinished(-1);
        emit resultReady(QString("Moving files to backup folder ")+ QString("/sdcard/"+backupDir+"\n"));
        mProcess->start(adbPath+" -s "+deviceid +" shell mv -v /sdcard/"+destinationDirec +" /sdcard/"+backupDir);
        mProcess->waitForFinished(-1);
    }
    qDebug() << " removendo arquivos";
    emit resultReady(QString("Deleting destination folder ") + QString("/sdcard/"+destinationDirec+"\n"));
    mProcess->start(adbPath+QString(" -s ")+deviceid +QString(" shell rm -rf /sdcard/"+destinationDirec));
    mProcess->waitForFinished(-1);
    qDebug() << QString("Creating folder /sdcard/")+destinationDirec+QString("/default");
    emit resultReady(QString("Creating folder /sdcard/"+destinationDirec+"/default \n"));
    mProcess->start(adbPath+" -s "+deviceid +" shell mkdir -p /sdcard/"+destinationDirec+"/default");
    mProcess->waitForFinished(-1);
    qDebug() << "Coppy files from "+sourcePath + " to "+ "/sdcard/"+destinationDirec+"/default";
    emit resultReady(QString("Coppy files from "+ sourcePath+" to "+"/sdcard/"+destinationDirec+"/default"));
    mProcess->start(adbPath+" -s "+deviceid +" push \""+ sourcePath  +"\" /sdcard/"+destinationDirec+"/default");
    mProcess->waitForFinished(-1);
    emit resultReady("Finish !!!\n");


}
void WorkerThread::readyReadOutput(){
   emit resultReady(mProcess->readAllStandardOutput());
}
