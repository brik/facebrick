#ifndef NEWSFEEDMODEL_H
#define NEWSFEEDMODEL_H

#include <QAbstractListModel>

#include "fbconnectglobal.h"

class NewsFeedPost;

class NewsFeedModel : public QAbstractListModel
{
public:
    explicit NewsFeedModel(QObject *parent);

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    void createNewsItem(FBUID userId, const QString &url, const QString &message);
private:
    QList<NewsFeedPost *> m_posts;
};

#endif // NEWSFEEDMODEL_H
