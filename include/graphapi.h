#ifndef GRAPHAPI_H
#define GRAPHAPI_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "qfacebookreply.h"

class QFacebookReply;

class GraphAPI : public QObject
{
public:
    enum HttpMethod {
        GET,
        POST,
        PUT,
        DELETE
    };

    GraphAPI(QString accessToken, QObject *parent = 0);

    QFacebookReply *getObject(QString id);
    QFacebookReply *getConnections(QString id, QString connectionName);
    QFacebookReply *putObject(QString id, QString connection,
                       QByteArray data = QByteArray());
    QFacebookReply *deleteObject(QString id);
    QFacebookReply *request(HttpMethod method, QString path,
                            QByteArray postArgs = QByteArray());

    QNetworkReply *get(QUrl url);
    QNetworkReply *put(QUrl url, QByteArray data);
    QNetworkReply *del(QUrl url);
    QNetworkReply *request(HttpMethod method, QUrl url,
                            QByteArray postArgs = QByteArray());

private:
    QString m_accessToken;
    QNetworkAccessManager *m_networkManager;
};

#endif // GRAPHAPI_H
