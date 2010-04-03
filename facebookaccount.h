#ifndef FACEBOOKACCOUNT_H
#define FACEBOOKACCOUNT_H

#include <QObject>

#include "fbconnectglobal.h"

class FacebookAccount : public QObject
{
public:
    FacebookAccount(QObject *parent, FBUID uid);

    FBUID uid() const;
    const QString &name() const;
    void setName(const QString &name);
private:
    FBUID m_uid;
    QString m_name;
};

#endif // FACEBOOKACCOUNT_H
