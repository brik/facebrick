#ifndef GETCONNECTION_H
#define GETCONNECTION_H

#include <QObject>
#include <QImage>
#include "qfacebook.h"
#include "qfacebookreply.h"

class GetConnection : public QObject
{
    Q_OBJECT
public:
    explicit GetConnection(QObject *parent = 0, QString token = "");

signals:
    void requestDidLoad (const QVariant& aResult);
    void commentsLoaded (const QVariant& aResult);
    void imageLoaded(const QImage& image);

public slots:
    void load(QString id, QString type);
    void getPicture(QString id);
    void GetComments(QString postId);

private slots:
    void onDataReady();
    void onPictureReady();
    void onCommentsReady();

private:
    QFacebookReply *m_reply;
    QFacebook *m_qFacebook;
};

#endif // GETCONNECTION_H
