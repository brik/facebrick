/*
 * Copyright (C) 2010 Kamilla Bremeraunet <kamilla@bremeraunet.com>
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

#include "fberror.h"
#include "fbpermissiondialog.h"

#include "facebrick.h"
#include "newsfeed.h"

void NewsFeed::requestFailedWithFacebookError(const FBError &error, bool handled)
{
    qDebug() << "facebook error is " << error.code() << " - " << error.description();

    // If it's a permissions error, request permission.
    if (error.code() == 250) {
        // Find permission requested
        // XXX: is there any easier way to do this?
        QString permission = error.description().split(QChar(' ')).last();
        qDebug() << "About to request " << permission;

        // Note: this is deallocated in the relevant slots.
        FBPermissionDialog *d = new FBPermissionDialog(FaceBrick::instance()->session());
        connect(d, SIGNAL(dialogDidSucceed()), this, SLOT(permissionGranted()));
        connect(d, SIGNAL(dialogDidCancel()), this, SLOT(permissionDeniedOrCancelled()));
        connect(d, SIGNAL(dialogDidFailWithError(FBError)), this, SLOT(permissionRequestFailedWithError(FBError)));
        d->setPermissionToRequest(permission);
        d->load();
        d->show();
    } else if (!handled) {
        // If it wasn't handled elsewhere, let the user know
/*#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this, tr("Facebook error: %1 (%2)").arg(error.code()).arg(error.description()));
#endif*/
    }

    sender()->deleteLater();
}

void NewsFeed::permissionRequestFailedWithError(const FBError &error)
{
/*#ifdef Q_WS_MAEMO_5
    QMaemo5InformationBox::information(this, tr("Error requesting Facebook permission: %1 (%2)").arg(error.code()).arg(error.description()));
#endif*/
    qDebug() << "Permission request failed! Error code: " << error.code() << ": " << error.description();
    sender()->deleteLater();

    // Don't try do anything smart like calling requestFailedWithFacebookError,
    // because it might just end up getting us back here.
}

void NewsFeed::permissionGranted()
{
    qDebug() << "Permission granted";
    sender()->deleteLater();
}

void NewsFeed::permissionDeniedOrCancelled()
{
    qDebug() << "Permission denied";
    sender()->deleteLater();
}

