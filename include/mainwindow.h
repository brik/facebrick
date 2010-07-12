/*
 * Copyright (C) 2010 Robin Burchell <robin.burchell@collabora.co.uk>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
    class MainWindow;
}

class NewsFeedModel;
class FacebookAccountModel;
class FBError;
class FBSession;
class FBLoginDialog;

#include "fbconnectglobal.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent);
    ~MainWindow();

public slots:
    /** Generic error handler
      * @param error The error that occurred
      */
    void requestFailedWithFacebookError(const FBError& error, bool handled = false);

private slots:
    void onLogoutMenuAction();
    void onSettingsMenuAction();
    void onInboxMenuAction();

    void sendStatusUpdate();
    void statusUpdateError(const FBError &error);
    void statusUpdated(const QVariant &);

    void fetchNewsFeed();
    void newsFeedLoadingError(const FBError &error);
    void newsFeedLoaded(const QVariant&);
    void newsFeedListClicked(QModelIndex);

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

    void sessionDidLogout();

    void updateInterval();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow * const m_ui;

    NewsFeedModel * const m_newsFeedModel;
    bool m_updatingNewsFeed;
    long long m_lastUpdatedNewsFeed;

    QTimer *m_newsFeedRefreshTimer;
};

#endif // MAINWINDOW_H
