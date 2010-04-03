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
    int height = 60;

    QSize imageSize = index.data(Qt::DecorationRole).value<QImage>().size();
    QSize fontSize = option.fontMetrics.boundingRect(option.rect, 0, index.data(Qt::DisplayRole).toString()).size();
    QSize fontSizeTwo = option.fontMetrics.boundingRect(option.rect, 0, index.data(NewsFeedModel::NameRole).toString()).size();

    return QSize(imageSize.width() + qMax(fontSize.width(), fontSizeTwo.width()) + 20, height);
}

void NewsFeedDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Fetch image
    QImage img = index.data(Qt::DecorationRole).value<QImage>();

    // Draw 10px from left
    painter->drawImage(option.rect.left() + 10, option.rect.top() + 5, img);

    // Adjust text to push further right, leave 10px padding either side of the image, plus image width
    QRect textRect = option.rect.adjusted(20 + img.width(), 5, 0, 0);

    // Set bold
    painter->save();
    QFont f = painter->font();
    f.setBold(true);
    painter->setFont(f);

    // Calculate offset for name role
    QRect nameTextMetrics;

    // Draw name role, saving offset rect for later reuse
    painter->drawText(textRect, Qt::AlignTop, index.data(NewsFeedModel::NameRole).toString(), &nameTextMetrics);

    // Reset bold
    painter->restore();

    // Move message below name, using calculated offset
    textRect.adjust(0, nameTextMetrics.height(), 0, 0);
    painter->drawText(textRect, Qt::AlignTop, index.data(Qt::DisplayRole).toString());
}
