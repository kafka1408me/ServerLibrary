QT       += core sql network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    databaseaccessor.cpp \
    main.cpp \
    myclient.cpp \
    mylibraryservergui.cpp \
    myserver.cpp #\
#    mytask.cpp

HEADERS += \
  codes.h \
  databaseaccessor.h \
  myclient.h \
  mylibraryservergui.h \
  myserver.h #\
#  mytask.h

RC_FILE = myapp.rc


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
  mylibraryservergui.ui

CONFIG(debug, debug|release){
  DESTDIR = $${PWD}\\MyLibraryServer_debug
}else{
  DESTDIR = $${PWD}\\MyLibraryServer_release
}

# разделяем по директориям все выходные файлы проекта
MOC_DIR = $${PWD}/tmp
RCC_DIR = $${PWD}/tmp
UI_DIR = $${PWD}/tmp
win32:OBJECTS_DIR = $${PWD}/tmp

PWD_WIN = $${PWD}
PWD_WIN ~= s,/,\\,g

DESTDIR_WIN = $${DESTDIR}
DESTDIR_WIN ~= s,/,\\,g


# Copies the given files to the destination directory
defineTest(copyToDestdir) {
       files = $$1

       for(FILE, files) {

           # Replace slashes in paths with backslashes for Windows
           win32:FILE ~= s,/,\\,g

           QMAKE_POST_LINK += $$QMAKE_COPY $$quote(\"$$FILE\") $$quote(\"$$DESTDIR_WIN\") $$escape_expand(\\n\\t)
       }
       export(QMAKE_POST_LINK)
}


defineTest(copyToPlatform) {
       files = $$1

       for(FILE, files) {

           # Replace slashes in paths with backslashes for Windows
           win32:FILE ~= s,/,\\,g

           QMAKE_POST_LINK += $$QMAKE_COPY $$quote(\"$$FILE\") $$quote(\"$$DESTDIR_WIN\\platforms\") $$escape_expand(\\n\\t)
#           QMAKE_POST_LINK += $$quote(cmd /c copy /y $${FILE} $${DESTDIR_WIN}\\platforms$$escape_expand(\\n\\t))
       }
       export(QMAKE_POST_LINK)
}

defineTest(copyToSqldrivers) {
       files = $$1

       for(FILE, files) {

           # Replace slashes in paths with backslashes for Windows
           win32:FILE ~= s,/,\\,g

           QMAKE_POST_LINK += $$QMAKE_COPY $$quote(\"$$FILE\") $$quote(\"$$DESTDIR_WIN\\sqldrivers\") $$escape_expand(\\n\\t)
#           QMAKE_POST_LINK += $$quote(cmd /c copy /y $${FILE} $${DESTDIR_WIN}\\sqldrivers$$escape_expand(\\n\\t))
       }
       export(QMAKE_POST_LINK)
}


win32 {
win32-g++:contains(QMAKE_HOST.arch, x86_64):{
    copyToDestdir("$${PWD_WIN}\\postgresql-12.4/x64/libpq.dll") #for win64
} else {
    copyToDestdir("$${PWD_WIN}\\postgresql-12.4/x86/libpq.dll") #for win32
}
}



CONFIG(debug, debug|release){

}else{
  QMAKE_POST_LINK += $$quote(cmd /c mkdir \"$${DESTDIR_WIN}\\platforms\"$$escape_expand(\\n\\t))
  QMAKE_POST_LINK += $$quote(cmd /c mkdir \"$${DESTDIR_WIN}\\sqldrivers\"$$escape_expand(\\n\\t))

  copyToDestdir("$$(QTDIR)/bin/Qt5Gui.dll")
  copyToDestdir("$$(QTDIR)/bin/Qt5Core.dll")
  copyToDestdir("$$(QTDIR)/bin/Qt5Sql.dll")
  copyToDestdir("$$(QTDIR)/bin/Qt5Network.dll")
  copyToDestdir("$$(QTDIR)/bin/Qt5WebSockets.dll")
  copyToDestdir("$$(QTDIR)/bin/Qt5Widgets.dll")

  copyToDestdir("$$(QTDIR)/bin/libgcc_s_seh-1.dll")
  copyToDestdir("$$(QTDIR)/bin/libwinpthread-1.dll")
  copyToPlatform("$$(QTDIR)/plugins/platforms/qwindows.dll")
  copyToSqldrivers("$$(QTDIR)/plugins/sqldrivers/qsqlpsql.dll")

  copyToDestdir("$$(QTDIR)/bin/libstdc++-6.dll")
}



