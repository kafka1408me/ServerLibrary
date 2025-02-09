#include "mylibraryservergui.h"
#include "ui_mylibraryservergui.h"
#include "myserver.h"
#include "databaseaccessor.h"
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>
#include <QDateTime>
#include "myclient.h"
#include "MyLog.h"


//#define MSG_TYPE  // Раскоментировать эту строку, чтобы выводился в лог тип сообщений

const int timeScrollingMSecs = 1*500;

const int port = 4735;

static MyLibraryServerGui* myLibServ = nullptr;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
#ifdef MSG_TYPE
    static QString myMsg("%1 : %2 : %3");
#else
    static QString myMsg("%1 : %2");
#endif

    QDateTime currentTime = QDateTime::currentDateTime();
    QString msgToSend;
    QString typeMsg("");

    bool isDebug = false;

    switch (type) {
    case QtDebugMsg:
    {
        isDebug = true;
#ifdef MSG_TYPE
        typeMsg = "DEBUG";
#endif
        break;
    }
#ifdef MSG_TYPE
    case QtInfoMsg:
    {
        typeMsg = "INFO";
        break;
    }
    case QtWarningMsg:
    {
        typeMsg = "WARING";
        break;
    }
    case QtCriticalMsg:
    {
        typeMsg = "CRITICAL";
        break;
    }
    case QtFatalMsg:
    {
        typeMsg = "FATAL";
        break;
    }
#else
    default:
        break;
#endif
    }
#ifdef MSG_TYPE
    msgToSend = myMsg.arg(currentTime.toString()).arg(typeMsg).arg(msg);
#else
    msgToSend = myMsg.arg(currentTime.toString()).arg(msg);
#endif
    if(!isDebug)
    {
        msgToSend += " " + QString(context.file) + " " + QString(context.line) + " " + QString(context.function);
    }
    myLibServ->sendMessageToTextEdit(msgToSend);
}


MyLibraryServerGui::MyLibraryServerGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mylibraryservergui)
{
    ui->setupUi(this);

    myLibServ = this;

    myLog = new MyLog(this);

    connect(this, &MyLibraryServerGui::sendMsg , ui->logText, &QTextEdit::append);
    connect(this, &MyLibraryServerGui::sendMsg , myLog, &MyLog::Log);

    defaultMsgHandler = qInstallMessageHandler(myMessageOutput);

    QtConcurrent::run(this, &MyLibraryServerGui::startServer);
    QtConcurrent::run(this, &MyLibraryServerGui::startDB);

    scrollTimer = new QTimer(this);
    connect(scrollTimer, &QTimer::timeout, this, &MyLibraryServerGui::slot_Scroll_Log);
    scrollTimer->setInterval(timeScrollingMSecs);

    if(ui->autoScroll->isChecked())
    {
        scrollTimer->start();
    }

    ui->portLbl->hide();
    ui->portNumLbl->hide();
    ui->logText->setReadOnly(true);
    ui->portNumLbl->setNum(port);

    QStringList listColumnNames;
    listColumnNames << "ip" << "login" << "isAdmin" << "ptr";
    ui->tableWidget->setColumnCount(4);

    ui->tableWidget->setHorizontalHeaderLabels(listColumnNames);
    ui->tableWidget->hideColumn(3);

    setWindowTitle("My Library Server");

    qDebug() << "MainThread = " << QThread::currentThreadId();
}

MyLibraryServerGui::~MyLibraryServerGui()
{
    QString myMsg("%1 : %2");
    QDateTime currentTime = QDateTime::currentDateTime();
    myLog->Log(myMsg.arg(currentTime.toString()).arg("SERVER FINISHED"));

    qInstallMessageHandler(defaultMsgHandler);
    threadServer->exit();
    threadDBAccess->exit();
    delete ui;
}

void MyLibraryServerGui::sendMessageToTextEdit(const QString &msg)
{
    emit sendMsg(msg);
}


void MyLibraryServerGui::slot_setConnectionDBStatus(bool isConnected)
{
    if(statusConnectionDB == int(isConnected))
    {
        return;
    }
    statusConnectionDB = ConnectionStatus(isConnected);

    QString backGroundColor = isConnected? "rgb(20,190,0)" : "rgb(230,24,0)";

    ui->connectDbStatus->setStyleSheet(QString("background-color:%1;"
                                               "border-radius: 15px;"
                                               "border-width: 6px;"
                                               "border-style: solid;"
                                               "border-color:rgb(150,150,150);").arg(backGroundColor));
}

void MyLibraryServerGui::slot_setConnectionServerStatus(bool isConnected)
{
    if(statusConnectionServer == int(isConnected))
    {
        return;
    }
    statusConnectionServer = ConnectionStatus(isConnected);

    QString backGroundColor = isConnected? "rgb(20,190,0)" : "rgb(230,24,0)";

    ui->connectServerStatus->setStyleSheet(QString("background-color:%1;"
                                                   "border-radius: 15px;"
                                                   "border-width: 6px;"
                                                   "border-style: solid;"
                                                   "border-color:rgb(150,150,150);").arg(backGroundColor));

    ui->portLbl->setHidden(!isConnected);
    ui->portNumLbl->setHidden(!isConnected);

}

void MyLibraryServerGui::slot_Scroll_Log()
{
    static QTextCursor cursor = ui->logText->textCursor();

    cursor.movePosition(QTextCursor::End);
    ui->logText->setTextCursor(cursor);
}

void MyLibraryServerGui::slot_newActiveUser(qint64 ptr, QString ip)
{
    int countRow = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(countRow);
    ui->tableWidget->setItem(countRow ,0, new QTableWidgetItem(ip));
    ui->tableWidget->setItem(countRow ,1, new QTableWidgetItem(""));
    ui->tableWidget->setItem(countRow ,2, new QTableWidgetItem(""));
    ui->tableWidget->setItem(countRow ,3, new QTableWidgetItem(QString::number(ptr)));
}

void MyLibraryServerGui::slot_loggedInUser(qint64 ptr, QString login, bool is_admin)
{
    for(int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        qint64 ptr_in = ui->tableWidget->item(row, 3)->text().toInt();
        if(ptr_in == ptr)
        {
            ui->tableWidget->setItem(row ,1, new QTableWidgetItem(login));
            ui->tableWidget->setItem(row ,2, new QTableWidgetItem(is_admin ? "true" : "false"));
            break;
        }
    }
}

void MyLibraryServerGui::slot_disconnectUser(qint64 ptr)
{
    for(int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        qint64 ptr_in = ui->tableWidget->item(row, 3)->text().toInt();
        if(ptr_in == ptr)
        {
            ui->tableWidget->removeRow(row);
            break;
        }
    }
    qobject_cast<MyClient*>(sender())->deleteLater();
}

void MyLibraryServerGui::startDB()
{
    DatabaseAccessor::dbHost="192.168.1.70";  // localhost должен быть!
    DatabaseAccessor::dbName="library";
    DatabaseAccessor::dbUser = "postgres";
    DatabaseAccessor::dbPass = "simplepass";
    DatabaseAccessor::getInstance();

    connect(DatabaseAccessor::getInstance(), &DatabaseAccessor::isConnectedDB, this, &MyLibraryServerGui::slot_setConnectionDBStatus);

    DatabaseAccessor::getInstance()->slot_sendConnectionStatus();

    threadDBAccess = new QThread;
    DatabaseAccessor::getInstance()->moveToThread(threadDBAccess);
    threadDBAccess->start();
    DatabaseAccessor::getInstance()->slot_unblockAllBooks();
}

void MyLibraryServerGui::startServer()
{
    myServer = new MyServer(port);
    myServer->setMyLibServer(this);

    connect(myServer, &MyServer::serverIsConnected, this, &MyLibraryServerGui::slot_setConnectionServerStatus);

    myServer->StartServer();
    myServer->slot_sendConnectionStatus();

    threadServer = new QThread;
    myServer->moveToThread(threadServer);
    threadServer->start();
}

void MyLibraryServerGui::on_autoScroll_stateChanged(int state)
{
    if(!state)
    {
        scrollTimer->stop();
    }
    else
    {
        scrollTimer->start();
    }
}

