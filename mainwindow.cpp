/*
 * Copyright (C) 2010 Robin Burchell <robin.burchell@collabora.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>

#include "fbrequest.h"
#include "fberror.h"
#include "fblogindialog.h"
#include "fbsession.h"
#include "fbpermissiondialog.h"

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"
#include "newsfeedpost.h"
#include "facebookaccountmodel.h"
#include "facebookaccount.h"

static QString UserId;

MainWindow::MainWindow(QWidget *parent, FBSession *session) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_fbSession(session),
    m_newsFeedModel(new NewsFeedModel(this)),
    m_facebookAccountModel(new FacebookAccountModel(this))
{
    m_ui->setupUi(this);
    m_ui->postsListView->setModel(m_newsFeedModel);
    m_ui->postsListView->setItemDelegate(new NewsFeedDelegate(this));

    connect(m_ui->action_Logout, SIGNAL(triggered()), this, SLOT(onLogoutMenuAction()));
    connect(m_ui->updateStatusButton, SIGNAL(clicked()), this, SLOT(sendStatusUpdate()));
    connect (m_fbSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));

    connect(m_ui->postsListView, SIGNAL(clicked(QModelIndex)), this, SLOT(newsFeedListClicked(QModelIndex)));

    UserId = QString::number(m_fbSession->uid(), 10);

    fetchNewsFeed();

    //QTimer *t = new QTimer(this);
    //connect(t, SIGNAL(timeout()), SLOT(fetchNewsFeed()));
    //t->start(5000);
    connect(m_ui->checkForNewPosts, SIGNAL(clicked()), this, SLOT(fetchNewsFeed()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
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

void MainWindow::sessionDidLogout()
{
    // TODO: go back to logindialog?
    QApplication::exit();
}

void MainWindow::requestFailedWithFacebookError(const FBError &error)
{
    qDebug() << "facebook error is " << error.code() << " - " << error.description();

    // If it's a permissions error, request permission.
    if (error.code() == 250) {
        // Find permission requested
        // XXX: is there any easier way to do this?
        QString permission = error.description().split(QChar(' ')).last();
        qDebug() << "About to request " << permission;

        // XXX: Deallocated in the slots. This isn't a good way to do MM, come up with a better design :(
        // XXX: the error condition (dialogDidFailWithError()) in particular currently leaks, and might recurse - not good.
        FBPermissionDialog *d = new FBPermissionDialog(this->m_fbSession);
        connect(d, SIGNAL(dialogDidSucceed()), this, SLOT(permissionGranted()));
        connect(d, SIGNAL(dialogDidCancel()), this, SLOT(permissionDeniedOrCancelled()));
        connect(d, SIGNAL(dialogDidFailWithError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
        d->setPermissionToRequest(permission);
        d->load();
        d->show();
    }
}

void MainWindow::permissionGranted()
{
    qDebug() << "Permission granted";
    sender()->deleteLater();
}

void MainWindow::permissionDeniedOrCancelled()
{
    qDebug() << "Permission denied";
    sender()->deleteLater();
}

void MainWindow::newsFeedLoaded(const QVariant &container)
{
    if (container.type() == QVariant::List) {
        // TODO: this should really not be needed, just insertItem() and have the model handle inserting/sorting by timestamp.
        bool prepend = m_newsFeedModel->rowCount(QModelIndex()) != 0;

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

            NewsFeedPost *np = new NewsFeedPost(m_newsFeedModel,
                                                account,
                                                newsFeedPostData["created_time"].toLongLong(),
                                                newsFeedPostData["permalink"].toString(),
                                                newsFeedPostData["message"].toString());
            if (prepend)
                m_newsFeedModel->prependNewsItem(np);
            else
                m_newsFeedModel->appendNewsItem(np);

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

    // Tell the newsfeed fetcher about the updated timestamp.
    fetchNewsFeed(m_newsFeedModel->newestCreatedTime());
}

void MainWindow::newsFeedListClicked(QModelIndex index)
{
    QDesktopServices::openUrl(QUrl(m_newsFeedModel->data(index, NewsFeedModel::UrlRole).toString()));
}

void MainWindow::fetchNewsFeed(long long timeStamp)
{
    static int lastNewsPost = 0;

    if (timeStamp != -1) {
        // HACK: I don't really like this. Maybe use a member instead?
        lastNewsPost = timeStamp;
        qDebug() << "fetchNewsFeed: lastNewsPost is now " << lastNewsPost;
        return;
    }

    FBRequest* request = FBRequest::request();
    Dictionary params;
    //QString query = "select name,pic_big, status,birthday_date, timezone from user where uid in (select uid2 from friend where uid1==" +UserId+ ")";

    QString queryOne = "SELECT post_id, actor_id, target_id, message, permalink, created_time FROM stream WHERE filter_key in (SELECT filter_key FROM stream_filter WHERE uid=" + UserId + " AND type='newsfeed') AND is_hidden = 0";

    if (lastNewsPost != 0) {
        // Fetch all newer posts
        queryOne += " AND created_time > " + QString::number(lastNewsPost);
    }

    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT actor_id FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    qDebug() << "fetchNewsFeed: Sending " << fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(newsFeedLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("facebook.fql.multiquery",params);
}

void MainWindow::onLogoutMenuAction()
{
    m_fbSession->logout();
}

void MainWindow::sendStatusUpdate()
{
    FBRequest* request = FBRequest::request();
    Dictionary params;
    params["status"] = m_ui->statusText->toPlainText();

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(statusUpdated(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("Status.set",params);
}

void MainWindow::statusUpdated(const QVariant &)
{
    qDebug() << "Status updated!";
    m_ui->statusText->setPlainText(QLatin1String(""));

    // Trigger a check for our own post
    fetchNewsFeed();
}
