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

#include "facebookaccountmodel.h"
#include "facebookaccount.h"

FacebookAccountModel::FacebookAccountModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

FacebookAccountModel *FacebookAccountModel::instance(QObject *parent)
{
    static FacebookAccountModel *myinstance = 0;

    if (myinstance == 0 && parent == 0) {
        // not allowed to not parent at least once.
        return 0;
    } else if (!myinstance) {
        // create instance
        myinstance = new FacebookAccountModel(parent);
    }

    return myinstance;
}

int FacebookAccountModel::rowCount(const QModelIndex&) const
{
    return 0;
}

QVariant FacebookAccountModel::data(const QModelIndex&, int) const
{
    return QVariant();
}

FacebookAccount *FacebookAccountModel::account(FBUID uid)
{
    foreach (FacebookAccount *account, m_accounts) {
        if (account->uid() == uid) {
            return account;
        }
    }

    FacebookAccount *account = new FacebookAccount(this, uid);
    beginInsertRows(QModelIndex(), m_accounts.length(), m_accounts.length());
    m_accounts.append(account);
    endInsertRows();

    return account;
}
