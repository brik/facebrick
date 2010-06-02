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
#include "keyhandler.h"

NewsFeedModel::NewsFeedModel(QObject *parent, bool newestAtTop) :
    QAbstractListModel(parent),
    m_newestAtTop(newestAtTop)
{
    connect(KeyHandler::instance(),  SIGNAL(decreaseKeyPressed()), SLOT(fontSizeChanged()));
    connect(KeyHandler::instance(),  SIGNAL(increaseKeyPressed()), SLOT(fontSizeChanged()));
}

NewsFeedModel::~NewsFeedModel()
{
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
    case NewsFeedModel::PostRole:
        return QVariant::fromValue<void*>(np);
        break;
    case NewsFeedModel::TimeRole:
        return np->timeAsString();
        break;
    }

    return QVariant();
}

// NOTE: DO NOT TAKE OWNERSHIP of newsItem.
// a newsItem might be in many models!
void NewsFeedModel::insertNewsItem(NewsFeedPost *const newsItem)
{
    int i = 0;

    // Find the correct place to insert it
    if (m_newestAtTop) {
        for (; i < m_posts.count(); ++i) {
            if (newsItem->createdTime() > m_posts.at(i)->createdTime())
                break;
        }
    } else {
        // TODO: this is sorting the wrong way now we append newest. could optimize.
        for (; i < m_posts.count(); ++i) {
            if (newsItem->createdTime() < m_posts.at(i)->createdTime())
                break;
        }
    }

    connect(newsItem, SIGNAL(modified()), SLOT(onChildModified()));

    // Insert it to the given index
    beginInsertRows(QModelIndex(), i, i);
    m_posts.insert(i, newsItem);
    endInsertRows();
}

void NewsFeedModel::onChildModified()
{
    // Find the child that was modified
    NewsFeedPost *np = qobject_cast<NewsFeedPost *>(sender());
    Q_ASSERT(np);

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

    if (m_newestAtTop)
        return m_posts.at(0)->createdTime();
    else
        return m_posts.at(m_posts.count() - 1)->createdTime();
}

void NewsFeedModel::fontSizeChanged()
{
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}
