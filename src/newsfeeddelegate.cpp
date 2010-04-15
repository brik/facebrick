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
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QTextLayout>

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"

const int avatarWidth = 50;
const int avatarHeight = 50;
const int verticalPadding = 10;
const int horizontalPadding = 20;

NewsFeedDelegate::NewsFeedDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_textCache.setMaxCost(5);
    m_timeCache.setMaxCost(10);
    m_nameCache.setMaxCost(10);
}

NewsFeedDelegate::~NewsFeedDelegate()
{
    m_textCache.clear();
    m_timeCache.clear();
    m_nameCache.clear();
}

QSize NewsFeedDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize imageSize = index.data(Qt::DecorationRole).value<QPixmap>().size();

    if (m_delegateSize.width() != option.rect.size().width()) {
        // Clear layout cache; the view has changed size.
        m_textCache.clear();
        m_timeCache.clear();
        m_nameCache.clear();
        m_delegateSize = option.rect.size();
    }

    // Layout name and newsfeed text
    QTextLayout *layoutName = getNameTextLayout(index.data(NewsFeedModel::NameRole).toString(), option);
    QTextLayout *layoutText = this->getStoryTextLayout(index.data((Qt::DisplayRole)).toString(), option);

    // Max height is both bits of text added
    QRectF nameRect = layoutName->boundingRect();
    QRectF textRect = layoutText->boundingRect();
    int height = nameRect.height() + textRect.height();

    QSize s(imageSize.width() + qMax(nameRect.width(), textRect.width()) + horizontalPadding,
                 qMax(height, avatarHeight + verticalPadding));

#ifdef Q_WS_MAEMO_5
    if (QApplication::style()->inherits("QMaemo5Style")) {
        // QMaemo5Style is *really* slow at drawing listview items != 70px tall.
        // We partially get around this here by forcing smaller items to become bigger -
        // this looks no visually different on Maemo but gives us a big performance gain.
        if (s.height() < 70)
            s.setHeight(70);
    }
#endif

    return s;
}

void NewsFeedDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QTextLayout *layoutName = getNameTextLayout(index.data(NewsFeedModel::NameRole).toString(), option);
    QTextLayout *layoutText = getStoryTextLayout(index.data((Qt::DisplayRole)).toString(), option);
    QTextLayout *layoutTime = getTimeTextLayout(index.data(NewsFeedModel::TimeRole).toString(), option);

    bool drawPrimitive = true;
    if (!(option.state & QStyle::State_Selected)) {
#ifdef Q_WS_MAEMO_5
        if (QApplication::style()->inherits("QMaemo5Style")) {
            // QMaemo5Style is *really* slow at drawing listview items != 70px tall.
            // We partially get around this here by drawing unselected state ourselves.
            // This isn't exactly best practice, but hey, it works.
            drawPrimitive = false;
            painter->save();
            painter->setPen(QColor(0x00, 0x20, 0x35));
            painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
            painter->restore();
        }
#endif
    }

    if (drawPrimitive)
        QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    // Fetch image
    QPixmap img = index.data(Qt::DecorationRole).value<QPixmap>();

    // Draw 10px from left
    painter->drawPixmap(option.rect.left() + (horizontalPadding / 2), option.rect.top() + (verticalPadding / 2), img);

    // Draw name role, saving offset rect for later reuse
    layoutName->draw(painter, QPointF(option.rect.left(), option.rect.top()));

    // Draw time on the right
    painter->save();
    painter->setPen(Qt::gray);
    layoutTime->draw(painter, QPointF(option.rect.left(), option.rect.top()));
    painter->restore();

    // Move message below name, using calculated offset
    layoutText->draw(painter, QPointF(option.rect.left(), option.rect.top() + layoutName->boundingRect().height() /* TODO: space between lines? */));
}

/** run in fear before you modify any further **/

QTextLayout *NewsFeedDelegate::getNameTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_nameCache.object(text))) {
        return layout;
    }

    layout = new QTextLayout(text);
    QFont font = option.font;
    font.setBold(true);
    layout->setFont(font);
    layoutText(layout, option.rect, (avatarWidth + horizontalPadding));

    m_nameCache.insert(text, layout);

    return layout;
}

QTextLayout *NewsFeedDelegate::getStoryTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_textCache.object(text))) {
        return layout;
    }

    QString mangledText = text;
    mangledText.replace('\n', QChar::LineSeparator);
    layout = new QTextLayout(mangledText);
    layoutText(layout, option.rect, (avatarWidth + horizontalPadding));

    m_textCache.insert(text, layout);

    return layout;
}

QTextLayout *NewsFeedDelegate::getTimeTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_timeCache.object(text))) {
        return layout;
    }

    layout = new QTextLayout(text);
    QFont font = option.font;
    QTextOption opt = layout->textOption();
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout->setTextOption(opt);
    layoutText(layout, option.rect, 0);

    m_timeCache.insert(text, layout);

    return layout;
}

void NewsFeedDelegate::layoutText(QTextLayout *layout, const QRect &rect, int xoffset) const
{
    // Now lay it out
    int height = 0;

    layout->beginLayout();

    while (1) {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(rect.width() - xoffset);
        line.setPosition(QPointF(xoffset, height));
        height += line.height();
    }

    // phew.
    layout->endLayout();
}
