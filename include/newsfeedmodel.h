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

#ifndef NEWSFEEDMODEL_H
#define NEWSFEEDMODEL_H

#include <QAbstractListModel>

#include "fbconnectglobal.h"

class NewsFeedPost;
class FacebookAccount;

class NewsFeedModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit NewsFeedModel(QObject *parent);
    virtual ~NewsFeedModel();

    enum Roles
    {
        UrlRole = Qt::UserRole,
        NameRole,
        PostRole,
        TimeRole,
    };

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    void insertNewsItem(NewsFeedPost * const newsItem);
    long long newestCreatedTime() const;
private slots:
    void onChildModified();
private:
    QList<NewsFeedPost *> m_posts;
};

#endif // NEWSFEEDMODEL_H
