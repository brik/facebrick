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
#include "newsfeedpostview.h"
#include "newsfeeddelegate.h"
#include "ui_newsfeedpostview.h"
#include "settingsdialog.h"
#include "newsfeeditemwidget.h"

NewsFeedPostView::NewsFeedPostView(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::NewsFeedPostView),
    m_post(0),
    m_fetchingComments(false),
    m_doingLikeDislike(false),
    m_likeAction(new QAction(this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(windowFlags() | Qt::Window);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif

    m_ui->setupUi(this);
    m_ui->commentsListView->setMetaObject(&NewsFeedItemWidget::staticMetaObject);
    //m_ui->commentsListView->setItemDelegate(new NewsFeedDelegate(this));

    connect(m_ui->action_Go_to_post, SIGNAL(triggered()), SLOT(goToPost()));
    connect(m_ui->action_Synchronise, SIGNAL(triggered()), SLOT(fetchComments()));
    connect(m_ui->actionS_ettings, SIGNAL(triggered()), SLOT(onSettingsMenuAction()));
    connect(m_ui->commentButton, SIGNAL(clicked()), SLOT(sendComment()));

    QTimer *commentRefreshTimer = new QTimer(this);
    connect(commentRefreshTimer, SIGNAL(timeout()), SLOT(fetchComments()));
    commentRefreshTimer->start(300000 /* 5 minutes */);
}

NewsFeedPostView::~NewsFeedPostView()
{
    delete m_ui;
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

void NewsFeedPostView::setupUi()
{
    setWindowTitle(m_post->author()->name());

    if (m_post->iLikeThis()) {
        m_likeAction->deleteLater();
        m_likeAction = new QAction(tr("&Unlike post"), this);
        connect(m_likeAction, SIGNAL(triggered()), SLOT(iDislikeThis()));

        QList<QAction *> actions;
        actions.append(m_likeAction);
        m_ui->menu_File->addActions(actions);
    } else {
        m_likeAction->deleteLater();
        m_likeAction = new QAction(tr("&Like post"), this);
        connect(m_likeAction, SIGNAL(triggered()), SLOT(iLikeThis()));

        QList<QAction *> actions;
        actions.append(m_likeAction);
        m_ui->menu_File->addActions(actions);
    }
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

void NewsFeedPostView::onSettingsMenuAction()
{
    SettingsDialog dialog;
    dialog.exec();
}
