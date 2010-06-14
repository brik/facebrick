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
#include <QDebug>
#include <QSettings>

#include "newsfeeddelegate.h"
#include "newsfeedpost.h"
#include "facebookaccount.h"
#include "newsfeedmodel.h"
#include "keyhandler.h"

const int avatarWidth = 50;
const int avatarHeight = 50;
const int verticalAvatarPadding = 10;
const int horizontalAvatarPadding = 20;

NewsFeedDelegate::NewsFeedDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_postTextCache.setMaxCost(5);
    m_postTimeCache.setMaxCost(10);
    m_postAuthorCache.setMaxCost(10);
    m_postAttachmentUrlCache.setMaxCost(5);
    m_postAttachmentDescriptionCache.setMaxCost(5);
}

NewsFeedDelegate::~NewsFeedDelegate()
{
    m_postTextCache.clear();
    m_postTimeCache.clear();
    m_postAuthorCache.clear();
    m_postAttachmentUrlCache.clear();
    m_postAttachmentDescriptionCache.clear();
}

QSize NewsFeedDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return QSize(-1, -1);

    NewsFeedPost *np = static_cast<NewsFeedPost *>(index.data(NewsFeedModel::PostRole).value<void *>());

    if (m_delegateSize.width() != option.rect.size().width()) {
        // Clear layout cache; the view has changed size.
        m_postTextCache.clear();
        m_postTimeCache.clear();
        m_postAuthorCache.clear();
        m_postAttachmentUrlCache.clear();
        m_postAttachmentDescriptionCache.clear();
        m_delegateSize = option.rect.size();
    }

    // First: let's make sure FB avatar is accounted for.
    QSize imageSize = np->author()->avatar().size();
    imageSize.setWidth(imageSize.width() + horizontalAvatarPadding);
    imageSize.setHeight(imageSize.height() + verticalAvatarPadding);

    // Now name
    QRectF nameRect = getNameTextLayout(index.data(NewsFeedModel::NameRole).toString(), option)->boundingRect();
    QRectF textRect = getStoryTextLayout(index.data((Qt::DisplayRole)).toString(), option)->boundingRect();

    // Attachment info (if any)
    int attachmentHeight = 0;
    int attachmentWidth = 0;
    if (np->hasAttachment()) {
        // Name
        QRectF attachmentNameRect;

        if (np->attachmentName().length()) {
            attachmentNameRect = getAttachmentNameTextLayout(np->attachmentName(), option)->boundingRect();
            attachmentHeight += attachmentNameRect.height();
        }

        // Now pic/desc
        QRectF attachmentDescriptionRect = getAttachmentDescriptionTextLayout(np->description(), option)->boundingRect();
        QSize attachmentThumbSize = np->thumbnail().size();
        attachmentHeight += qMax(attachmentThumbSize.height(), (int)attachmentDescriptionRect.height());

        // Work out width
        attachmentWidth = qMax(attachmentNameRect.width(), attachmentThumbSize.width() + (horizontalAvatarPadding / 2) + attachmentDescriptionRect.width());
    }

    // Now calculate final sizes.
    int requiredHeight = qMax(imageSize.height(), (int)(nameRect.height() + textRect.height()) + attachmentHeight);
    int requiredWidth = imageSize.width() + qMax((int)nameRect.width(), (int)textRect.width());
    requiredWidth = qMax(requiredWidth, imageSize.width() + attachmentWidth);

    QSize s(requiredWidth, requiredHeight);

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

    NewsFeedPost *np = static_cast<NewsFeedPost *>(index.data(NewsFeedModel::PostRole).value<void *>());

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
    QPixmap avatarImage = index.data(Qt::DecorationRole).value<QPixmap>();

    // Draw 10px from left
    painter->drawPixmap(option.rect.left() + (horizontalAvatarPadding / 2), option.rect.top() + (verticalAvatarPadding / 2), avatarImage);

    // Draw name role, saving offset rect for later reuse
    painter->save();
    painter->setPen(QColor(37, 168, 229));
    QTextLayout *layoutName = getNameTextLayout(np->author()->name(), option);
    layoutName->draw(painter, QPointF(option.rect.left(), option.rect.top()));
    painter->restore();

    // Draw time on the right
    painter->save();
    painter->setPen(Qt::gray);
    QTextLayout *layoutTime = getTimeTextLayout(np->timeAsString(), option);
    layoutTime->draw(painter, QPointF(option.rect.left(), option.rect.top()));
    painter->restore();


    // Move message below name, using calculated offset
    QTextLayout *layoutText = getStoryTextLayout(np->message(), option);
    layoutText->draw(painter, QPointF(option.rect.left(), option.rect.top() + layoutName->boundingRect().height()));

    int descriptionY = layoutName->boundingRect().height() + layoutText->boundingRect().height();
    if (np->hasAttachment()) {
        // Now we need to render attachments. Cry!
        // first, title
        if (np->attachmentName().length()) {
            QTextLayout *attachmentName = getAttachmentNameTextLayout(np->attachmentName(), option);
            attachmentName->draw(painter, QPointF(0, option.rect.top() + descriptionY));
            descriptionY += attachmentName->boundingRect().height();
        }

        // Picture
        painter->drawPixmap(option.rect.left() + horizontalAvatarPadding + avatarImage.width(), option.rect.top() + descriptionY, np->thumbnail());

        // Description, on the right hand side
        QTextLayout *attachmentDesc = this->getAttachmentDescriptionTextLayout(np->description(), option);
        attachmentDesc->draw(painter, QPointF(np->thumbnail().width() == 0 ? -avatarWidth : 0, option.rect.top() + descriptionY));

        // Now push attachment total Y down to draw the user's message
        //descriptionY += qMax(np->thumbnail().height(), qRound(attachmentDesc->boundingRect().height()));
    }
}

/** run in fear before you modify any further **/

QTextLayout *NewsFeedDelegate::getNameTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_postAuthorCache.object(text))) {
        return layout;
    }

    layout = new QTextLayout(text);
    QFont font = option.font;
    font.setBold(true);

    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    int fontSize = settings.value("fontSize", fontSize).toInt();
    settings.endGroup();

    font.setPointSize(option.font.pointSize() + fontSize - 1);
    layout->setFont(font);
    layoutText(layout, option.rect, (avatarWidth + horizontalAvatarPadding));

    m_postAuthorCache.insert(text, layout);

    return layout;
}

QTextLayout *NewsFeedDelegate::getStoryTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_postTextCache.object(text))) {
        return layout;
    }

    QString mangledText = text;
    mangledText.replace('\n', QChar::LineSeparator);
    layout = new QTextLayout(mangledText);

    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    int fontSize = settings.value("fontSize", fontSize).toInt();
    settings.endGroup();

    QFont font = option.font;
    font.setPointSize(option.font.pointSize() + fontSize - 1);
    layout->setFont(font);
    layoutText(layout, option.rect, (avatarWidth + horizontalAvatarPadding));

    m_postTextCache.insert(text, layout);

    return layout;
}

QTextLayout *NewsFeedDelegate::getTimeTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_postTimeCache.object(text))) {
        return layout;
    }

    layout = new QTextLayout(text);

    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    int fontSize = settings.value("fontSize", fontSize).toInt();
    settings.endGroup();

    QFont font = option.font;
    font.setPointSize(option.font.pointSize() + fontSize);

    QTextOption opt = layout->textOption();
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout->setTextOption(opt);
    layout->setFont(font);
    layoutText(layout, option.rect, 0);

    m_postTimeCache.insert(text, layout);

    return layout;
}

QTextLayout *NewsFeedDelegate::getAttachmentNameTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_postAttachmentUrlCache.object(text))) {
        return layout;
    }

    layout = new QTextLayout(text);
    layoutText(layout, option.rect, (avatarWidth + horizontalAvatarPadding));

    m_postAttachmentUrlCache.insert(text, layout);

    return layout;
}


QTextLayout *NewsFeedDelegate::getAttachmentDescriptionTextLayout(const QString &text, const QStyleOptionViewItem &option) const
{
    QTextLayout *layout;

    if ((layout = m_postAttachmentDescriptionCache.object(text))) {
        return layout;
    }

    QString mangledText = text;
    mangledText.replace('\n', QChar::LineSeparator);
    layout = new QTextLayout(mangledText);

    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    int fontSize = settings.value("fontSize", fontSize).toInt();
    settings.endGroup();

    QFont font = option.font;
    font.setPointSize(option.font.pointSize() + fontSize - 1);
    layout->setFont(font);
    layoutText(layout, option.rect, ((avatarWidth * 2) + horizontalAvatarPadding * 2.5));

    m_postAttachmentDescriptionCache.insert(text, layout);

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
