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

#include "newsfeedpost.h"
#include "mainwindow.h"
#include "newsfeedpostview.h"
#include "ui_newsfeedpostview.h"

void NewsFeedPostView::sendComment()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
    m_ui->commentButton->setEnabled(false);
    m_ui->commentText->setEnabled(false);

    qDebug() << "Sending comment to " << m_post->id();

    FBRequest* request = FBRequest::request();
    Dictionary params;

    params["post_id"] = m_post->id();
    params["comment"] = m_ui->commentText->text();

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(commentAdded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(commentAddError(FBError)));
    request->call("facebook.Stream.addComment",params);
}

void NewsFeedPostView::commentAdded(const QVariant &container)
{
    Q_UNUSED(container);

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
    m_ui->commentText->setText(QLatin1String(""));

    m_ui->commentButton->setEnabled(true);
    m_ui->commentText->setEnabled(true);
    fetchComments();

    sender()->deleteLater();
}

void NewsFeedPostView::commentAddError(const FBError &error)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error adding comment: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    MainWindow *mw = qobject_cast<MainWindow *>(parent());
    mw->requestFailedWithFacebookError(error, true);
    m_ui->commentButton->setEnabled(true);
    m_ui->commentText->setEnabled(true);
}
