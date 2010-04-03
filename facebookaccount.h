#ifndef FACEBOOKACCOUNT_H
#define FACEBOOKACCOUNT_H

#include <QObject>
#include <QUrl>
#include <QImage>
#include <QNetworkAccessManager>

#include "fbconnectglobal.h"

class QNetworkReply;

class FacebookAccount : public QObject
{
Q_OBJECT
public:
    FacebookAccount(QObject *parent, FBUID uid);

    FBUID uid() const;
    const QString &name() const;
    void setName(const QString &name);
    const QImage &avatar() const;
    void setAvatar(const QUrl &url);
signals:
    void modified();
private slots:
    void onAvatarDownloaded(QNetworkReply *);
private:
    FBUID m_uid;
    QString m_name;
    QImage m_avatar;
    QNetworkAccessManager m_networkaccessmgr;
};

#endif // FACEBOOKACCOUNT_H
