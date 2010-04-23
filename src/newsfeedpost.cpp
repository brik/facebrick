/*
 * Copyright (C) 2010 Robin Burchell <robin.burchell@collabora.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <QDateTime>

#include "newsfeedpost.h"
#include "facebookaccount.h"
#include "newsfeedmodel.h"

NewsFeedPost::NewsFeedPost(QObject *parent, FacebookAccount *account, const QString &postId,
                           long long createdTime, const QString &url, const QString &message)
    : QObject(parent),
    m_createdTime(createdTime),
    m_account(account),
    m_url(url),
    m_message(message),
    m_id(postId),
    m_iLikeThis(false),
    m_commentsModel(new NewsFeedModel(this, false)),
    m_hasAttachment(false)
{
    connect(m_account, SIGNAL(modified()), SIGNAL(modified()));

    // add ourself so the user knows who they are replying to.
    m_commentsModel->insertNewsItem(this);
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

long long NewsFeedPost::createdTime() const
{
    return m_createdTime;
}

QString NewsFeedPost::timeAsString() const
{
    QDateTime dateTime = QDateTime::fromTime_t(m_createdTime);
    QDateTime now = QDateTime::currentDateTime();

    int seconds = dateTime.secsTo(now);
    int minutes = seconds / 60;
    int hours = seconds / (60 * 60);
    int days = dateTime.daysTo(now);

    minutes -= hours * 60;

    // make the times a little fuzzy so it's 'easier' to read
    if (hours > 0 && minutes < 5)
        minutes = 0;
    else if (minutes > 55) {
        minutes = 0;
        hours += 1;
    }

    if (minutes && !hours) {
        return tr("%1m ago").arg(minutes);
    } else if (hours < 6 && minutes > 0) {
        return tr("%1h %2m ago").arg(hours).arg(minutes);
    } else if (hours < 6 && minutes == 0) {
        return tr("%1h ago").arg(hours);
    } else if (days < 8) { // See if it's in the past week
        switch (days) {
        case 0:
            return tr("Today");
            break;
        case 1:
            return tr("Yesterday");
            break;
        default:
            return dateTime.date().toString("dddd");
            break;
        }
    } else {
        return dateTime.date().toString(Qt::SystemLocaleShortDate);
    }

    return QLatin1String("I have no idea how long ago :(");
}

const QString &NewsFeedPost::id() const
{
    return m_id;
}

void NewsFeedPost::setILikeThis(bool iLikeThis)
{
    m_iLikeThis = iLikeThis;
}

bool NewsFeedPost::iLikeThis() const
{
    return m_iLikeThis;
}

NewsFeedModel *NewsFeedPost::commentsModel() const
{
    return m_commentsModel;
}
