#ifndef MYSERVER_H
#define MYSERVER_H

#include "QObject"

class QWebSocketServer;
class MyLibraryServerGui;

class MyServer : public QObject
{
    Q_OBJECT
public:
    explicit MyServer(int port, QObject *parent = nullptr);
    void StartServer();
    void setMyLibServer(MyLibraryServerGui* serverGui);

public slots:
    void slot_NewConnection();
    void slot_sendConnectionStatus();

signals:
    void serverIsConnected(bool);

private:
    int port;
    QWebSocketServer *webSocketServer;
    MyLibraryServerGui* myLibServ;
};

#endif // MYSERVER_H
