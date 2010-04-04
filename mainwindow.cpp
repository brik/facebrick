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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QDebug>

#include "fbrequest.h"
#include "fberror.h"
#include "fblogindialog.h"
#include "fbsession.h"
#include "fbpermissiondialog.h"

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"
#include "newsfeedpost.h"
#include "facebookaccountmodel.h"
#include "facebookaccount.h"

static QString UserId;

MainWindow::MainWindow(QWidget *parent, FBSession *session) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_fbSession(session),
    m_newsFeedModel(new NewsFeedModel(this)),
    m_facebookAccountModel(new FacebookAccountModel(this)),
    m_updatingNewsFeed(false),
    m_lastUpdatedNewsFeed(0)
{
    m_ui->setupUi(this);
    m_ui->postsListView->setModel(m_newsFeedModel);
    m_ui->postsListView->setItemDelegate(new NewsFeedDelegate(this));

    connect(m_ui->action_Logout, SIGNAL(triggered()), this, SLOT(onLogoutMenuAction()));
    connect(m_ui->updateStatusButton, SIGNAL(clicked()), this, SLOT(sendStatusUpdate()));
    connect (m_fbSession, SIGNAL(sessionDidLogout()), this, SLOT(sessionDidLogout()));

    connect(m_ui->postsListView, SIGNAL(clicked(QModelIndex)), this, SLOT(newsFeedListClicked(QModelIndex)));

    UserId = QString::number(m_fbSession->uid(), 10);

    fetchNewsFeed();
    connect(m_ui->checkForNewPosts, SIGNAL(clicked()), this, SLOT(fetchNewsFeed()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_fbSession;
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

void MainWindow::requestFailedWithFacebookError(const FBError &error)
{
    qDebug() << "facebook error is " << error.code() << " - " << error.description();

    // If it's a permissions error, request permission.
    if (error.code() == 250) {
        // Find permission requested
        // XXX: is there any easier way to do this?
        QString permission = error.description().split(QChar(' ')).last();
        qDebug() << "About to request " << permission;

        // XXX: Deallocated in the slots. This isn't a good way to do MM, come up with a better design :(
        // XXX: the error condition (dialogDidFailWithError()) in particular currently leaks, and might recurse - not good.
        FBPermissionDialog *d = new FBPermissionDialog(this->m_fbSession);
        connect(d, SIGNAL(dialogDidSucceed()), this, SLOT(permissionGranted()));
        connect(d, SIGNAL(dialogDidCancel()), this, SLOT(permissionDeniedOrCancelled()));
        connect(d, SIGNAL(dialogDidFailWithError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
        d->setPermissionToRequest(permission);
        d->load();
        d->show();
    }
}

void MainWindow::permissionGranted()
{
    qDebug() << "Permission granted";
    sender()->deleteLater();
}

void MainWindow::permissionDeniedOrCancelled()
{
    qDebug() << "Permission denied";
    sender()->deleteLater();
}

void MainWindow::onLogoutMenuAction()
{
    m_fbSession->logout();
}

void MainWindow::sendStatusUpdate()
{
    FBRequest* request = FBRequest::request();
    Dictionary params;
    params["status"] = m_ui->statusText->toPlainText();

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(statusUpdated(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(requestFailedWithFacebookError(FBError)));
    request->call("Status.set",params);
}

void MainWindow::statusUpdated(const QVariant &)
{
    qDebug() << "Status updated!";
    m_ui->statusText->setPlainText(QLatin1String(""));

    // Trigger a check for our own post
    fetchNewsFeed();
}
