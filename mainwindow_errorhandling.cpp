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

#include "fberror.h"
#include "fbpermissiondialog.h"

#include "mainwindow.h"

void MainWindow::requestFailedWithFacebookError(const FBError &error)
{
    qDebug() << "facebook error is " << error.code() << " - " << error.description();

    // If it's a permissions error, request permission.
    if (error.code() == 250) {
        // Find permission requested
        // XXX: is there any easier way to do this?
        QString permission = error.description().split(QChar(' ')).last();
        qDebug() << "About to request " << permission;

        // Note: this is deallocated in the relevant slots.
        FBPermissionDialog *d = new FBPermissionDialog(this->m_fbSession);
        connect(d, SIGNAL(dialogDidSucceed()), this, SLOT(permissionGranted()));
        connect(d, SIGNAL(dialogDidCancel()), this, SLOT(permissionDeniedOrCancelled()));
        connect(d, SIGNAL(dialogDidFailWithError(FBError)), this, SLOT(permissionRequestFailedWithError(FBError)));
        d->setPermissionToRequest(permission);
        d->load();
        d->show();
    }
}

void MainWindow::permissionRequestFailedWithError(const FBError &error)
{
    qDebug() << "Permission request failed! Error code: " << error.code() << ": " << error.description();
    sender()->deleteLater();

    // Don't try do anything smart like calling requestFailedWithFacebookError,
    // because it might just end up getting us back here.
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

