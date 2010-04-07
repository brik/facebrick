#-------------------------------------------------
#
# Project created by QtCreator 2009-11-20T00:54:21
#
#-------------------------------------------------

QT       += network webkit xml

maemo5:* {
    QT += maemo5
}

TARGET = facebrick
TEMPLATE = app
DESTDIR = ../debug

SOURCES += main.cpp\
        mainwindow.cpp \
    newsfeedmodel.cpp \
    newsfeedpost.cpp \
    facebookaccountmodel.cpp \
    facebookaccount.cpp \
    newsfeeddelegate.cpp \
    logindialog.cpp \
    mainwindow_newsfeed.cpp \
    mainwindow_errorhandling.cpp \
    mainwindow_statusupdate.cpp

HEADERS  += mainwindow.h \
    newsfeedmodel.h \
    newsfeedpost.h \
    facebookaccountmodel.h \
    facebookaccount.h \
    newsfeeddelegate.h \
    logindialog.h

FORMS    += mainwindow.ui


INCLUDEPATH = ../inc/
win32:LIBS += ../debug/libqfacebookconnect.a
unix:LIBS += /usr/lib/libqfacebookconnect.so

target.path = /usr/bin

maemo5:* {
    INSTALLS += desktop
    desktop.path =  /usr/local/share/applications/hildon
    desktop.files += facebrick.desktop
}



INSTALLS += target
