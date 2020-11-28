#include "databaseaccessor.h"
#include "codes.h"
#include "myclient.h"
#include <QDateTime>

static const char* invokeSlot = "slot_getResultQuery";

static const int reserveTimeSecs = 5*60; // в секундах 5 минут

QString DatabaseAccessor::dbHost;
QString DatabaseAccessor::dbName;
QString DatabaseAccessor::dbUser;
QString DatabaseAccessor::dbPass;

void generateRequest(QString& request, const QString& var, const QString& value, bool& isSetPrevious)
{
    if(!value.isEmpty())
    {
        if(isSetPrevious)
        {
            request += ",";
        }
        else
        {
            isSetPrevious = true;
        }
        request += QString("%1='%2'").arg(var).arg(value);
    }
}

void generateRequest(QString& request, const QString& var, const int& value, bool& isSetPrevious)
{
    if(value != 0)
    {
        if(isSetPrevious)
        {
            request += ",";
        }
        else
        {
            isSetPrevious = true;
        }
        request += QString("%1=%2").arg(var).arg(value);
    }
}

DatabaseAccessor::DatabaseAccessor()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(dbHost);
    db.setDatabaseName(dbName);
    db.setUserName(dbUser);
    db.setPassword(dbPass);
    if (db.open())
    {
        qDebug() << "DatabaseAccessor: SUCCESS connection to database";
        connect(this, &DatabaseAccessor::request, this, &DatabaseAccessor::slot_requestDB);
    }
    else
    {
        qDebug() << "DatabaseAccessor: FAILURE connection to database: " << db.lastError();
    }
}

void DatabaseAccessor::slot_sendConnectionStatus()
{
    emit isConnectedDB(db.isOpen());
}

DatabaseAccessor* DatabaseAccessor::getInstance()
{
    static DatabaseAccessor instance;
    return &instance;
}
void DatabaseAccessor::slot_requestDB(QJsonObject obj)
{
    qDebug() << "DatabaseAccessor: slot_requestDB THREAD= " << QThread::currentThreadId();
    QSqlQuery query(db);

    const int type = obj.value("type").toInt();

    QJsonObject mainObj = obj.value("main").toObject();

    qDebug() << "DatabaseAccessor: mainObj" << mainObj;
    switch (type)
    {
    case Authorization:
    {
        QString login = mainObj.value("login").toString();
        QString pass = mainObj.value("pass").toString();

        query.prepare("SELECT user_id,is_admin FROM users WHERE login =:login and pass=:pass");
        query.bindValue(":login", login);
        query.bindValue(":pass", pass);

        query.exec();

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.first())
        {
            QSqlRecord rec = query.record();
            const int user_id = query.value(rec.indexOf("user_id")).toInt();
            const bool is_admin = query.value(rec.indexOf("is_admin")).toBool();

            replyObjMain.insert("result","yes");
            replyObjMain.insert("user_id", user_id);
            replyObjMain.insert("is_admin", is_admin);

            MyClient* client = qobject_cast<MyClient*>(sender());
            client->setUserId(user_id);
            client->setAdmin(is_admin);
            client->setLogin(login);
        }
        else
        {
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result","no");
        }
        qDebug()<< "DatabaseAccessor: replyObjMain: " << replyObjMain;
        replyObj.insert("main",replyObjMain);

        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case CreateUser:
    {
        QString login = mainObj.value("login").toString();
        QString pass = mainObj.value("pass").toString();
        bool is_admin = mainObj.value("is_admin").toBool();

        query.prepare("INSERT INTO users (login, pass, is_admin)"
                      "VALUES(:login,:pass,:is_admin);");
        query.bindValue(":login", login);
        query.bindValue(":pass", pass);
        query.bindValue(":is_admin", is_admin);

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.exec())
        {
            replyObjMain.insert("result","yes");
        }
        else
        {
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result","no");
        }
        qDebug()<< "DatabaseAccessor: replyObjMain: " << replyObjMain;
        replyObj.insert("main",replyObjMain);

        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case DelUser:
    {
        const int user_id = mainObj.value("user_id").toInt();
        query.prepare("DELETE FROM users where user_id=:user_id");
        query.bindValue(":user_id", user_id);

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.exec())
        {
            replyObjMain.insert("result", "yes");
        }
        else
        {
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result","no");
        }
        replyObj.insert("main", replyObjMain);

        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case AddBook:
    {
        qDebug() << "addBook";
        QString title = mainObj.value("title").toString();
        QString author = mainObj.value("author").toString();
        QString genre = mainObj.value("genre").toString();
        int year_publication = mainObj.value("year_publication").toInt();
        QString photo = mainObj.value("photo").toString();
        QString description = mainObj.value("description").toString();

        query.prepare("INSERT INTO books (title,author,genre,year_publication,photo,description)"
                      "VALUES(:title,:author,:genre,:year_publication,:photo,:description)");

        query.bindValue(":title", title);
        query.bindValue(":author", author);
        query.bindValue(":genre", genre);
        query.bindValue(":year_publication", year_publication);
        query.bindValue(":photo", photo);
        query.bindValue(":description", description);

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.exec())
        {
            replyObjMain.insert("result", "yes");
        }
        else
        {
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result", "no");
        }
        replyObjMain.insert("title", title);

        replyObj.insert("main", replyObjMain);

        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case DelBook:
    {
        const int book_id = mainObj.value("book_id").toInt();

        query.prepare("DELETE FROM books WHERE book_id=:book_id");
        query.bindValue(":book_id", book_id);

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", DelBook);

        if(query.exec())
        {
            replyObjMain.insert("result", "yes");
        }
        else
        {
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result", "no");
        }
        replyObjMain.insert("book_id", book_id);

        replyObj.insert("main", replyObjMain);
        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case GetAllBooks:
    {
        QString request = "SELECT book_id, title, author, genre, year_publication, photo, description, reservation_user_id FROM books";

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.exec(request))
        {
            QSqlRecord rec = query.record();

            QJsonArray books;
            QJsonObject book;

            while(query.next())
            {
                book["book_id"] = query.value(rec.indexOf("book_id")).toInt();
                book["title"] = query.value(rec.indexOf("title")).toString();
                book["author"] = query.value(rec.indexOf("author")).toString();
                book["genre"] = query.value(rec.indexOf("genre")).toString();
                book["year_publication"] = query.value(rec.indexOf("year_publication")).toInt();
                book["photo"] = query.value(rec.indexOf("photo")).toString();
                book["description"] = query.value(rec.indexOf("description")).toString();
                book["reservation_user_id"] = query.value(rec.indexOf("reservation_user_id")).toInt();

                books.append(book);
            }
            replyObjMain.insert("result", "yes");
            replyObjMain.insert("books", books);
        }
        else
        {
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result", "no");
        }

        replyObj.insert("main", replyObjMain);
        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case ReserveBook:
    {
        QDateTime unblocking_time = QDateTime::currentDateTime().addSecs(reserveTimeSecs);

        const int reservation_user_id = mainObj.value("reservation_user_id").toInt();
        const int book_id = mainObj.value("book_id").toInt();

        query.prepare("UPDATE books SET reservation_user_id=:reservation_user_id, time_unblocking=:time_unblocking "
                      "WHERE book_id=:book_id");
        query.bindValue(":reservation_user_id", reservation_user_id);
        query.bindValue(":time_unblocking", unblocking_time);
        query.bindValue(":book_id", book_id);

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.exec())
        {
            qDebug() << "DatabaseAccessor: ReserveBook SUCCESS";
            replyObjMain.insert("result", "yes");
        }
        else
        {
            qDebug() <<query.lastQuery();
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
            replyObjMain.insert("result", "no");
        }

        QTimer::singleShot(reserveTimeSecs*1000, [=]{
            QJsonObject obj;
            QJsonObject mainObj;
            mainObj.insert("book_id", book_id);
            obj.insert("type", Request::UnblockBook);
            obj.insert("main",mainObj);

            emit request(obj);
        });

        replyObjMain.insert("book_id", book_id);

        replyObj.insert("main", replyObjMain);
        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case ChangeBook:
    {
        int book_id = mainObj.value("book_id").toInt();
        int year_publication = mainObj.value("year_publication").toInt();
        QString title = mainObj.value("title").toString();
        QString author = mainObj.value("author").toString();
        QString genre = mainObj.value("genre").toString();
        QString photo = mainObj.value("photo").toString();
        QString description = mainObj.value("description").toString();

        QString request = "UPDATE books SET ";

        bool isSet = false;

        generateRequest(request, "year_publication", year_publication, isSet);
        generateRequest(request, "title", title, isSet);
        generateRequest(request, "author", author, isSet);
        generateRequest(request, "genre", genre, isSet);
        generateRequest(request, "photo", photo, isSet);
        generateRequest(request, "description", description, isSet);

        request += " WHERE book_id=" + QString::number(book_id);

        QJsonObject replyObj;
        QJsonObject replyObjMain;
        replyObj.insert("type", type);

        if(query.exec(request))
        {
            replyObjMain.insert("result", "yes");
            qDebug() << "DatabaseAccessor: ChangeBook SUCCESS; book_id=" << book_id;
        }
        else
        {
            replyObjMain.insert("result", "no");
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
        }
        replyObjMain.insert("book_id", book_id);

        replyObj.insert("main", replyObjMain);
        QMetaObject::invokeMethod(sender(), invokeSlot, Qt::QueuedConnection, Q_ARG(QJsonObject, replyObj));
        break;
    }
    case UnblockBook:
    {
        const int book_id = mainObj.value("book_id").toInt();

        query.prepare("UPDATE books SET reservation_user_id=:reservation_user_id, time_unblocking=:time_unblocking "
                      "WHERE book_id=:book_id");
        query.bindValue(":reservation_user_id", QVariant());
        query.bindValue(":time_unblocking", QVariant());
        query.bindValue(":book_id", book_id);

        if(query.exec())
        {
            qDebug() << "DatabaseAccessor: SUCCESS unblocking book with book_id = " << book_id;
        }
        else
        {
            qDebug() << "DatabaseAccessor: FAILURE unblocking book with book_id = " << book_id;
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
        }

        break;
    }
    case UnblockAllBooks:
    {
        qDebug() << "DatabaseAccessor: UnblockAllBooks";
        if(query.exec("SELECT book_id,time_unblocking FROM books WHERE time_unblocking IS NOT NULL"))
        {
            QSqlRecord rec = query.record();

            const int countRecords = query.size();

            while(query.next())
            {
                int book_id = query.value(rec.indexOf("book_id")).toInt();
                QDateTime time_unblocking = query.value(rec.indexOf("time_unblocking")).toDateTime();

                auto msecs = QDateTime::currentDateTime().msecsTo(time_unblocking);
                if(msecs < 0)
                {
                    msecs = 0;
                }

                QTimer::singleShot(msecs, [=]{
                    QJsonObject obj;
                    QJsonObject mainObj;
                    mainObj.insert("book_id", book_id);
                    obj.insert("type", Request::UnblockBook);
                    obj.insert("main",mainObj);

                    emit request(obj);
                });
            }
            qDebug() << QString("DatabaseAccessor: %1 book(s) will be unblocked").arg(countRecords);
        }
        else
        {
            qDebug() << "DatabaseAccessor: UnblockAllBooks failed";
            qDebug() << "DatabaseAccessor: error : " << query.lastError().text();
        }

        break;
    }
    default:
        qDebug() << "DataBaseAccessor: undefined type of message";
        break;
    }
}

void DatabaseAccessor::slot_unblockAllBooks()
{
    // Разблокировать все книги по истечению времени резервирования
    QJsonObject obj;
    obj.insert("type", Request::UnblockAllBooks);
    emit request(obj);
}


