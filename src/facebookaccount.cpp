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

#include <QUrl>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "fbconnectglobal.h"

#include "facebookaccount.h"
#include "facebrick.h"
#include "qfacebook.h"

FacebookAccount::FacebookAccount(QObject *parent, FBUID uid)
    : QObject(parent),
    m_uid(uid)
{
}

FBUID FacebookAccount::uid() const
{
    return m_uid;
}

const QString &FacebookAccount::name() const
{
    return m_name;
}

void FacebookAccount::setName(const QString &name)
{
    m_name = name;
    emit modified();
}

const QPixmap &FacebookAccount::avatar() const
{
    return m_avatar;
}

void FacebookAccount::setAvatar(const QUrl &url)
{
    if (m_avatarUrl == url.toString()) {
        // Don't bother with a network request to fetch the same file
        return;
    }

    m_avatarUrl = url.toString();
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QNetworkReply *reply = FaceBrick::instance()->m_facebook->get(url);
    connect(reply, SIGNAL(finished()), SLOT(onAvatarDownloaded()));

    // modified() is emitted when avatar download is done.
}

void FacebookAccount::onAvatarDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);

    QByteArray data;
    QUrl redir;
    if (!reply) {
        qDebug() << "cast fail";
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Picture get fail!";
        reply->deleteLater();
        reply = 0;
        return;
    }

    qDebug() << reply;
    redir = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    qDebug() << redir;
    // Facebook is redirecting the picture request
    if(!redir.isEmpty()) {
        qDebug() << "Redirecting picture request to " + redir.toString();
        setAvatar(redir);
        // not finished yet!
        return;
    }

    QImage temporary = QImage::fromData(reply->readAll());

    if (temporary.width() > 50 || temporary.height() > 50) {
        // Facebook are occasionally cocks and give us an image which is too large
        temporary = temporary.scaled(QSize(50, 50), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    }

    m_avatar = QPixmap::fromImage(temporary);

    reply->deleteLater();
    emit modified();

    /*QByteArray data;
    QUrl redir;
    if (!reply) {
        qDebug() << "Cast to qnetwork reply fail!";
        goto onfinished;
    }
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Picture get fail!";
        reply->deleteLater();
        reply = 0;

        goto onfinished;
    }

    redir = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    // Facebook is redirecting the picture request
    if(!redir.isEmpty()) {
        qDebug() << "Redirecting picture request to " + redir.toString();
        reply = qfaceBook->get(redir);
        if (reply)
            connect(reply, SIGNAL(finished()), this, SLOT(onPictureReady()));
        // not finished yet!
        return;
    }

    qDebug() << "######### Parsed picture:";
    data = reply->readAll();
    m_picture = QImage::fromData(data, "JPEG");

    onfinished:
    if (m_isLoaded)
        emit finished();
    m_isLoaded = true;*/
}
