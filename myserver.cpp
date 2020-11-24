#include "myserver.h"
#include <QWebSocketServer>
#include "myclient.h"
#include "mylibraryservergui.h"
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QSslKey>

MyServer::MyServer(int port, QObject *parent) :
    QObject(parent),
    port(port),
    webSocketServer(nullptr)
{
}

void MyServer::StartServer()
{
    qDebug() << "version build = " << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << QSslSocket::supportsSsl();

    QFile certFile (":/keys/mylibraryserver.crt");
    if (!certFile.exists())
    {
        qDebug() << "SmacADService: CERTFILE NOT EXIST!!!!!!!!!!!";
        exit(-3);
    }


    QFile keyFile (":/keys/mylibraryserver-privateKey.key");
    if (!keyFile.exists())
    {
        qDebug() << "SmacADService: KEYFILE NOT EXIST!!!!!!!!!!!";
        exit(-4);
    }

    if (!certFile.open (QIODevice::ReadOnly))
    {
        qDebug() << "SmacADService: CERTFILE can 't be opened";
        exit(-5);
    }
    if (!keyFile.open (QIODevice::ReadOnly))
    {
        qDebug() << "SmacADService: KEYFILE can 't be opened";
        exit(-6);
    }

    QSslCertificate certificate (&certFile, QSsl::Pem);
    QSslKey sslKey (&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();

    //ssl конфиг для сервера
    QSslConfiguration sslConfigServer;

    sslConfigServer.setPeerVerifyMode (QSslSocket::VerifyNone);
    sslConfigServer.setLocalCertificate (certificate);
    sslConfigServer.setPrivateKey (sslKey);
    sslConfigServer.setProtocol (QSsl::TlsV1SslV3);



    qDebug() << "MyServer::StartServer() = THREAD" << QThread::currentThreadId();
    webSocketServer = new QWebSocketServer("MyLibraryServer",
                                           QWebSocketServer::SslMode::SecureMode,
                                           this);

    webSocketServer->setSslConfiguration(sslConfigServer);

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
