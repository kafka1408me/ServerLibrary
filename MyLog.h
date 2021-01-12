#ifndef MYLOG_H
#define MYLOG_H

#include <QFile>
#include <QTextStream>
#include <QString>

class MyLog: public QObject
{
    Q_OBJECT
    QFile file;
    QTextStream stream;
public:
    MyLog(QObject* parent = nullptr);
    ~MyLog();

public slots:
    void Log(const QString& str);
};


#endif // MYLOG_H
