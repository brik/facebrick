#include <QtGui/QApplication>
#include <QDebug>
#include <QTimer>

#include "qfactoryinterface.h"

#include "mainwindow.h"
#include "facebrick.h"

#include "graphapi.h"
#include "fblogindialog.h"

#include "getconnection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString clientId = "", clientSecret ="";
    FaceBrick::instance(clientId, clientSecret);
    FBLogin *m_fbLoginDialog = new FBLogin(clientId);
    m_fbLoginDialog->show();

    MainWindow w(0);
    w.show();

    int retval = a.exec();
    delete FaceBrick::instance();
    return retval;
}
