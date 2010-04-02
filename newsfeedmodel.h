#ifndef NEWSFEEDMODEL_H
#define NEWSFEEDMODEL_H

#include <QAbstractListModel>

class NewsFeedModel : public QAbstractListModel
{
public:
    NewsFeedModel(QObject *parent);

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;

};

#endif // NEWSFEEDMODEL_H
