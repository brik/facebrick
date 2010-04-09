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

#include <QDesktopServices>
#include <QTextDocument>
#include <QDebug>
#include <QTimer>
#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "fbsession.h"
#include "fbrequest.h"
#include "fberror.h"

#include "mainwindow.h"
#include "newsfeedpost.h"
#include "facebookaccount.h"
#include "newsfeedpost.h"
#include "newsfeedmodel.h"
#include "newsfeedpostview.h"
#include "newsfeeddelegate.h"
#include "facebookaccountmodel.h"
#include "ui_newsfeedpostview.h"

NewsFeedPostView::NewsFeedPostView(QWidget *parent, FBSession *session) :
    QMainWindow(parent),
    m_ui(new Ui::NewsFeedPostView),
    m_post(0),
    m_session(session),
    m_fetchingComments(false)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(windowFlags() | Qt::Window);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif

    m_ui->setupUi(this);
    m_ui->commentsListView->setItemDelegate(new NewsFeedDelegate(this));

    connect(m_ui->action_Go_to_post, SIGNAL(triggered()), SLOT(goToPost()));
    connect(m_ui->commentButton, SIGNAL(clicked()), SLOT(sendComment()));

    QTimer *commentRefreshTimer = new QTimer(this);
    connect(commentRefreshTimer, SIGNAL(timeout()), SLOT(fetchComments()));
    commentRefreshTimer->start(300000 /* 5 minutes */);
}

NewsFeedPostView::~NewsFeedPostView()
{
    delete m_ui;
}

void NewsFeedPostView::sendComment()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
    m_ui->commentButton->setEnabled(false);
    m_ui->commentText->setEnabled(false);

    qDebug() << "Sending comment to " << m_post->id();

    FBRequest* request = FBRequest::request();
    Dictionary params;

    params["post_id"] = m_post->id();
    params["comment"] = m_ui->commentText->text();

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(commentAdded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(commentAddError(FBError)));
    request->call("facebook.Stream.addComment",params);

    m_ui->commentText->setText(QLatin1String(""));
}

void NewsFeedPostView::commentAdded(const QVariant &container)
{
    Q_UNUSED(container);

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif

    m_ui->commentButton->setEnabled(true);
    m_ui->commentText->setEnabled(true);
    fetchComments();

    sender()->deleteLater();
}

void NewsFeedPostView::commentAddError(const FBError &error)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error adding comment: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    MainWindow *mw = qobject_cast<MainWindow *>(parent());
    mw->requestFailedWithFacebookError(error, true);
    m_ui->commentButton->setEnabled(true);
    m_ui->commentText->setEnabled(true);
}

void NewsFeedPostView::goToPost()
{
    QDesktopServices::openUrl(QUrl(m_post->url()));
}

void NewsFeedPostView::setPost(NewsFeedPost *post)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
    if (m_post)
        disconnect(m_post, SIGNAL(modified()));
    m_post = post;
    connect(m_post, SIGNAL(modified()), SLOT(setupUi()));
    setupUi();

    // Request comments
    qDebug() << "Viewing post " << m_post->id();

    fetchComments();
}

void NewsFeedPostView::fetchComments()
{
    if (m_fetchingComments)
        return;

    m_fetchingComments = true;

    FBRequest* request = FBRequest::request();
    Dictionary params;

    // Query to fetch comments
    QString queryOne = "SELECT post_id, fromid, time, text FROM comment WHERE post_id='" + m_post->id() + "'";

    if (m_post->commentsModel()->newestCreatedTime() != 0)
        queryOne += " AND time > " + QString::number(m_post->commentsModel()->newestCreatedTime());

    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT fromid FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    qDebug() << "NewsFeedPostView::setPost: Sending " << fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(commentsLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(commentsLoadError(FBError)));
    request->call("facebook.fql.multiquery",params);
}

void NewsFeedPostView::commentsLoaded(const QVariant &container)
{
    m_fetchingComments = false;

    if (m_ui->commentsListView->model() != m_post->commentsModel()) {
        // HACK: this was previously in setupUi(), but if it's there we get fucked comments on Maemo 5 for reasons I have yet to understand.
        m_ui->commentsListView->setModel(m_post->commentsModel());
    }

    qDebug() << "Comments loaded: " << container;

    if (container.type() == QVariant::List) {
        QVariantList list = container.toList();

        //foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
        //    QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();
        foreach (const QVariant &commentHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> commentData = commentHash.toHash();
            qDebug() << commentData;

            FacebookAccount *author = FacebookAccountModel::instance()->account(commentData["fromid"].toULongLong());
            Q_ASSERT(author);

            NewsFeedPost *comment = new NewsFeedPost(m_post, author, QLatin1String(""),
                                                     commentData["time"].toLongLong(),
                                                     QLatin1String(""),
                                                     commentData["text"].toString());
            m_post->commentsModel()->insertNewsItem(comment);
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

    qDebug() << "NewsFeedPostView::commentsLoaded: Total comments count is " << m_post->commentsModel()->rowCount(QModelIndex());
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif

    sender()->deleteLater();
}

void NewsFeedPostView::commentsLoadError(const FBError &error)
{
    m_fetchingComments = false;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error loading newsfeed: %1 (%2)").arg(error.code()).arg(error.description()));
#endif
    MainWindow *mw = qobject_cast<MainWindow *>(parent());
    mw->requestFailedWithFacebookError(error, true);
}

void NewsFeedPostView::setupUi()
{
    setWindowTitle(m_post->author()->name());
}

void NewsFeedPostView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
