QT       += network webkit xml

maemo5:* {
    QT += maemo5
}

TARGET = facebrick
TEMPLATE = app
#DESTDIR = 

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/newsfeedmodel.cpp \
    src/newsfeedpost.cpp \
    src/facebookaccountmodel.cpp \
    src/facebookaccount.cpp \
    src/newsfeeddelegate.cpp \
    src/mainwindow_newsfeed.cpp \
    src/mainwindow_errorhandling.cpp \
    src/mainwindow_statusupdate.cpp \
    src/newsfeedpostview.cpp \
    src/newsfeedpostview_addcomment.cpp \
    src/newsfeedpostview_fetchcomments.cpp \
    src/newsfeedpostview_likedislike.cpp \
    src/facebrick.cpp \
    src/newsfeedpostattachment.cpp \
    src/settingsdialog.cpp \
    src/getconnection.cpp \
    src/fblogindialog.cpp \
    src/fbdialog.cpp

HEADERS  += include/mainwindow.h \
    include/newsfeedmodel.h \
    include/newsfeedpost.h \
    include/facebookaccountmodel.h \
    include/facebookaccount.h \
    include/newsfeeddelegate.h \
    include/newsfeedpostview.h \
    include/facebrick.h \
    include/settingsdialog.h \
    include/graphapi.h \
    include/getconnection.h \
    include/fblogindialog.h \
    include/fbdialog.h

FORMS    += src/ui/mainwindow.ui \
    src/ui/newsfeedpostview.ui \
    src/ui/settingsdialog.ui


INCLUDEPATH += ../qfacebook/src
INCLUDEPATH += include/
win32:LIBS += ../qfacebook/lib/libqfacebook.a
unix:LIBS += ../qfacebook/lib/libqfacebook.so

INCLUDEPATH += ../libqfacebook/inc/
INCLUDEPATH += include/
win32:LIBS += ../libqfacebook/debug/libqfacebookconnect.a
unix:LIBS += ../libqfacebook/debug/libqfacebookconnect.so

target.path = /usr/bin

maemo5:* {
    INSTALLS += desktop icon
    desktop.path =  /usr/share/applications/hildon
    desktop.files += facebrick.desktop

    icon.path = /usr/share/pixmaps
    icon.files += facebrick.png
}

INSTALLS += target
