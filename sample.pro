#-------------------------------------------------
#
# Project created by QtCreator 2009-11-20T00:54:21
#
#-------------------------------------------------

QT       += network webkit xml

TARGET = sample
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
    mainwindow_newsfeed.cpp

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
unix:LIBS += ../debug/libqfacebookconnect.so
