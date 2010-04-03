#ifndef NEWSFEEDMODEL_H
#define NEWSFEEDMODEL_H

#include <QAbstractListModel>

#include "fbconnectglobal.h"

class NewsFeedPost;
class FacebookAccount;

class NewsFeedModel : public QAbstractListModel
{
public:
    explicit NewsFeedModel(QObject *parent);
    virtual ~NewsFeedModel();

    enum Roles
    {
        UrlRole = Qt::UserRole
    };

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;
    void createNewsItem(FacebookAccount *account, const QString &url, const QString &message);
private:
    QList<NewsFeedPost *> m_posts;
};

#endif // NEWSFEEDMODEL_H
