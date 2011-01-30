#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QImage>

#include "getconnection.h"
#include "facebrick.h"

GetConnection::GetConnection(QObject *parent, QString token) :
    QObject(parent),
    m_reply(0),
    m_qFacebook(new QFacebook(token, this))
{
}

void GetConnection::load(QString id, QString type)
{
    m_reply = m_qFacebook->getConnections(id, type);
    if (m_reply)
        connect(m_reply, SIGNAL(finished()), this, SLOT(onDataReady()));
}

void GetConnection::getPicture(QString id)
{
    if (!m_qFacebook)
        return;
    QImage m_picture = QImage();

    QNetworkReply *picReply;

    // Requesting the picture using simple get
    picReply = m_qFacebook->get(QUrl("http://graph.facebook.com/" + id + "/picture"));
    if (picReply)
        connect(picReply, SIGNAL(finished()), this, SLOT(onPictureReady()));
}

void GetConnection::GetComments(QString postId)
{
    qDebug() << postId;
    QUrl postUrl = "graph.facebook.com/" + postId + "?access_token=" + FaceBrick::instance()->m_token;
    qDebug() << postUrl;
    QNetworkReply *reply = m_qFacebook->get(postUrl);
    qDebug() << "lol";

    if (reply)
        connect(reply, SIGNAL(finished()), this, SLOT(onCommentsReady()));

}

void GetConnection::onDataReady()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        qDebug() << "Simple get fail!";
        m_reply->deleteLater();
        m_reply = 0;
        return;
    }

    QVariant parsedData = m_reply->data();

    emit requestDidLoad(parsedData);
}

void GetConnection::onCommentsReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Simple get fail!";
        reply->deleteLater();
        reply = 0;
        return;
    }

    QVariant parsedData = reply->readAll();
    qDebug() << parsedData;

    emit commentsLoaded(parsedData);
}

void GetConnection::onPictureReady()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data;
    QUrl redir;
    if (!reply) {
        qDebug() << "Cast to qnetwork reply fail!";
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Picture get fail!";
        reply->deleteLater();
        reply = 0;

        return;
    }

    redir = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    // Facebook is redirecting the picture request
    if(!redir.isEmpty()) {
        qDebug() << "Redirecting picture request to " + redir.toString();
        reply = m_qFacebook->get(redir);
        if (reply)
            connect(reply, SIGNAL(finished()), this, SLOT(onPictureReady()));
        // not finished yet!
        return;
    }

    qDebug() << "######### Parsed picture:";
    data = reply->readAll();
    qDebug() << data;
    QImage m_picture = QImage::fromData(data, "JPEG");

    emit imageLoaded(m_picture);
}
