#include <QtGui/QApplication>
#include <QDebug>

#include "qmaemo5homescreenadaptor.h"

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
    NewsFeed::instance();

    DesktopWidget w;

    if (QCoreApplication::arguments().count() > 1 && QCoreApplication::arguments().at(1) == "desktop") {     
        QMaemo5HomescreenAdaptor *adaptor = new QMaemo5HomescreenAdaptor(&w);
        adaptor->setSettingsAvailable(true);
        QObject::connect(adaptor, SIGNAL(settingsRequested()), &w, SLOT(showSettingsDialog()));

        w.show();
    }
    else {
        MainWindow::instance()->show();
    }

    int retval = a.exec();
    delete FaceBrick::instance();
    delete NewsFeed::instance();
    delete session;
    return retval;
}
