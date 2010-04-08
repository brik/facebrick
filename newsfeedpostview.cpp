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

#include <QTextDocument>

#include "fbsession.h"
#include "fbrequest.h"

#include "newsfeedpost.h"
#include "facebookaccount.h"
#include "newsfeedpost.h"
#include "newsfeedpostview.h"
#include "ui_newsfeedpostview.h"

NewsFeedPostView::NewsFeedPostView(QWidget *parent, FBSession *session) :
    QDialog(parent),
    m_ui(new Ui::NewsFeedPostView),
    m_post(0),
    m_session(session)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    m_ui->setupUi(this);
    m_ui->newsPost->setOpenExternalLinks(true);

    // Bold author name
    QFont f = m_ui->authorName->font();
    f.setBold(true);
    m_ui->authorName->setFont(f);

    // Request comments
    //SELECT post_id, fromid, time, text FROM comment WHERE post_id=<post_id>
}

NewsFeedPostView::~NewsFeedPostView()
{
    delete m_ui;
}

void NewsFeedPostView::setPost(const NewsFeedPost * const post)
{
    if (m_post)
        disconnect(m_post, SIGNAL(modified()));
    connect(m_post, SIGNAL(modified()), SLOT(setupUi()));
    m_post = post;
    setupUi();
}

void NewsFeedPostView::setupUi()
{
    m_ui->authorAvatar->setPixmap(m_post->author()->avatar());
    m_ui->authorName->setText(m_post->author()->name());
    m_ui->newsPost->setText(QLatin1String("<a href=\"") +
                            m_post->url() +
                            QLatin1String("\">") +
                            Qt::escape(m_post->message()) +
                            QLatin1String("</a>"));
}

void NewsFeedPostView::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
