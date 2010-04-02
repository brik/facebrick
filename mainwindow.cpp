#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QDebug>
#include <QMessageBox>

#include "fbrequest.h"
#include "fberror.h"
#include "fblogindialog.h"
#include "fbsession.h"

#include "newsfeedmodel.h"

static QString UserId;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_fbSession(FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab", "3b86a756f77967dea4674f080fa5d345", QString())),
    m_fbLoginDialog ( NULL ),
    m_newsFeedModel(new NewsFeedModel(this))
{
    m_ui->setupUi(this);

    connect (m_fbSession,SIGNAL(sessionDidLogin(FBUID)), this, SLOT(sessionDidLogin(FBUID)));
    connect (m_fbSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));

    if (m_fbSession->resume() == false)
    {
        m_fbLoginDialog = new FBLoginDialog();
        m_fbLoginDialog->show();
    }
}

MainWindow::~MainWindow()
{
    delete m_ui;
    if (m_fbLoginDialog)
        delete m_fbLoginDialog;
    delete m_fbSession;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::sessionDidLogin(FBUID aUid)
{
    QMessageBox msgbox;
    UserId = QString::number(aUid,10);
    QString msg ("Logged in sucessfully, your FBUID is " + UserId);
    msgbox.setText(msg);
    msgbox.exec();

    if (m_fbLoginDialog)
    {
        m_fbLoginDialog->deleteLater();;
        m_fbLoginDialog = NULL;
    }

    FBRequest* request = FBRequest::request();
    Dictionary params;
    QString query = "select name,pic_big, status,birthday_date, timezone from user where uid in (select uid2 from friend where uid1==" +UserId+ ")";
    params["query"] = query;
    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(friendsRequestLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("facebook.fql.query",params);
}

void MainWindow::sessionDidLogout()
{
    QMessageBox msgbox;
    msgbox.setText("logged out");
    msgbox.exec();
}

void MainWindow::requestFailedWithFacebookError ( const FBError& aError )
{
    qDebug() << "facebook error is " << aError.code() << " - " << aError.description();
}

void MainWindow::friendsRequestLoaded(const QVariant& aContainer)
{
    if (aContainer.type() == QVariant::List)
    {
        QVariantList list = aContainer.toList();

        for (int i = 0 ; i < list.count(); i ++)
        {
            QVariantHash dictionary = list.at(i).toHash();
            QHashIterator<QString, QVariant> iterator(dictionary);

            QString name = dictionary.value("name").toString();
            m_ui->listWidget->addItem(name);


        }

        sender()->deleteLater();
    }
}



void MainWindow::on_buttonForget_clicked()
{
    m_fbSession->logout();
}
