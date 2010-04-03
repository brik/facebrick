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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QObject>
#include <QDialog>

#include "fbconnectglobal.h"

class FBSession;
class FBError;

class LoginDialog : public QObject
{
Q_OBJECT
public:
    explicit LoginDialog(FBSession *session);

signals:

public slots:
private slots:
    void sessionDidLogin(FBUID uid);
    void unableToGetStreamRead();
    void errorRequestingPermission(const FBError &error);
private:
    FBSession *m_session;
};

#endif // LOGINDIALOG_H
