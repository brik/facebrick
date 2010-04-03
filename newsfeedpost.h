#ifndef NEWSFEEDPOST_H
#define NEWSFEEDPOST_H

#include <QObject>

#include "fbconnectglobal.h"

class FacebookAccount;

class NewsFeedPost : QObject
{
Q_OBJECT
public:
    explicit NewsFeedPost(QObject *parent, FacebookAccount *account, const QString &url, const QString &message);

    const QString &url() const;
    const QString &message() const;
    FacebookAccount *author() const;

private:
    FacebookAccount *m_account;
    QString m_url;
    QString m_message;
};

#endif // NEWSFEEDPOST_H
