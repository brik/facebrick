/*
 * Copyright (C) 2010 Kamilla Bremeraunet <kamilla@bremeraunet.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
    void updateInterval();

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
signals:
    void newsFeedLoadingErrorSignal();
    void newsFeedLoaded();
    void newsFeedLoading();
};

#endif // NEWSFEED_H
