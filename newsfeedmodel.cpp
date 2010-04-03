#include "fbconnectglobal.h"

#include "newsfeedmodel.h"
#include "newsfeedpost.h"
#include "facebookaccount.h"

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

    NewsFeedPost *np = m_posts[index.row()];
QSize sz ;
    switch (role) {
    case Qt::SizeHintRole:
        return QSize(-1, qMax(np->author()->avatar().height(), 55));
        break;
    case Qt::DisplayRole:
        return np->author()->name() + ": " + np->message();
        break;
    case Qt::DecorationRole:
        return np->author()->avatar();
        break;
    case NewsFeedModel::UrlRole:
        return np->url();
        break;
    }

    return QVariant();
}

void NewsFeedModel::createNewsItem(FacebookAccount *account, const QString &url, const QString &message)
{
    beginInsertRows(QModelIndex(), m_posts.length(), m_posts.length());
    m_posts.append(new NewsFeedPost(this, account, url, message));
    endInsertRows();
}

