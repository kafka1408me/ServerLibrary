#ifndef DATABASEACCESSOR_H
#define DATABASEACCESSOR_H

#include <QObject>
#include <QtSql>

class DatabaseAccessor : public QObject
{
    Q_OBJECT

public:
    static DatabaseAccessor* getInstance();

private:
    DatabaseAccessor();

signals:
    void request(QJsonObject obj);
    void isConnectedDB(bool);

public slots:
    void slot_sendConnectionStatus();
    void slot_requestDB(QJsonObject str_in);

public:
    static QString dbHost;
    static QString dbName;
    static QString dbUser;
    static QString dbPass;

private:
    QSqlDatabase db;
};

#endif // DATABASEACCESSOR_H
