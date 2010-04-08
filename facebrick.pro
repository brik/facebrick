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
    mainwindow_statusupdate.cpp \
    newsfeedpostview.cpp \
    newsfeedcomment.cpp \
    newsfeedcommentsmodel.cpp

HEADERS  += mainwindow.h \
    newsfeedmodel.h \
    newsfeedpost.h \
    facebookaccountmodel.h \
    facebookaccount.h \
    newsfeeddelegate.h \
    logindialog.h \
    newsfeedpostview.h \
    newsfeedcomment.h \
    newsfeedcommentsmodel.h

FORMS    += mainwindow.ui \
    newsfeedpostview.ui


INCLUDEPATH = ../libqfacebook/inc/
win32:LIBS += ../debug/libqfacebookconnect.a
unix:LIBS += /usr/lib/libqfacebookconnect.so

target.path = /usr/bin

maemo5:* {
    INSTALLS += desktop icon
    desktop.path =  /usr/share/applications/hildon
    desktop.files += facebrick.desktop

    icon.path = /usr/share/pixmaps
    icon.files += facebrick.png
}



INSTALLS += target
