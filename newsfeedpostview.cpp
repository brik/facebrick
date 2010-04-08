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
#include <QDebug>

#include "fbsession.h"
#include "fbrequest.h"
#include "fberror.h"

#include "newsfeedpost.h"
#include "facebookaccount.h"
#include "newsfeedpost.h"
#include "newsfeedcomment.h"
#include "newsfeedpostview.h"
#include "facebookaccountmodel.h"
#include "newsfeedcommentsmodel.h"
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
}

NewsFeedPostView::~NewsFeedPostView()
{
    delete m_ui;
}

void NewsFeedPostView::setPost(const NewsFeedPost * const post)
{
    if (m_post)
        disconnect(m_post, SIGNAL(modified()));
    m_post = post;
    connect(m_post, SIGNAL(modified()), SLOT(setupUi()));
    setupUi();

    // Request comments
    qDebug() << "Viewing post " << m_post->id();

    FBRequest* request = FBRequest::request();
    Dictionary params;

    // Query to fetch comments
    QString queryOne = "SELECT post_id, fromid, time, text FROM comment WHERE post_id='" + m_post->id() + "'";

    if (m_post->commentsModel()->newestCreatedTime() != 0)
        queryOne += " WHERE time > " + QString::number(m_post->commentsModel()->newestCreatedTime());

    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT fromid FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    qDebug() << "fetchNewsFeed: Sending " << fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(commentsLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(commentsLoadError(FBError)));
    request->call("facebook.fql.multiquery",params);
}

void NewsFeedPostView::commentsLoaded(const QVariant &container)
{
    qDebug() << "Comments loaded: " << container;

    if (container.type() == QVariant::List) {
        QVariantList list = container.toList();

        //foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
        //    QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();
        foreach (const QVariant &commentHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> commentData = commentHash.toHash();
            qDebug() << commentData;

            // Make sure it is for the right post.
            if (m_post->id() != commentData["post_id"].toString()) {
                qWarning("Recieved a comment for a post that isn't current (%s vs %s)",
                         qPrintable(m_post->id()), qPrintable(commentData["post_id"].toString()));
                continue;
            }

            FacebookAccount *author = FacebookAccountModel::instance()->account(commentData["fromid"].toULongLong());
            Q_ASSERT(author);

            NewsFeedComment *comment = new NewsFeedComment(author, author, commentData["time"].toLongLong(),
                                                           commentData["text"].toString());
            m_post->commentsModel()->insertComment(comment);
        }

        foreach (const QVariant &newsFeedUserHash, list.at(1).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedUserData = newsFeedUserHash.toHash();

            // Get (or create - though this should have already been done above) the account
            FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedUserData["id"].toLongLong());
            Q_ASSERT(account);

            account->setName(newsFeedUserData["name"].toString());
            account->setAvatar(newsFeedUserData["pic_square"].toString());
        }
    }
}

void NewsFeedPostView::commentsLoadError(const FBError &error)
{
    qDebug() << "commentsLoadError: " << error.code() << ": " << error.description();
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

    m_ui->commentsListView->setModel(m_post->commentsModel());
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
