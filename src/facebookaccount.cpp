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
    QNetworkReply *reply = FaceBrick::instance()->networkManager()->get(request);
    connect(reply, SIGNAL(finished()), SLOT(onAvatarDownloaded()));

    // modified() is emitted when avatar download is done.
}

void FacebookAccount::onAvatarDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);

    qDebug() << "Avatar recieved for " << m_uid;
    QImage temporary = QImage::fromData(reply->readAll());

    if (temporary.width() > 50 || temporary.height() > 50) {
        // Facebook are occasionally cocks and give us an image which is too large
        temporary = temporary.scaled(QSize(50, 50), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    }

    m_avatar = QPixmap::fromImage(temporary);

    reply->deleteLater();
    emit modified();
}
