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

#include <QStyledItemDelegate>
#include <QImage>
#include <QPainter>
#include <QDebug>

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"

NewsFeedDelegate::NewsFeedDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize NewsFeedDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize imageSize = index.data(Qt::DecorationRole).value<QImage>().size();

    // Left offset of text is imagesize + total padding
    QRect textRect = option.rect.adjusted(imageSize.width() + 20, 0, 0, 0);

    // Bounding size for name, in bold, at the top
    QSize fontSize = option.fontMetrics.boundingRect(textRect, Qt::TextWordWrap, index.data(NewsFeedModel::NameRole).toString()).size();

    // Bounding size for message, at the bottom
    QSize fontSizeTwo = option.fontMetrics.boundingRect(textRect, Qt::TextWordWrap, index.data(Qt::DisplayRole).toString()).size();

    // Max height is both bits of text added
    int height = fontSize.height() + fontSizeTwo.height();

    return QSize(imageSize.width() + qMax(fontSize.width(), fontSizeTwo.width()) + 20, qMax(height, 60));
}

void NewsFeedDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Fetch image
    QImage img = index.data(Qt::DecorationRole).value<QImage>();

    // Draw 10px from left
    painter->drawImage(option.rect.left() + 10, option.rect.top() + 5, img);

    // Adjust text to push further right, leave 10px padding either side of the image, plus image width
    QRect textRect = option.rect.adjusted(20 + img.width(), 0, 0, 0);

    // Set bold
    painter->save();
    QFont f = painter->font();
    f.setBold(true);
    painter->setFont(f);

    // Calculate offset for name role
    QRect nameTextMetrics;

    // Draw name role, saving offset rect for later reuse
    painter->drawText(textRect, Qt::TextWordWrap | Qt::AlignTop, index.data(NewsFeedModel::NameRole).toString(), &nameTextMetrics);

    // Reset bold
    painter->restore();

    // Move message below name, using calculated offset
    textRect.adjust(0, nameTextMetrics.height(), 0, 0);
    painter->drawText(textRect, Qt::TextWordWrap | Qt::AlignTop, index.data(Qt::DisplayRole).toString());
}
