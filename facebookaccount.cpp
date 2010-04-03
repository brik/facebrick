#include <QUrl>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "fbconnectglobal.h"

#include "facebookaccount.h"

FacebookAccount::FacebookAccount(QObject *parent, FBUID uid)
    : QObject(parent),
    m_uid(uid)
{
    connect(&m_networkaccessmgr, SIGNAL(finished(QNetworkReply*)), SLOT(onAvatarDownloaded(QNetworkReply*)));
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

const QImage &FacebookAccount::avatar() const
{
    return m_avatar;
}

void FacebookAccount::setAvatar(const QUrl &url)
{
    QNetworkRequest request(url);
    m_networkaccessmgr.get(request);
}

void FacebookAccount::onAvatarDownloaded(QNetworkReply *reply)
{
    qDebug() << "Avatar recieved for " << m_uid;
    m_avatar = QImage::fromData(reply->readAll());

    reply->deleteLater();
}
