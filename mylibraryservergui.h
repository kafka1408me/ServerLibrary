#ifndef MYLIBRARYSERVERGUI_H
#define MYLIBRARYSERVERGUI_H

#include <QWidget>

namespace Ui {
class mylibraryservergui;
}

class QThread;
class MyServer;
class QTimer;

enum ConnectionStatus
{
    ConnectionNo   = 0,
    ConnectionOk   = 1,
    WaitConnection = 2,
};

class MyLibraryServerGui : public QWidget
{
    Q_OBJECT

public:
    explicit MyLibraryServerGui(QWidget *parent = nullptr);
    ~MyLibraryServerGui();

    void sendMessageToTextEdit(const QString& msg);

private:
    void startDB();
    void startServer();

signals:
    void sendMsg(const QString&);

public slots:
    void slot_newActiveUser(qint64 ptr, QString ip);
    void slot_loggedInUser(qint64 ptr, QString login, bool is_admin);
    void slot_disconnectUser(qint64 ptr);

private slots:
    void slot_setConnectionDBStatus(bool isConnected);
    void slot_setConnectionServerStatus(bool isConnected);
    void slot_Scroll_Log();

    void on_autoScroll_stateChanged(int state);

private:
    ConnectionStatus statusConnectionDB     = WaitConnection;
    ConnectionStatus statusConnectionServer = WaitConnection;
    Ui::mylibraryservergui *ui;
    QThread* threadDBAccess;
    QThread* threadServer;
    MyServer* myServer;
    QTimer* scrollTimer;
    QtMessageHandler defaultMsgHandler;
};

#endif // MYLIBRARYSERVERGUI_H
