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

#include "fbrequest.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::sendStatusUpdate()
{
    FBRequest* request = FBRequest::request();
    Dictionary params;
    params["status"] = m_ui->statusText->toPlainText();

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(statusUpdated(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(statusUpdateError(FBError)));
    request->call("Status.set",params);

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
}

void MainWindow::statusUpdateError(const FBError &error)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif

    // Pass to generic error handler
    requestFailedWithFacebookError(error);
}

void MainWindow::statusUpdated(const QVariant &)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif

    qDebug() << "Status updated!";
    m_ui->statusText->setPlainText(QLatin1String(""));

    // Trigger a check for our own post
    fetchNewsFeed();
}
