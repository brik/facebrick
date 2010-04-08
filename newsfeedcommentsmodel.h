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

#ifndef NEWSFEEDCOMMENTSMODEL_H
#define NEWSFEEDCOMMENTSMODEL_H

#include <QAbstractListModel>

class NewsFeedComment;

class NewsFeedCommentsModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit NewsFeedCommentsModel(QObject *parent);
    virtual ~NewsFeedCommentsModel();

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    void insertComment(NewsFeedComment * const comment);
    long long newestCreatedTime() const;
private slots:
    void onChildModified();
private:
    QList<NewsFeedComment *> m_comments;
};

#endif // NEWSFEEDCOMMENTSMODEL_H
