#include "myserver.h"
#include <QWebSocketServer>
#include "myclient.h"
#include "mylibraryservergui.h"
#include <QThread>
#include <QTimer>

MyServer::MyServer(int port, QObject *parent) :
    QObject(parent),
    port(port),
    webSocketServer(nullptr)
{
}

void MyServer::StartServer()
{
    qDebug() << "MyServer::StartServer() = THREAD" << QThread::currentThreadId();
    webSocketServer = new QWebSocketServer("MyLibraryServer",
                                           QWebSocketServer::SslMode::NonSecureMode,
                                           this);

    if (!webSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "MyServer: is not started: " << webSocketServer->errorString();
    }
    else
    {
        qDebug() << "MyServer: successfully start; and listen port = " << port;
        connect(webSocketServer, &QWebSocketServer::newConnection, this, &MyServer::slot_NewConnection);
    }

//    QTimer* timer = new QTimer;
//    timer->setInterval(10*1000);      // Каждые 10 секунд посылаем сигнал
//    connect(timer, &QTimer::timeout, this, &MyServer::slot_sendConnectionStatus);
//    timer->start();
}

void MyServer::setMyLibServer(MyLibraryServerGui *serverGui)
{
    myLibServ = serverGui;
}

void MyServer::slot_NewConnection()
{
    qDebug() << "MyServer::slot_NewConnection() THREAD= " << QThread::currentThreadId();
    qDebug() << "MyServer: slot_NewConnection";
    QWebSocket* pSocket = webSocketServer->nextPendingConnection();
    MyClient* client = new MyClient(pSocket, this);

    connect(client, &MyClient::newActiveUser, myLibServ, &MyLibraryServerGui::slot_newActiveUser);
    connect(client, &MyClient::loggedInUser, myLibServ, &MyLibraryServerGui::slot_loggedInUser);
    connect(client, &MyClient::disconnectUser, myLibServ, &MyLibraryServerGui::slot_disconnectUser);

    client->sendNewActiveUser();
}

void MyServer::slot_sendConnectionStatus()
{
    emit serverIsConnected(webSocketServer->isListening());
}
