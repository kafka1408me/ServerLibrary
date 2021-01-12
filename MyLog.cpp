#include "MyLog.h"
#include <QDir>
#include <QDateTime>
#include <QApplication>

QString createLogFileName()
{
    QString dirPath = QApplication::applicationDirPath() + "/LogFiles";
    if(!QDir(dirPath).exists())
    {
        QDir().mkpath(dirPath);
    }

    auto date = QDateTime::currentDateTime();

    return dirPath  + "/" + QString("logFile-%1-_%2.txt").arg(date.toString("dd-MM-yyyy-_hh-mm-ss")).arg(QString::number(date.toMSecsSinceEpoch()));
}

// MyLog CLASS

MyLog::MyLog(QObject *parent):
    QObject(parent)
{
    QString fileName = createLogFileName();
    file.setFileName(fileName);
    //        file.resize(0);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    stream.setDevice(&file);
}

MyLog::~MyLog()
{
    file.close();
}

void MyLog::Log(const QString& str)
{
    stream << str << '\n';
    stream.flush();
    file.flush();
}
