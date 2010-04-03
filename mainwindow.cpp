#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>

#include "fbrequest.h"
#include "fberror.h"
#include "fblogindialog.h"
#include "fbsession.h"

#include "newsfeedmodel.h"
#include "facebookaccountmodel.h"
#include "facebookaccount.h"

static QString UserId;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_fbSession(FBSession::sessionForApplication("df51def3e750a350ddb961a70b5ab5ab", "3b86a756f77967dea4674f080fa5d345", QString())),
    m_fbLoginDialog ( NULL ),
    m_newsFeedModel(new NewsFeedModel(this)),
    m_facebookAccountModel(new FacebookAccountModel(this))
{
    m_ui->setupUi(this);
    m_ui->postsListView->setModel(m_newsFeedModel);

    connect (m_fbSession,SIGNAL(sessionDidLogin(FBUID)), this, SLOT(sessionDidLogin(FBUID)));
    connect (m_fbSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));

    connect(m_ui->postsListView, SIGNAL(clicked(QModelIndex)), this, SLOT(newsFeedListClicked(QModelIndex)));

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
    UserId = QString::number(aUid,10);

    if (m_fbLoginDialog)
    {
        m_fbLoginDialog->deleteLater();;
        m_fbLoginDialog = NULL;
    }

    FBRequest* request = FBRequest::request();
    Dictionary params;
    //QString query = "select name,pic_big, status,birthday_date, timezone from user where uid in (select uid2 from friend where uid1==" +UserId+ ")";
    QString queryOne = "SELECT post_id, actor_id, target_id, message, permalink FROM stream WHERE source_id in (SELECT target_id FROM connection WHERE source_id=" + UserId + " AND is_following=1) AND is_hidden = 0";
    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT actor_id FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(newsFeedLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("facebook.fql.multiquery",params);
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

void MainWindow::newsFeedLoaded(const QVariant &container)
{
    if (container.type() == QVariant::List) {
        //qDebug() << container.toList();
        QVariantList list = container.toList();

        // Item #0 will be our result set on news items
        foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();

            // Available fields:
            // message, target_id, post_id, actor_id, permalink
            qDebug() << newsFeedPostData["actor_id"] << " said: "
                    << newsFeedPostData["message"]
                    << " at " << newsFeedPostData["permalink"];

            FacebookAccount *account = m_facebookAccountModel->account(newsFeedPostData["actor_id"].toLongLong());

            m_newsFeedModel->createNewsItem(account,
                                            newsFeedPostData["permalink"].toString(),
                                            newsFeedPostData["message"].toString());
            //QHash<QString, QVariant> messageData = newsFeedPost.toHash();
            //qDebug() << messageData["message"];
        }

        foreach (const QVariant &newsFeedUserHash, list.at(1).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedUserData = newsFeedUserHash.toHash();
            qDebug() << newsFeedUserData;

            // Available fields:
            // name,pic_big, status,birthday_date, timezone
            FacebookAccount *account = m_facebookAccountModel->account(newsFeedUserData["id"].toLongLong());
            Q_ASSERT(account);

            account->setName(newsFeedUserData["name"].toString());
            account->setAvatar(newsFeedUserData["pic_square"].toString());
        }

        //qDebug() << secondList;

        // Item #1 will be our result set on user details
        //for (int i = 0; i < list.length(); ++i) {
        //    qDebug() << list.at(i);
        //    qDebug() << "*** END OF ITEM ***";
        //}
    }
}

void MainWindow::newsFeedListClicked(QModelIndex index)
{
    QDesktopServices::openUrl(QUrl(m_newsFeedModel->data(index, NewsFeedModel::UrlRole).toString()));
}

void MainWindow::on_buttonForget_clicked()
{
    m_fbSession->logout();
}

