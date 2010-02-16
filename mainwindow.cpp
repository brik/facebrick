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
    ui(new Ui::MainWindow),
    iLoginDialog ( NULL )
{
    ui->setupUi(this);

    iFBSession = FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab", "3b86a756f77967dea4674f080fa5d345", QString());
    connect (iFBSession,SIGNAL(sessionDidLogin(FBUID)), this, SLOT(sessionDidLogin(FBUID)));
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
        iLoginDialog->show();
    }
}

void MainWindow::sessionDidLogin(FBUID aUid)
{
    QMessageBox msgbox;
    UserId = QString::number(aUid,10);
    QString msg ("Logged in sucessfully, your FBUID is " + UserId);
    msgbox.setText(msg);
    msgbox.exec();

    if (iLoginDialog)
    {
        iLoginDialog->deleteLater();;
        iLoginDialog = NULL;
    }

}

void MainWindow::on_pushButton_2_clicked()
{
    FBRequest* request = FBRequest::request();
    Dictionary params;
    QString query = "select name,pic_big, status,birthday_date, timezone from user where uid in (select uid2 from friend where uid1==" +UserId+ ")";
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

        for (int i = 0 ; i < list.count(); i ++)
        {
            QVariantHash dictionary = list.at(i).toHash();
            QHashIterator<QString, QVariant> iterator(dictionary);

            QString name = dictionary.value("name").toString();
            ui->listWidget->addItem(name);


        }

        sender()->deleteLater();
    }
}



void MainWindow::on_buttonForget_clicked()
{
    iFBSession->logout();
}
