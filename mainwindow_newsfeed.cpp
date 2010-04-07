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

#include <QDesktopServices>
#include <QDebug>
#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "fbrequest.h"
#include "fbsession.h"
#include "fberror.h"

#include "facebookaccount.h"
#include "facebookaccountmodel.h"
#include "newsfeedpost.h"
#include "newsfeedmodel.h"
#include "mainwindow.h"

void MainWindow::newsFeedListClicked(QModelIndex index)
{
    QDesktopServices::openUrl(QUrl(m_newsFeedModel->data(index, NewsFeedModel::UrlRole).toString()));
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

    FBRequest* request = FBRequest::request();
    Dictionary params;

    // Query to fetch news posts
    QString queryOne = "SELECT post_id, actor_id, target_id, message, permalink, created_time FROM stream WHERE filter_key in (SELECT filter_key FROM stream_filter WHERE uid=" + QString::number(this->m_fbSession->uid()) + " AND type='newsfeed') AND is_hidden = 0";

    if (m_lastUpdatedNewsFeed != 0) {
        // Fetch all posts newer than the ones we have now
        queryOne += " AND created_time > " + QString::number(m_lastUpdatedNewsFeed);
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

    if (container.type() == QVariant::List) {
        QVariantList list = container.toList();

        // Item #0 will be our result set on news items
        foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();

            // Fetch (or create) the account that made this newsfeed post
            FacebookAccount *account = m_facebookAccountModel->account(newsFeedPostData["actor_id"].toLongLong());
            Q_ASSERT(account);

            // Create a new newsfeed post
            NewsFeedPost *np = new NewsFeedPost(m_newsFeedModel,
                                                account,
                                                newsFeedPostData["created_time"].toLongLong(),
                                                newsFeedPostData["permalink"].toString(),
                                                newsFeedPostData["message"].toString());
            Q_ASSERT(np);

            // Seed it into the model
            m_newsFeedModel->insertNewsItem(np);

            // Update our 'recent posts' block badger.
            if (np->createdTime() > m_lastUpdatedNewsFeed)
                m_lastUpdatedNewsFeed = np->createdTime();
        }

        foreach (const QVariant &newsFeedUserHash, list.at(1).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedUserData = newsFeedUserHash.toHash();

            // Get (or create - though this should have already been done above) the account
            FacebookAccount *account = m_facebookAccountModel->account(newsFeedUserData["id"].toLongLong());
            Q_ASSERT(account);

            account->setName(newsFeedUserData["name"].toString());
            account->setAvatar(newsFeedUserData["pic_square"].toString());
        }
    }
}
