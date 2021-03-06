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

#ifndef FACEBOOKACCOUNT_H
#define FACEBOOKACCOUNT_H

#include <QObject>
#include <QUrl>
#include <QPixmap>
#include <QNetworkAccessManager>

#include "fbconnectglobal.h"

class QNetworkReply;

class FacebookAccount : public QObject
{
Q_OBJECT
public:
    FacebookAccount(QObject *parent, FBUID uid);

    FBUID uid() const;
    const QString &name() const;
    void setName(const QString &name);
    const QPixmap &avatar() const;
    void setAvatar(const QUrl &url);
signals:
    void modified();
private slots:
    void onAvatarDownloaded();
private:
    FBUID m_uid;
    QString m_name;
    QString m_avatarUrl;
    QPixmap m_avatar;
};

#endif // FACEBOOKACCOUNT_H
