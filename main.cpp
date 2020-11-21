#include <QApplication>

#include "mylibraryservergui.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyLibraryServerGui myLibServerGui;
    myLibServerGui.show();

    return a.exec();
}

