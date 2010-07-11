#ifndef NEWSFEED_H
#define NEWSFEED_H

#include "fbconnectglobal.h"
#include "QTimer"

class FacebookAccountModel;
class FBError;
class FBSession;
class FBLoginDialog;

class NewsFeed : public QObject
{
    Q_OBJECT
public:
    static NewsFeed *instance(QWidget *);
    static NewsFeed *instance();

public slots:
    /** Generic error handler
      * @param error The error that occurred
      */
    void requestFailedWithFacebookError(const FBError& error, bool handled = false);
    void fetchNewsFeed();

private slots:

    void newsFeedLoadingError(const FBError &error);
    void newsFeedLoaded(const QVariant&);

private:
    NewsFeed(QWidget *parent = 0);

    /** Called when a request to elevate permissions fails
      * @param error Why the request to elevate permissions failed
      */
    void permissionRequestFailedWithError(const FBError &error);

    /** Called when request to elevate permissions succeeds
      */
    void permissionGranted();

    /** Called when request to elevate permissions is denied or cancelled
      */
    void permissionDeniedOrCancelled();

    bool m_updatingNewsFeed;

    long long m_lastUpdatedNewsFeed;

    QTimer *m_newsFeedRefreshTimer;
};

#endif // NEWSFEED_H
