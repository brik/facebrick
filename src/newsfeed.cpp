#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QWidget>
#include <QList>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "newsfeed.h"

#include "fbrequest.h"
#include "fberror.h"
#include "fblogindialog.h"
#include "fbsession.h"
#include "fbpermissiondialog.h"
#include "facebrick.h"
#include "facebookaccountmodel.h"
#include "facebookaccount.h"

#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"
#include "newsfeedpost.h"

static NewsFeed *feed = NULL;

NewsFeed *NewsFeed::instance(QWidget *parent)
{
    feed = new NewsFeed(parent);
    return feed;
}

NewsFeed *NewsFeed::instance()
{
    Q_ASSERT(feed);

    return feed;
}

NewsFeed::NewsFeed(QWidget *parent) :
    QObject(0),
    m_updatingNewsFeed(false),
    m_lastUpdatedNewsFeed(0),
    m_newsFeedRefreshTimer(new QTimer(parent))
{
    // Add timer, get timer interval from settings

    fetchNewsFeed();

}

void NewsFeed::fetchNewsFeed()
{
    // Lock
    if (m_updatingNewsFeed) {
        qDebug() << "fetchNewsFeed: Already updating...";
        return;
    }

    m_updatingNewsFeed = true;
      /*
       This needs to be done in a slot on the mainwindow
            #ifdef Q_WS_MAEMO_5
                setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
            #endif*/

    FBRequest* request = FBRequest::request();
    Dictionary params;

    // Query to fetch messages
    QString queryOne = "SELECT folder_id, name, unread_count FROM mailbox_folder WHERE viewer_id=" + QString::number(FaceBrick::instance()->session()->uid());

    if (m_lastUpdatedNewsFeed != 0) {
        // Fetch all posts newer than the ones we have now
        queryOne += " AND created_time > " + QString::number(m_lastUpdatedNewsFeed);
    }

    QString fql = "{\"query1\":\"" + queryOne + "\"}";
    params["queries"] = fql;

    qDebug() << "fetchMessages: Sending " << fql;

    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(newsFeedLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(newsFeedLoadingError(FBError)));

    request->call("facebook.fql.multiquery",params);
}

void NewsFeed::newsFeedLoadingError(const FBError &error)
{/*
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    QMaemo5InformationBox::information(this, tr("Error loading newsfeed: %1 (%2)").arg(error.code()).arg(error.description()));
#endif*/
    m_updatingNewsFeed = false;
    requestFailedWithFacebookError(error, true);
}

void NewsFeed::newsFeedLoaded(const QVariant &container)
{
    connect (request, SIGNAL(requestDidLoad(QVariant)), this, SLOT(inboxLoaded(QVariant)));
    connect (request, SIGNAL(requestFailedWithFacebookError(FBError)), this, SLOT(inboxLoadingError(FBError)));
    request->call("facebook.fql.multiquery",params);
}
