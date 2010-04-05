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

#ifndef NEWSFEEDDELEGATE_H
#define NEWSFEEDDELEGATE_H

#include <QStyledItemDelegate>
#include <QCache>

class QTextLayout;

class NewsFeedDelegate : public QStyledItemDelegate
{
public:
    NewsFeedDelegate(QObject *parent);
    virtual ~NewsFeedDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    mutable QCache<QString, QTextLayout> m_laidOutText;
    mutable QSize m_delegateSize;

    QTextLayout *getTextLayout(const QString &text, const QStyleOptionViewItem &option, bool &created, bool requiresBold) const;
    void insertLayoutIntoCache(const QString &text, QTextLayout *layout) const;
};

#endif // NEWSFEEDDELEGATE_H
