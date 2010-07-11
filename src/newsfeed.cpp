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

    // Query to fetch news posts
    QString queryOne = "SELECT post_id, actor_id, target_id, message, permalink, created_time, likes, attachment FROM stream WHERE filter_key in (SELECT filter_key FROM stream_filter WHERE uid=" + QString::number(FaceBrick::instance()->session()->uid()) + " AND type='newsfeed') AND is_hidden = 0";

    if (m_lastUpdatedNewsFeed != 0) {
        // Fetch all posts newer than the ones we have now
        queryOne += " AND created_time > " + QString::number(m_lastUpdatedNewsFeed);
    }

    // Fetch all people that made these posts, combine them into a single FQL multiquery
    QString queryTwo = "SELECT id, name, url, pic_square FROM profile WHERE id IN (SELECT actor_id FROM #query1)";
    QString fql = "{\"query1\":\"" + queryOne + "\",\"queryTwo\":\"" + queryTwo + "\"}";
    params["queries"] = fql;

    qDebug() << "fetchNewsFeed: Sending " << fql;

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
/* This needs to be moved to a slot
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif*/
    m_updatingNewsFeed = false;

    if (container.type() == QVariant::List) {
        QVariantList list = container.toList();

        // Item #0 will be our result set on news items
        foreach (const QVariant &newsFeedPostHash, list.at(0).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedPostData = newsFeedPostHash.toHash();

            // Fetch (or create) the account that made this newsfeed post
            FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedPostData["actor_id"].toLongLong());
            Q_ASSERT(account);

            // Create a new newsfeed post
            NewsFeedPost *np = new NewsFeedPost(FaceBrick::instance()->m_newsFeedModel,
                                                account,
                                                newsFeedPostData["post_id"].toString(),
                                                newsFeedPostData["created_time"].toLongLong(),
                                                newsFeedPostData["permalink"].toString(),
                                                newsFeedPostData["message"].toString());
            Q_ASSERT(np);

            // Seed it into the model
            FaceBrick::instance()->m_newsFeedModel->insertNewsItem(np);

            // Update our 'recent posts' block badger.
            if (np->createdTime() > m_lastUpdatedNewsFeed)
                m_lastUpdatedNewsFeed = np->createdTime();

            // Process like info too.
            bool iLikeThis = newsFeedPostData["likes"].toHash()["user_likes"].toString() == "1";
            np->setILikeThis(iLikeThis);

            // *breathe deeply* ok, and now let's try manage attachments
            QHash<QString, QVariant> attachmentHash = newsFeedPostData["attachment"].toHash();

            // Facebook... why not just *not* send attachment data if there isn't one?
            if (attachmentHash.count() == 1)
                continue;

            // Ignore "sent from my mobile" - there should be a better way to do this
            if (attachmentHash["href"].toString() == "http://www.facebook.com")
                continue;

            // yes, apparantly this can happen!
            if (attachmentHash["media"].toList().count() == 0)
                continue;

            // GOD DAMMIT I HATE THIS BOXING.
            QHash<QString, QVariant> mediaInfo = attachmentHash["media"].toList().at(0).toHash();
            qDebug() << mediaInfo;

            np->setHasAttachment(true);
            np->setDescription(attachmentHash["description"].toString());
            np->setAttachmentName(mediaInfo["name"].toString());
            // TODO: href?
            np->setThumbnail(mediaInfo["src"].toString());
        }

        foreach (const QVariant &newsFeedUserHash, list.at(1).toHash().begin().value().toList()) {
            QHash<QString, QVariant> newsFeedUserData = newsFeedUserHash.toHash();

            // Get (or create - though this should have already been done above) the account
            FacebookAccount *account = FacebookAccountModel::instance()->account(newsFeedUserData["id"].toLongLong());
            Q_ASSERT(account);

            account->setName(newsFeedUserData["name"].toString());
            account->setAvatar(newsFeedUserData["pic_square"].toString());
        }
    }

    sender()->deleteLater();
}
