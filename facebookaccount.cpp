#include "fbconnectglobal.h"

#include "facebookaccount.h"

FacebookAccount::FacebookAccount(QObject *parent, FBUID uid)
    : QObject(parent),
    m_uid(uid)
{
}

FBUID FacebookAccount::uid() const
{
    return m_uid;
}

const QString &FacebookAccount::name() const
{
    return m_name;
}

void FacebookAccount::setName(const QString &name)
{
    m_name = name;
}
