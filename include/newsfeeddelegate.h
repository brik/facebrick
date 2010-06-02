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
    Q_OBJECT
public:
    NewsFeedDelegate(QObject *parent);
    virtual ~NewsFeedDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    mutable QCache<QString, QTextLayout> m_postTextCache;
    mutable QCache<QString, QTextLayout> m_postAuthorCache;
    mutable QCache<QString, QTextLayout> m_postTimeCache;
    mutable QCache<QString, QTextLayout> m_postAttachmentUrlCache;
    mutable QCache<QString, QTextLayout> m_postAttachmentDescriptionCache;
    mutable QSize m_delegateSize;

    QTextLayout *getNameTextLayout(const QString &text, const QStyleOptionViewItem &option) const;
    QTextLayout *getStoryTextLayout(const QString &text, const QStyleOptionViewItem &option) const;
    QTextLayout *getTimeTextLayout(const QString &text, const QStyleOptionViewItem &option) const;
    QTextLayout *getAttachmentDescriptionTextLayout(const QString &text, const QStyleOptionViewItem &option) const;
    QTextLayout *getAttachmentNameTextLayout(const QString &text, const QStyleOptionViewItem &option) const;
    void layoutText(QTextLayout *layout, const QRect &rect, int xoffset) const;
};

#endif // NEWSFEEDDELEGATE_H
