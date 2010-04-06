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
#include <QApplication>
#include <QTextLayout>

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"

NewsFeedDelegate::NewsFeedDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

NewsFeedDelegate::~NewsFeedDelegate()
{
    m_laidOutText.clear();
    m_laidOutText.setMaxCost(40);
}

QSize NewsFeedDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize imageSize = index.data(Qt::DecorationRole).value<QImage>().size();

    if (m_delegateSize.width() != option.rect.size().width()) {
        // Clear layout cache; the view has changed size.
        m_laidOutText.clear();
        m_delegateSize = option.rect.size();
    }

    // Layout name and newsfeed text
    bool layoutNameRequiresInsert = false;
    bool layoutTextRequiresInsert = false;
    QTextLayout *layoutName = getTextLayout(index.data(NewsFeedModel::NameRole).toString(), option, layoutNameRequiresInsert, true);
    QTextLayout *layoutText = getTextLayout(index.data((Qt::DisplayRole)).toString(), option, layoutTextRequiresInsert, false);

    // Max height is both bits of text added
    int height = layoutName->boundingRect().height() + layoutText->boundingRect().height();

    QSize s(imageSize.width() + qMax(layoutName->boundingRect().width(), layoutText->boundingRect().width()) + 20,
                 qMax(height, 60));

    // done with items - cache them. don't use them after this as they might go away suddenly!
    if (layoutNameRequiresInsert)
        insertLayoutIntoCache(layoutName->text(), layoutName);
    if (layoutTextRequiresInsert)
        insertLayoutIntoCache(layoutText->text(), layoutText);

    return s;
}

void NewsFeedDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    bool layoutNameRequiresInsert = false;
    bool layoutTextRequiresInsert = false;
    QTextLayout *layoutName = getTextLayout(index.data(NewsFeedModel::NameRole).toString(), option, layoutNameRequiresInsert, true);
    QTextLayout *layoutText = getTextLayout(index.data((Qt::DisplayRole)).toString(), option, layoutTextRequiresInsert, false);

    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    // Fetch image
    QImage img = index.data(Qt::DecorationRole).value<QImage>();

    // Draw 10px from left
    painter->drawImage(option.rect.left() + 10, option.rect.top() + 5, img);

    // Draw name role, saving offset rect for later reuse
    layoutName->draw(painter, QPointF(option.rect.left(), option.rect.top()));

    // Move message below name, using calculated offset
    layoutText->draw(painter, QPointF(option.rect.left(), option.rect.top() + layoutName->boundingRect().height() /* TODO: space between lines? */));

    // done with items - cache them. don't use them after this as they might go away suddenly!
    if (layoutNameRequiresInsert)
        insertLayoutIntoCache(layoutName->text(), layoutName);
    if (layoutTextRequiresInsert)
        insertLayoutIntoCache(layoutText->text(), layoutText);
}

QTextLayout *NewsFeedDelegate::getTextLayout(const QString &text, const QStyleOptionViewItem &option, bool &created, bool requiresBold) const
{
    // Shortcut: return cached item
    if (QTextLayout *layout = m_laidOutText.object(text)) {
        //qDebug() << "NewsFeedDelegate::cacheAndReturn: returning hit for " << text;
        created = false;
        return layout;
    }

    //qDebug() << "NewsFeedDelegate::cacheAndReturn: laying out " << text;

    // Long way around: create our new layout
    QTextLayout *layout = new QTextLayout(text);
    QFont font = option.font;

    // TODO: technically, there is a small bug possible here.
    // if we have one piece of text created with requiresBold, added to the cache, and later on, someone else comes along and requests
    // the same text without bold, we'll (erroneously) return the cached bolded text.
    // we should probably come up with a better QCache key than the text itself.
    if (requiresBold)
        font.setBold(true);

    // Now lay it out
    int leading = QFontMetrics(font).leading();
    int height = 0;
    qreal widthUsed = 0;

    layout->setFont(font);

    layout->beginLayout();

    while (1) {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(option.rect.width() - (20 + 50) /* XXX: hardcoded 20px padding, 50px img width, BAD! */);
        height += leading;
        // 20+50 is padding+imgwidth again.. sigh
        line.setPosition(QPointF(20 + 50, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
    }

    // phew.
    layout->endLayout();

    created = true;

    return layout;
}

// note: cache insertion is handled seperately because QCache will get pissy on us and delete items that are too big
// *or* if we're inserting multiple items, and one is large, it might delete one of the earlier ones before we're done with it.
// either situation is subtle, and very ouch.
void NewsFeedDelegate::insertLayoutIntoCache(const QString &text, QTextLayout *layout) const
{
    //qDebug() << "NewsFeedDelegate::insertLayoutIntoCache: caching layout for " << text;
    m_laidOutText.insert(text, layout);
    //qDebug() << "NewsFeedDelegate::insertLayoutIntoCache: item cache now contains " << m_laidOutText.count() << " items";
}
