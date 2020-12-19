#ifndef CODES_H
#define CODES_H

#include <QObject>

//enum Request
//{
//    // Эти коды используют клиенты
//    Authorization   = 1,
//    CreateUser      = 2,
//    DelUser         = 3,
//    AddBook         = 4,
//    DelBook         = 5,
//    GetAllBooks     = 6,
//    ReserveBook     = 7,
//    ChangeBook      = 8,

//    Check           =30,
//    RateAppSet      =31,

//    // Эти коды использует только сервер!!!
//    UnblockBook     = 100,
//    UnblockAllBooks = 101
//};

//#endif // CODES_H



namespace Codes
{
    Q_NAMESPACE         // required for meta object creation
enum Request
{
    // Эти коды используют клиенты
    Authorization   = 1,
    CreateUser      = 2,
    DelUser          = 3,
    AddBook         = 4,
    DelBook         = 5,
    GetAllBooks     = 6,
    ReserveBook     = 7,
    ChangeBook      = 8,

    Check          = 30,
    RateAppSet      =31,

    // Эти коды использует только сервер!!!
    UnblockBook     = 100,
    UnblockAllBooks = 101
};
    Q_ENUM_NS(Request)  // register the enum in meta object data
}

using namespace Codes;
#endif
