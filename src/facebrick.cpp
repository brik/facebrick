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

#include <QNetworkAccessManager>
#include <QDebug>

#include "fbpermissiondialog.h"
#include "fberror.h"

#include "facebrick.h"

static FaceBrick *sinstance = NULL;

FaceBrick *FaceBrick::instance(FBSession *session)
{
    Q_ASSERT(sinstance == NULL);

    sinstance = new FaceBrick(session);
    return sinstance;
}

FaceBrick *FaceBrick::instance()
{
    Q_ASSERT(sinstance);

    return sinstance;
}

FaceBrick::FaceBrick(FBSession *session)
    : QObject(0),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_session(session)
{
    // Request stream_read permissions (needed to show stupid newsfeed, and stupid FB API won't tell us we don't have it.)
    // TODO: it might be nice to investigate if we can check if we have this perm already to avoid showing multiple times.
    // TODO: permission dialog is leaked, see note in constructor
    FBPermissionDialog *d = new FBPermissionDialog(m_session);
    connect(d, SIGNAL(dialogDidCancel()), this, SLOT(unableToGetStreamRead()));
    connect(d, SIGNAL(dialogDidFailWithError(FBError)), this, SLOT(errorRequestingPermission(FBError)));
    d->setPermissionToRequest("read_stream");
    d->show();
}

QNetworkAccessManager *FaceBrick::networkManager() const
{
    return m_networkAccessManager;
}

FBSession *FaceBrick::session() const
{
    return m_session;
}

void FaceBrick::errorRequestingPermission(const FBError &error)
{
    qDebug() << "Error requesting stream_read: " << error.code() << ": " << error.description();

    if (error.code() == 0) {
        // It seems error code 0 is emitted if we already have the permission.
        // How delightful.
        // Well, let's handle it by pretending it worked
        qDebug() << "Faking success, closing stuff";
    }
    else {
        unableToGetStreamRead();
    }
}

void FaceBrick::unableToGetStreamRead()
{
    qFatal("I require you to accept the stream_read permission for me to keep working, sorry.");
}
