#include "newsfeedpost.h"
#include "facebookaccount.h"

NewsFeedPost::NewsFeedPost(QObject *parent, FacebookAccount *account, const QString &url, const QString &message)
    : QObject(parent),
    m_account(account),
    m_url(url),
    m_message(message)
{
    connect(m_account, SIGNAL(modified()), SIGNAL(modified()));
}

const QString &NewsFeedPost::url() const
{
    return m_url;
}

const QString &NewsFeedPost::message() const
{
    return m_message;
}

FacebookAccount *NewsFeedPost::author() const
{
    return m_account;
}

