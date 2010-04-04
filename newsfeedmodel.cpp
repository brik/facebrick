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

#include "fbconnectglobal.h"

#include "newsfeedmodel.h"
#include "newsfeedpost.h"
#include "facebookaccount.h"

NewsFeedModel::NewsFeedModel(QObject *parent) : QAbstractListModel(parent)
{
}

NewsFeedModel::~NewsFeedModel()
{
    foreach (NewsFeedPost *post, m_posts) {
        delete post;
    }
}

int NewsFeedModel::rowCount(const QModelIndex&) const
{
    return m_posts.count();
}

QVariant NewsFeedModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_posts.length())
        return QVariant();

    NewsFeedPost *np = m_posts[index.row()];
QSize sz ;
    switch (role) {
    case Qt::DisplayRole:
        return np->message();
        break;
    case Qt::DecorationRole:
        return np->author()->avatar();
        break;
    case NewsFeedModel::NameRole:
        return np->author()->name();
        break;
    case NewsFeedModel::UrlRole:
        return np->url();
        break;
    }

    return QVariant();
}

void NewsFeedModel::appendNewsItem(NewsFeedPost * const newsItem)
{
    beginInsertRows(QModelIndex(), m_posts.length(), m_posts.length());
    connect(newsItem, SIGNAL(modified()), this, SLOT(onChildModified()));
    m_posts.append(newsItem);
    endInsertRows();
}

void NewsFeedModel::prependNewsItem(NewsFeedPost * const newsItem)
{
    beginInsertRows(QModelIndex(), 0, 0);
    connect(newsItem, SIGNAL(modified()), this, SLOT(onChildModified()));
    m_posts.prepend(newsItem);
    endInsertRows();
}

void NewsFeedModel::onChildModified()
{
    // Find the child that was modified
    NewsFeedPost *np = qobject_cast<NewsFeedPost *>(sender());
    Q_ASSERT(np);

    qDebug("onChildModified for %p", np);
    for (int i = 0; i < m_posts.count(); ++i) {
        if (np == m_posts[i]) {
            dataChanged(createIndex(i, 0), createIndex(i, 0));
            return;
        }
    }
}

long long NewsFeedModel::newestCreatedTime() const
{
    if (m_posts.count() == 0)
        return 0;

    return m_posts.at(0)->createdTime();
}
