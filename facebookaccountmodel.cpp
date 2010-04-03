#include "facebookaccountmodel.h"
#include "facebookaccount.h"

FacebookAccountModel::FacebookAccountModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FacebookAccountModel::rowCount(const QModelIndex&) const
{
    return 0;
}

QVariant FacebookAccountModel::data(const QModelIndex&, int) const
{
    return QVariant();
}

FacebookAccount *FacebookAccountModel::account(FBUID uid) const
{
    foreach (FacebookAccount *account, m_accounts) {
        if (account->uid() == uid) {
            return account;
        }
    }

    return NULL;
}

FacebookAccount *FacebookAccountModel::createAccount(FBUID uid)
{
    FacebookAccount *account = new FacebookAccount(this, uid);
    beginInsertRows(QModelIndex(), m_accounts.length(), m_accounts.length());
    m_accounts.append(account);
    endInsertRows();

    return account;
}
