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

#include <QTimer>
#include <QDebug>

#include "fbrequest.h"
#include "fberror.h"
#include "fblogindialog.h"
#include "fbsession.h"
#include "fbpermissiondialog.h"
#include "facebrick.h"

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"
#include "newsfeedpost.h"
#include "newsfeed.h"
#include "facebookaccountmodel.h"
#include "facebookaccount.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "ui_desktopwidget.h"

static MainWindow *win = NULL;

MainWindow *MainWindow::instance()
{
    if (win == NULL)
        win = new MainWindow(0);

    return win;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
    m_ui->setupUi(this);
    m_ui->postsListView->setModel(FaceBrick::instance()->m_newsFeedModel);
    m_ui->postsListView->setItemDelegate(new NewsFeedDelegate(this));

    // Instantiate model so it's parented to us.
    FacebookAccountModel::instance(this);

    // Menu
    connect(m_ui->action_Synchronise, SIGNAL(triggered()), NewsFeed::instance(), SLOT(fetchNewsFeed()));
    connect(m_ui->action_Logout, SIGNAL(triggered()), this, SLOT(onLogoutMenuAction()));
    connect(m_ui->actionS_ettings, SIGNAL(triggered()), this, SLOT(onSettingsMenuAction()));

    // Status
    connect(m_ui->updateStatusButton, SIGNAL(clicked()), this, SLOT(sendStatusUpdate()));

    // Session
    connect(FaceBrick::instance()->session(), SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));

    // News posts
    connect(m_ui->postsListView, SIGNAL(clicked(QModelIndex)), this, SLOT(newsFeedListClicked(QModelIndex)));

    connect(NewsFeed::instance(), SIGNAL(newsFeedLoadingErrorSignal()), this, SLOT(newsFeedLoadingError()));
    connect(NewsFeed::instance(), SIGNAL(newsFeedLoaded()), this, SLOT(newsFeedLoaded()));
    connect(NewsFeed::instance(), SIGNAL(newsFeedLoading()), this, SLOT(newsFeedLoading()));

    NewsFeed::instance()->fetchNewsFeed();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::sessionDidLogout()
{
    // TODO: go back to logindialog?
    QApplication::exit();
}

void MainWindow::onLogoutMenuAction()
{
    FaceBrick::instance()->session()->logout();
}

void MainWindow::onSettingsMenuAction()
{
    SettingsDialog::instance()->exec();
}
