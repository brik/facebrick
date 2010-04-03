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
    case Qt::DisplayRole:
        return np->message();
        break;
    case Qt::DecorationRole:
        return np->author()->avatar();
        break;
    case NewsFeedModel::NameRole:
        return np->author()->name();
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
    NewsFeedPost *np = new NewsFeedPost(this, account, url, message);
    connect(np, SIGNAL(modified()), this, SLOT(onChildModified()));
    m_posts.append(np);
    endInsertRows();
}

void NewsFeedModel::onChildModified()
{
    // Find the child that was modified
    NewsFeedPost *np = qobject_cast<NewsFeedPost *>(sender());
    Q_ASSERT(np);

    qDebug("onChildModified for %p", np);
    for (int i = 0; i < m_posts.count(); ++i) {
        if (np == m_posts[i]) {
            dataChanged(createIndex(i, 0), createIndex(i, 0));
            return;
        }
    }
}
