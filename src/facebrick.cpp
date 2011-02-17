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
#include "getconnection.h"
#include "qfacebook.h"

static FaceBrick *sinstance = NULL;

FaceBrick *FaceBrick::instance(QString client_id, QString client_secret)
{
    Q_ASSERT(sinstance == NULL);

    sinstance = new FaceBrick(client_id, client_secret);
    return sinstance;
}

FaceBrick *FaceBrick::instance()
{
    Q_ASSERT(sinstance);

    return sinstance;
}

FaceBrick::FaceBrick(QString client_id, QString client_secret)
    : QObject(0),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_clientId(new QString(client_id)),
    m_clientSecret(new QString(client_secret))
{
}

QNetworkAccessManager *FaceBrick::networkManager() const
{
    return m_networkAccessManager;
}

void FaceBrick::setToken(QString token)
{
    m_token = token;
    m_facebook = new QFacebook(token, this);
}

QString FaceBrick::getToken()
{
    return m_token;
}

QString *FaceBrick::getClientId()
{
    return m_clientId;
}

void FaceBrick::createConnection()
{
    m_connection = new GetConnection(0, m_token);
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
