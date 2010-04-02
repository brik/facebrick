#include "newsfeedmodel.h"

NewsFeedModel::NewsFeedModel(QObject *parent) : QAbstractListModel(parent)
{
}

int NewsFeedModel::rowCount(const QModelIndex&) const
{
    return 0;
}

QVariant NewsFeedModel::data(const QModelIndex&, int) const
{
    return QVariant();
}
