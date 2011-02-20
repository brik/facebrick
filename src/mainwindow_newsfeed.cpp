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

#include <QDebug>
#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "fbrequest.h"
#include "fbsession.h"
#include "fberror.h"

#include "facebrick.h"
#include "facebookaccount.h"
#include "facebookaccountmodel.h"
#include "newsfeedpost.h"
#include "newsfeedmodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newsfeedpostview.h"

#include "getconnection.h"

void MainWindow::newsFeedListClicked(QModelIndex index)
{
    NewsFeedPostView *nfpv = new NewsFeedPostView(this);

    // Yes, I *know* this line is ugly.
    nfpv->setPost(static_cast<NewsFeedPost *>(m_newsFeedModel->data(index, NewsFeedModel::PostRole).value<void *>()));
    nfpv->show();
}

void MainWindow::fetchNewsFeed()
{
    // Lock
    if (m_updatingNewsFeed) {
        qDebug() << "fetchNewsFeed: Already updating...";
        return;
    }

    m_updatingNewsFeed = true;
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

    connect (FaceBrick::instance()->m_connection, SIGNAL(requestDidLoad(QVariant)), this, SLOT(newsFeedLoaded(QVariant)));
    // connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(newsFeedLoadingError(FBError)));
    FaceBrick::instance()->m_connection->load("me", "home");
}

void MainWindow::newsFeedLoadingError(const FBError &error)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error loading newsfeed: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    m_updatingNewsFeed = false;
    requestFailedWithFacebookError(error, true);
}

void MainWindow::newsFeedLoaded(const QVariant &container)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
    m_updatingNewsFeed = false;
    qDebug() << "newsfeed loaded";

    foreach (const QVariant &newsFeedPostHash, container.toMap()["data"].toList()) {
        QMap<QString, QVariant> newsFeedPostData = newsFeedPostHash.toMap();

        qDebug() << newsFeedPostData["from"].toMap()["name"].toString();

        // Fetch (or create) the account that made this newsfeed post
        FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedPostData["from"].toMap()["id"].toLongLong());
        Q_ASSERT(account);

        // Create a new newsfeed post
        NewsFeedPost *np = new NewsFeedPost(m_newsFeedModel,
                                            account,
                                            newsFeedPostData["id"].toString(),
                                            newsFeedPostData["created_time"].toDateTime(),
                                            newsFeedPostData["link"].toString(),
                                            newsFeedPostData["message"].toString());

        account->setName(newsFeedPostData["from"].toMap()["name"].toString());

        QUrl imageUrl = "http://graph.facebook.com/" + newsFeedPostData["from"].toMap()["id"].toString() + "/picture";
        account->setAvatar(imageUrl);
        Q_ASSERT(np);

        // Seed it into the model
        m_newsFeedModel->insertNewsItem(np);

/*
        // Update our 'recent posts' block badger.
        if (np->createdTime() > m_lastUpdatedNewsFeed)
            m_lastUpdatedNewsFeed = np->createdTime();

        // Process like info too.
        bool iLikeThis = newsFeedPostData["likes"].toHash()["user_likes"].toString() == "1";
        np->setILikeThis(iLikeThis);

        // *breathe deeply* ok, and now let's try manage attachments
        QHash<QString, QVariant> attachmentHash = newsFeedPostData["attachment"].toHash();

        // Facebook... why not just *not* send attachment data if there isn't one?
        if (attachmentHash.count() == 1)
            continue;

        // Ignore "sent from my mobile" - there should be a better way to do this
        if (attachmentHash["href"].toString() == "http://www.facebook.com")
            continue;

        // yes, apparantly this can happen!
        if (attachmentHash["media"].toList().count() == 0)
            continue;

        // GOD DAMMIT I HATE THIS BOXING.
        QHash<QString, QVariant> mediaInfo = attachmentHash["media"].toList().at(0).toHash();
        qDebug() << mediaInfo;

        np->setHasAttachment(true);
        np->setDescription(attachmentHash["description"].toString());
        np->setAttachmentName(mediaInfo["name"].toString());
        // TODO: href?
        np->setThumbnail(mediaInfo["src"].toString());*/
    }

        /*foreach (const QVariant &newsFeedUserHash, list.at(1).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedUserData = newsFeedUserHash.toHash();

            // Get (or create - though this should have already been done above) the account
            FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedUserData["id"].toLongLong());
            Q_ASSERT(account);

            account->setName(newsFeedUserData["name"].toString());
            account->setAvatar(newsFeedUserData["pic_square"].toString());
      // }
*/
    sender()->deleteLater();
}
