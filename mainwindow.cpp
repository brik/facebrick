#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QDebug>
#include <QMessageBox>

#include "fbrequest.h"
#include "fberror.h"

static QString UserId;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iFBSession = FBSession::sessionForApplication("b4db7267221ecf6333e60c1213d724cb", "f054071e18bd026c0b2e2f97ff68a7c5", QString());
    connect (iFBSession,SIGNAL(sessionDidLogin(FBSession*,FBUID)), this, SLOT(sessionDidLogin(FBSession*,FBUID)));
}

MainWindow::~MainWindow()
{
    delete iFBSession;
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (iFBSession->resume() == false)
    {
        iLoginDialog = new FBLoginDialog();
        iLoginDialog->setFixedSize(640,320);
        iLoginDialog->show();
    }
}

void MainWindow::sessionDidLogin(FBSession*,FBUID aUid)
{
    QMessageBox msgbox;
    UserId = QString::number(aUid,10);
    QString msg ("Logged in sucessfully, your FBUID is " + UserId);
    msgbox.setText(msg);
    msgbox.exec();

    if (iLoginDialog)
    {
        delete iLoginDialog;
        iLoginDialog = NULL;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    FBRequest* request = FBRequest::request();
    Dictionary params;
    QString query = "select name from user where uid == " + UserId;
    params["query"] = query;
    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(requestDidLoad(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("facebook.fql.query",params);

}

void MainWindow::requestFailedWithFacebookError ( const FBError& aError )
{
    qDebug() << "facebook error is " << aError.code() << " - " << aError.description();
}

void MainWindow::requestDidLoad(const QVariant& aContainer)
{
    if (aContainer.type() == QVariant::List)
    {
        QVariantList list = aContainer.toList();

        QVariantHash dictionary = list.at(0).toHash();

        QHashIterator<QString, QVariant> i(dictionary);

        while (i.hasNext())
        {
            i.next();
            QString key = i.key();
            QString val = i.value().toString();
        }

    }

    sender()->deleteLater();
}

void MainWindow::on_pushButton_3_clicked()
{
    FBRequest* request = FBRequest::request();
    Dictionary params;
    QString query = "select name,pic_big, status,birthday_date, timezone from user where uid in (select uid2 from friend where uid1==" +UserId+ ")";
    params["query"] = query;
    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(requestDidLoad(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("facebook.fql.query",params);
}
