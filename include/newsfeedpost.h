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

#ifndef NEWSFEEDPOST_H
#define NEWSFEEDPOST_H

#include <QObject>
#include <QPixmap>
#include <QUrl>

#include "fbconnectglobal.h"

class FacebookAccount;
class NewsFeedModel;

class NewsFeedPost : public QObject
{
Q_OBJECT
public:
    explicit NewsFeedPost(QObject *parent, FacebookAccount *account, const QString &postId,
                          long long createdTime, const QString &url, const QString &message);

    const QString &url() const;
    const QString &message() const;
    FacebookAccount *author() const;
    long long createdTime() const;
    QString timeAsString() const;
    const QString &id() const;
    NewsFeedModel *commentsModel() const;
    void setILikeThis(bool iLikeThis);
    bool iLikeThis() const;

    // Attachment stuff
    bool hasAttachment() const;
    void setHasAttachment(bool hasAttachment);
    const QString &description() const;
    void setDescription(const QString &description);
    const QString &attachmentName() const;
    void setAttachmentName(const QString &name);
    const QPixmap &thumbnail() const;
    void setThumbnail(const QUrl &url);
signals:
    void modified();
private slots:
    void onThumbnailDownloaded();
private:
    long long m_createdTime;
    FacebookAccount *m_account;
    QString m_url;
    QString m_message;
    QString m_id;
    bool m_iLikeThis;
    NewsFeedModel *m_commentsModel;

    // Attachment stuff
    bool m_hasAttachment;
    QString m_description;
    QString m_attachmentName;
    QUrl m_thumbnailUrl;
    QPixmap m_thumbnail;
};

#endif // NEWSFEEDPOST_H
