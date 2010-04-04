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

#include "newsfeedpost.h"
#include "facebookaccount.h"

NewsFeedPost::NewsFeedPost(QObject *parent, FacebookAccount *account, long long createdTime, const QString &url, const QString &message)
    : QObject(parent),
    m_createdTime(createdTime),
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

long long NewsFeedPost::createdTime() const
{
    return m_createdTime;
}
