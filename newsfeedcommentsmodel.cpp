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

#include "facebookaccount.h"
#include "newsfeedcomment.h"
#include "newsfeedcommentsmodel.h"

NewsFeedCommentsModel::NewsFeedCommentsModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

NewsFeedCommentsModel::~NewsFeedCommentsModel()
{

}

int NewsFeedCommentsModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return m_comments.count();
}

QVariant NewsFeedCommentsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    FacebookAccount *author = m_comments.at(index.row())->author();

    switch (role)
    {
    case Qt::DecorationRole:
            return author->avatar();
    case Qt::DisplayRole:
            return m_comments.at(index.row())->text();
    }

    return QVariant();
}

void NewsFeedCommentsModel::insertComment(NewsFeedComment * const comment)
{
    int i = 0;

    // Find the correct place to insert it
    for (; i < m_comments.count(); ++i) {
        if (comment->createdTime() > m_comments.at(i)->createdTime())
            break;
    }

    connect(comment, SIGNAL(modified()), SLOT(onChildModified()));

    // Insert it to the given index
    beginInsertRows(QModelIndex(), i, i);
    m_comments.insert(i, comment);
    endInsertRows();
}

long long NewsFeedCommentsModel::newestCreatedTime() const
{
    if (m_comments.count() == 0)
        return 0;

    return m_comments.at(0)->createdTime();
}

void NewsFeedCommentsModel::onChildModified()
{
    // Find the child that was modified
    NewsFeedComment *nc = qobject_cast<NewsFeedComment *>(sender());
    Q_ASSERT(nc);

    for (int i = 0; i < m_comments.count(); ++i) {
        if (nc == m_comments[i]) {
            dataChanged(createIndex(i, 0), createIndex(i, 0));
            return;
        }
    }
}
