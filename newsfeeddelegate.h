#ifndef NEWSFEEDDELEGATE_H
#define NEWSFEEDDELEGATE_H

#include <QStyledItemDelegate>

class NewsFeedDelegate : public QStyledItemDelegate
{
public:
    NewsFeedDelegate(QObject *parent);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // NEWSFEEDDELEGATE_H
