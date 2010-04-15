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

#include <QNetworkReply>
#include <QDebug>

#include "newsfeedpost.h"
#include "facebrick.h"

NewsFeedPost::MediaType NewsFeedPost::type() const
{
    return m_type;
}

void NewsFeedPost::setType(MediaType type)
{
    m_type = type;
    emit modified();
}

const QString &NewsFeedPost::description() const
{
    return m_description;
}

void NewsFeedPost::setDescription(const QString &description)
{
    m_description = description;
    emit modified();
}

const QUrl &NewsFeedPost::attachmentUrl() const
{
    return m_attachmentUrl;
}

void NewsFeedPost::setAttachmentUrl(const QUrl &url)
{
    m_attachmentUrl = url;
    emit modified();
}

const QPixmap &NewsFeedPost::thumbnail() const
{
    return m_thumbnail;
}

void NewsFeedPost::setThumbnail(const QUrl &url)
{
    if (m_thumbnailUrl == url.toString()) {
        // Don't bother with a network request to fetch the same file
        return;
    }

    m_thumbnailUrl = url.toString();
    QNetworkRequest request(url);
    QNetworkReply *reply = FaceBrick::networkManager()->get(request);
    connect(reply, SIGNAL(finished()), SLOT(onAvatarDownloaded()));

    // modified() is emitted when avatar download is done.
}

void NewsFeedPost::onThumbnailDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);

    QImage temporary = QImage::fromData(reply->readAll());

    if (temporary.width() > 50 || temporary.height() > 50) {
        // Facebook are occasionally cocks and give us an image which is too large
        temporary = temporary.scaled(QSize(50, 50), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    }

    m_thumbnail = QPixmap::fromImage(temporary);

    reply->deleteLater();
    emit modified();
}
