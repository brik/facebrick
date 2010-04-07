#include <QtGui/QApplication>

#include "fbsession.h"

#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FBSession *session = FBSession::sessionForApplication("fd707be7594bbb68b91bf86cc1870085", QString(), QString("http://w00t.dereferenced.net/facebook/"));

    if (session->resume() == false)
    {
        LoginDialog d(session);
    }

    MainWindow w(0, session);
    w.show();
    return a.exec();
}
