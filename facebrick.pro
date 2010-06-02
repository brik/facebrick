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
    src/keyhandler.cpp

HEADERS  += include/mainwindow.h \
    include/newsfeedmodel.h \
    include/newsfeedpost.h \
    include/facebookaccountmodel.h \
    include/facebookaccount.h \
    include/newsfeeddelegate.h \
    include/newsfeedpostview.h \
    include/facebrick.h \
    include/keyhandler.h

FORMS    += src/ui/mainwindow.ui \
    src/ui/newsfeedpostview.ui


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
