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

#ifndef FACEBOOKACCOUNTMODEL_H
#define FACEBOOKACCOUNTMODEL_H

#include <QAbstractListModel>

#include "fbconnectglobal.h"

class FacebookAccount;

class FacebookAccountModel : public QAbstractListModel
{
public:
    /** NOTE: This is a singleton. Use ::instance().
      */
    FacebookAccountModel(QObject *parent);

    /** Return the account model instance. Note that the first time this
      * is called, it must be given a parent. If it is not, it will return 0.
      */
    static FacebookAccountModel *instance(QObject *parent = 0);

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;

    FacebookAccount *account(FBUID uid);

private:
    QList<FacebookAccount *> m_accounts;
};

#endif // FACEBOOKACCOUNTMODEL_H
