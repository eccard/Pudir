#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QProcess>
class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QString adbPath= QString(""),QString sourcePath=QString(""),
                          QString deviceid=QString(""),QString destinationDirec=QString(""),
                          bool backup=false,QString backupDir=QString(""));

private:
    QString adbPath;
    QString sourcePath;
    QString deviceid;
    QString destinationDirec;
    bool backup;
    QString backupDir;

    QProcess *mProcess;
    void run();
signals:
    void resultReady(const QString &ss);
signals:


public slots:
private slots:
    void readyReadOutput();
};

#endif // WORKERTHREAD_H
