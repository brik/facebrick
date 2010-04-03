#include "newsfeedpost.h"

NewsFeedPost::NewsFeedPost(QObject *parent, FBUID userId, const QString &url, const QString &message)
    : QObject(parent),
    m_userId(userId),
    m_url(url),
    m_message(message)
{
}

const QString &NewsFeedPost::url() const
{
    return m_url;
}

const QString &NewsFeedPost::message() const
{
    return m_message;
}

const QString NewsFeedPost::author() const
{
    return QString::number(m_userId);
}

