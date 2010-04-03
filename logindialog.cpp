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

#include "fbsession.h"
#include "fblogindialog.h"
#include "fbpermissiondialog.h"
#include "fberror.h"

#include "logindialog.h"

LoginDialog::LoginDialog(FBSession *session) :
    QDialog(0),
    m_session(session)
{
    connect (m_session,SIGNAL(sessionDidLogin(FBUID)), this, SLOT(sessionDidLogin(FBUID)));

    FBLoginDialog *m_fbLoginDialog = new FBLoginDialog();
    m_fbLoginDialog->show();
    // TODO: m_fbLoginDialog is sort of kind of leaked, as is LoginDialog itself.
    // Not a big issue since both are only instantiated once, but it should be fixed.
}

void LoginDialog::sessionDidLogin(FBUID uid)
{
    Q_UNUSED(uid)

    // Request stream_read permissions (needed to show stupid newsfeed, and stupid FB API won't tell us we don't have it.)
    // TODO: it might be nice to investigate if we can check if we have this perm already to avoid showing multiple times.
    // TODO: permission dialog is leaked, see note in constructor
            // TODO: this is really weirdly broken if read_stream is already had. sigh.
//    FBPermissionDialog *d = new FBPermissionDialog(this->m_session);
//    connect(d, SIGNAL(dialogDidSucceed()), this, SLOT(close()));
//    connect(d, SIGNAL(dialogDidCancel()), this, SLOT(unableToGetStreamRead()));
//    connect(d, SIGNAL(dialogDidFailWithError(FBError)), this, SLOT(errorRequestingPermission(FBError)));
//    d->setPermissionToRequest("read_stream");
//    d->show();
}

void LoginDialog::errorRequestingPermission(const FBError &error)
{
    qDebug() << "Error requesting stream_read: " << error.code() << ": " << error.description();

    if (error.code() == 0) {
        // It seems error code 0 is emitted if we already have the permission.
        // How delightful.
        // Well, let's handle it by pretending it worked
        qDebug() << "Faking success, closing stuff";
        FBPermissionDialog *d = qobject_cast<FBPermissionDialog *>(sender());
        d->close();
        close();
    }
    else {
        unableToGetStreamRead();
    }
}

void LoginDialog::unableToGetStreamRead()
{
    qFatal("I require you to accept the stream_read permission for me to keep working, sorry.");
}
