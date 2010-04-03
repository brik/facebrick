#include <QtGui/QApplication>

#include "fbsession.h"

#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FBSession *session = FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab", "3b86a756f77967dea4674f080fa5d345", QString());

    if (session->resume() == false)
    {
        LoginDialog d(session);
    }

    MainWindow w(0, session);
    w.show();
    return a.exec();
}
