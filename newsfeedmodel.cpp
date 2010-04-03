#include "fbconnectglobal.h"

#include "newsfeedmodel.h"
#include "newsfeedpost.h"

NewsFeedModel::NewsFeedModel(QObject *parent) : QAbstractListModel(parent)
{
}

NewsFeedModel::~NewsFeedModel()
{
    foreach (NewsFeedPost *post, m_posts) {
        delete post;
    }
}

int NewsFeedModel::rowCount(const QModelIndex&) const
{
    return m_posts.count();
}

QVariant NewsFeedModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_posts.length())
        return QVariant();

    NewsFeedPost *np = NULL;

    switch (role) {
    case Qt::DisplayRole:
        np = m_posts[index.row()];
        return np->author() + ": " + np->message();
        break;
    case NewsFeedModel::UrlRole:
        np = m_posts[index.row()];
        return np->url();
        break;
    }

    return QVariant();
}

void NewsFeedModel::createNewsItem(FBUID userId, const QString &url, const QString &message)
{
    beginInsertRows(QModelIndex(), m_posts.length(), m_posts.length());
    m_posts.append(new NewsFeedPost(this, userId, url, message));
    endInsertRows();
}

