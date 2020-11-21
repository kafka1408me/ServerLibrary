#ifndef CODES_H
#define CODES_H

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

    // Эти коды использует только сервер!!!
    UnblockBook     = 100,
    UnblockAllBooks = 101
};

#endif // CODES_H
