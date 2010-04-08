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

#include "newsfeedcomment.h"
#include "facebookaccount.h"

NewsFeedComment::NewsFeedComment(QObject *parent, FacebookAccount *author, long long createdTime, const QString &text) :
    QObject(parent),
    m_author(author),
    m_createdTime(createdTime),
    m_text(text)
{
    connect(m_author, SIGNAL(modified()), SIGNAL(modified()));
}

FacebookAccount *NewsFeedComment::author() const
{
    return m_author;
}

long long NewsFeedComment::createdTime() const
{
    return m_createdTime;
}

const QString &NewsFeedComment::text() const
{
    return m_text;
}
