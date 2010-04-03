#ifndef FACEBOOKACCOUNTMODEL_H
#define FACEBOOKACCOUNTMODEL_H

#include <QAbstractListModel>

#include "fbconnectglobal.h"

class FacebookAccount;

class FacebookAccountModel : public QAbstractListModel
{
public:
    FacebookAccountModel(QObject *parent);

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex&, int) const;

    FacebookAccount *account(FBUID uid) const;
    FacebookAccount *createAccount(FBUID uid);

private:
    QList<FacebookAccount *> m_accounts;
};

#endif // FACEBOOKACCOUNTMODEL_H
