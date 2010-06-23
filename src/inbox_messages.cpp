/*
 * Copyright (C) 2010 Kamilla Bremeraunet <kamilla@bremeraunet.com>
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
#include "newsfeedmodel.h"
#include "inbox.h"
#include "ui_mainwindow.h"

void Inbox::fetchMessages()
{
    // Lock
    if (m_updatingInbox) {
        qDebug() << "fetchInbox: Already updating...";
        return;
    }

    m_updatingInbox = true;
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

    FBRequest* request = FBRequest::request();
    Dictionary params;

    // Query to fetch news posts
    QString queryOne = "SELECT post_id, actor_id, target_id, message, permalink, created_time, likes, attachment FROM stream WHERE filter_key in (SELECT filter_key FROM stream_filter WHERE uid=" + QString::number(FaceBrick::instance()->session()->uid()) + " AND type='newsfeed') AND is_hidden = 0";

    if (m_lastUpdatedInbox != 0) {
        // Fetch all posts newer than the ones we have now
        queryOne += " AND created_time > " + QString::number(m_lastUpdatedInbox);
    }

    // Fetch all people that made these posts, combine them into a single FQL multiquery
    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT actor_id FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    qDebug() << "fetchNewsFeed: Sending " << fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(newsFeedLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(newsFeedLoadingError(FBError)));
    request->call("facebook.fql.multiquery",params);
}

/*void Inbox::newsFeedLoadingError(const FBError &error)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error loading newsfeed: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    m_updatingNewsFeed = false;
    requestFailedWithFacebookError(error, true);
}

void Inbox::newsFeedLoaded(const QVariant &container)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
    m_updatingNewsFeed = false;

    if (container.type() == QVariant::List) {
        QVariantList list = container.toList();

        // Item #0 will be our result set on news items
        foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();

            // Fetch (or create) the account that made this newsfeed post
            FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedPostData["actor_id"].toLongLong());
            Q_ASSERT(account);

            // Create a new newsfeed post
            NewsFeedPost *np = new NewsFeedPost(m_newsFeedModel,
                                                account,
                                                newsFeedPostData["post_id"].toString(),
                                                newsFeedPostData["created_time"].toLongLong(),
                                                newsFeedPostData["permalink"].toString(),
                                                newsFeedPostData["message"].toString());
            Q_ASSERT(np);

            // Seed it into the model
            m_newsFeedModel->insertNewsItem(np);

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
            np->setThumbnail(mediaInfo["src"].toString());
        }

        foreach (const QVariant &newsFeedUserHash, list.at(1).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedUserData = newsFeedUserHash.toHash();

            // Get (or create - though this should have already been done above) the account
            FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedUserData["id"].toLongLong());
            Q_ASSERT(account);

            account->setName(newsFeedUserData["name"].toString());
            account->setAvatar(newsFeedUserData["pic_square"].toString());
        }
    }

    sender()->deleteLater();
}*/
