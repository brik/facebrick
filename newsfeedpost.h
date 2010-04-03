#ifndef NEWSFEEDPOST_H
#define NEWSFEEDPOST_H

#include "fbconnectglobal.h"

class NewsFeedPost
{
public:
    explicit NewsFeedPost(FBUID userId, const QString &url, const QString &message);

    const QString &url() const;
    const QString &message() const;
    const QString author() const;

private:
    FBUID m_userId;
    QString m_url;
    QString m_message;
};

#endif // NEWSFEEDPOST_H
