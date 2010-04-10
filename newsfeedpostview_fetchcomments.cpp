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
#include <QMaemo5InformationBox>
#endif

#include "fbrequest.h"
#include "fberror.h"

#include "mainwindow.h"
#include "facebookaccount.h"
#include "facebookaccountmodel.h"
#include "newsfeedpost.h"
#include "newsfeedmodel.h"
#include "newsfeedpostview.h"
#include "ui_newsfeedpostview.h"

void NewsFeedPostView::fetchComments()
{
    if (m_fetchingComments)
        return;

    m_fetchingComments = true;

    FBRequest* request = FBRequest::request();
    Dictionary params;

    // Query to fetch comments
    QString queryOne = "SELECT post_id, fromid, time, text FROM comment WHERE post_id='" + m_post->id() + "'";

    if (m_post->commentsModel()->newestCreatedTime() != 0)
        queryOne += " AND time > " + QString::number(m_post->commentsModel()->newestCreatedTime());

    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT fromid FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    qDebug() << "NewsFeedPostView::setPost: Sending " << fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(commentsLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(commentsLoadError(FBError)));
    request->call("facebook.fql.multiquery",params);
}

void NewsFeedPostView::commentsLoaded(const QVariant &container)
{
    m_fetchingComments = false;

    bool scrollToBottom = m_post->commentsModel()->rowCount(QModelIndex()) != 0;

    if (m_ui->commentsListView->model() != m_post->commentsModel()) {
        // HACK: this was previously in setupUi(), but if it's there we get fucked comments on Maemo 5 for reasons I have yet to understand.
        m_ui->commentsListView->setModel(m_post->commentsModel());
    }

    qDebug() << "Comments loaded: " << container;

    if (container.type() == QVariant::List) {
        QVariantList list = container.toList();

        //foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
        //    QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();
        foreach (const QVariant &commentHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> commentData = commentHash.toHash();
            qDebug() << commentData;

            FacebookAccount *author = FacebookAccountModel::instance()->account(commentData["fromid"].toULongLong());
            Q_ASSERT(author);

            NewsFeedPost *comment = new NewsFeedPost(m_post, author, QLatin1String(""),
                                                     commentData["time"].toLongLong(),
                                                     QLatin1String(""),
                                                     commentData["text"].toString());
            m_post->commentsModel()->insertNewsItem(comment);
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

    qDebug() << "NewsFeedPostView::commentsLoaded: Total comments count is " << m_post->commentsModel()->rowCount(QModelIndex());
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif

    if (scrollToBottom) {
        m_ui->commentsListView->scrollToBottom();
    }

    sender()->deleteLater();
}

void NewsFeedPostView::commentsLoadError(const FBError &error)
{
    m_fetchingComments = false;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error loading newsfeed: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    MainWindow *mw = qobject_cast<MainWindow *>(parent());
    mw->requestFailedWithFacebookError(error, true);
}
