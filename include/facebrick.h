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

#ifndef FACEBRICK_H
#define FACEBRICK_H

#include <QObject>

class QNetworkAccessManager;
class FBSession;
class FBError;
class GetConnection;
class QFacebook;

class FaceBrick : public QObject
{
Q_OBJECT
public:
    static FaceBrick *instance(QString client_id, QString client_secret);
    static FaceBrick *instance();

    void setToken(QString token);
    QString getToken();
    QString *getClientId();
    void createConnection();

    QNetworkAccessManager *networkManager() const;
    GetConnection *m_connection;
    QFacebook *m_facebook;
    QString *m_clientId;
    QString *m_clientSecret;
    QString m_token;
private slots:
    void unableToGetStreamRead();
    void errorRequestingPermission(const FBError &error);
private:
    FaceBrick(QString client_id, QString client_secret);

    QNetworkAccessManager * const m_networkAccessManager;
    //FBSession * const m_session;

};

#endif // FACEBRICK_H
