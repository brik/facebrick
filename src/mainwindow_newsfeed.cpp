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

#include <QDebug>
#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "fbrequest.h"
#include "fbsession.h"
#include "fberror.h"

#include "facebrick.h"
#include "facebookaccount.h"
#include "facebookaccountmodel.h"
#include "newsfeedpost.h"
#include "newsfeedmodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newsfeedpostview.h"

void MainWindow::newsFeedListClicked(QModelIndex index)
{
    NewsFeedPostView *nfpv = new NewsFeedPostView(this);

    // Yes, I *know* this line is ugly.
    nfpv->setPost(static_cast<NewsFeedPost *>(m_newsFeedModel->data(index, NewsFeedModel::PostRole).value<void *>()));
    nfpv->show();
}

void MainWindow::newsFeedLoadingError()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);    
#endif
}

void MainWindow::newsFeedLoaded()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
}

void MainWindow::newsFeedLoading()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
}
