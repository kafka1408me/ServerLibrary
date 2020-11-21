#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>

class QWebSocket;

class MyClient : public QObject
{
    Q_OBJECT
public:

    explicit MyClient(QWebSocket* wSocket, QObject *parent = nullptr);
    void setUserId(int userId);
    void setAdmin(bool isAdmin);
    void setLogin(QString loginStr);
    void sendNewActiveUser();

signals:
    void newActiveUser(qint64, QString);
    void loggedInUser(qint64, QString, bool);
    void unloginned(qint64);
    void disconnectUser(qint64);
    void request(QJsonObject);

public slots:
    void slot_messageReceived(const QString& message);
    void disconnected();
    void slot_getResultQuery(QJsonObject);

private:
    QWebSocket * socket;
    bool is_admin = false;
    int user_id = -1;
    QString ipStr;
    QString login;
};

#endif // MYCLIENT_H
