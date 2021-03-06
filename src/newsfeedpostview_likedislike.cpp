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
#include "newsfeedpost.h"
#include "newsfeedpostview.h"

void NewsFeedPostView::iDislikeThis()
{
    if (m_doingLikeDislike)
        return;

    m_doingLikeDislike = true;
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

    FBRequest* request = FBRequest::request();
    Dictionary params;

    params["post_id"] = m_post->id();

    qDebug() << "NewsFeedPostView::iDislikeThis: Sending request";

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(dislikeDone(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(dislikeError(FBError)));
    request->call("facebook.Stream.removeLike",params);
}

void NewsFeedPostView::iLikeThis()
{
    if (m_doingLikeDislike)
        return;

    m_doingLikeDislike = true;
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

    FBRequest* request = FBRequest::request();
    Dictionary params;

    params["post_id"] = m_post->id();

    qDebug() << "NewsFeedPostView::iLikeThis: Sending request";
    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(likeDone(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(likeError(FBError)));
    request->call("facebook.Stream.addLike",params);
}

void NewsFeedPostView::likeDone(const QVariant &container)
{
    qDebug() << "like done";
    Q_UNUSED(container);
    m_doingLikeDislike = false;
    m_post->setILikeThis(true);
    setupUi();

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("You liked this!"));
#endif

    sender()->deleteLater();
}

void NewsFeedPostView::dislikeDone(const QVariant &container)
{
    qDebug() << "dislike done";
    Q_UNUSED(container);
    m_doingLikeDislike = false;
    m_post->setILikeThis(false);
    setupUi();

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("You unliked this"));
#endif

    sender()->deleteLater();
}

void NewsFeedPostView::dislikeError(const FBError &error)
{
    m_doingLikeDislike = false;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error disliking post: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    MainWindow *mw = qobject_cast<MainWindow *>(parent());
    mw->requestFailedWithFacebookError(error, true);
}

void NewsFeedPostView::likeError(const FBError &error)
{
    m_doingLikeDislike = false;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error liking post: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    MainWindow *mw = qobject_cast<MainWindow *>(parent());
    mw->requestFailedWithFacebookError(error, true);
}
