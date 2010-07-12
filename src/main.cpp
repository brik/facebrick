#include <QtGui/QApplication>
#include <QDebug>

#include "fbsession.h"
#include "fblogindialog.h"

#include "mainwindow.h"
#include "facebrick.h"
#include "newsfeed.h"
#include "desktopwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FBSession *session = FBSession::sessionForApplication("fd707be7594bbb68b91bf86cc1870085", QString(), QString("http://w00t.dereferenced.net/facebook/"));

    if (session->resume() == false)
    {
        // Never logged in, let them login!
        FBLoginDialog *m_fbLoginDialog = new FBLoginDialog();
        m_fbLoginDialog->show();
    }

    // Initialise us
    FaceBrick::instance(session);
    NewsFeed::instance(0);

    DesktopWidget w;
    w.show();
    //MainWindow w(0);
    //w.show();

    int retval = a.exec();
    delete FaceBrick::instance();
    delete session;
    return retval;
}
