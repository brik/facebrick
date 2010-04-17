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

bool NewsFeedPost::hasAttachment() const
{
    return m_hasAttachment;
}

void NewsFeedPost::setHasAttachment(bool hasAttachment)
{
    m_hasAttachment = hasAttachment;
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

const QString &NewsFeedPost::attachmentName() const
{
    return m_attachmentName;
}

void NewsFeedPost::setAttachmentName(const QString &name)
{
    m_attachmentName = name;
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

    qDebug() << "Downloading thumbnail for post at " << url.toString();

    m_thumbnailUrl = url.toString();
    QNetworkRequest request(url);
    QNetworkReply *reply = FaceBrick::networkManager()->get(request);
    connect(reply, SIGNAL(finished()), SLOT(onThumbnailDownloaded()));

    // modified() is emitted when avatar download is done.
}

void NewsFeedPost::onThumbnailDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);

    qDebug() << "Downloaded thumbnail at " << m_thumbnailUrl.toString();

    QImage temporary = QImage::fromData(reply->readAll());

    if (temporary.width() > 50 || temporary.height() > 50) {
        // Facebook are occasionally cocks and give us an image which is too large
        temporary = temporary.scaled(QSize(50, 50), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    }

    m_thumbnail = QPixmap::fromImage(temporary);

    reply->deleteLater();
    emit modified();
}
