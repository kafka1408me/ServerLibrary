#include "myclient.h"
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include "databaseaccessor.h"
#include "codes.h"

MyClient::MyClient(QWebSocket *wSocket, QObject *parent) :
    QObject(parent),
    socket(wSocket)
{
    ipStr = socket->peerAddress().toString();

    qDebug() << "MyClient: new client = " << ipStr;

    connect(socket, &QWebSocket::textMessageReceived, this, &MyClient::slot_messageReceived);
    connect(this, &MyClient::request, DatabaseAccessor::getInstance(), &DatabaseAccessor::slot_requestDB);
    connect(socket, &QWebSocket::disconnected, this, &MyClient::disconnected);
}

void MyClient::setUserId(int userId)
{
    user_id = userId;
}

void MyClient::setAdmin(bool isAdmin)
{
    is_admin = isAdmin;
}

void MyClient::setLogin(QString loginStr)
{
    login = std::move(loginStr);
    emit loggedInUser(qint64(socket), login, is_admin);
}

void MyClient::sendNewActiveUser()
{
    emit newActiveUser(qint64(socket), ipStr);
}

void MyClient::slot_messageReceived(const QString &message)
{
    qDebug() << "MyClient::slot_messageReceived TREAD = " << QThread::currentThreadId();
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject obj = doc.object();

//    const int type = obj.value("type").toInt();

    emit request(obj);
}

void MyClient::disconnected()
{
    emit disconnectUser(qint64(socket));
    qDebug() << "MyClient: " << login << " " << ipStr << "disconnected";
    socket->close();
//    deleteLater();
}

void MyClient::slot_getResultQuery(QJsonObject obj)
{
//    qDebug()<<"result= " << obj;
    QJsonDocument doc(obj);
    socket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
}

