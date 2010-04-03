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
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_buttonForget_clicked();

    void sessionDidLogin(FBUID);
    void newsFeedLoaded(const QVariant&);
    void newsFeedListClicked(QModelIndex);

    void sessionDidLogout();

    void requestFailedWithFacebookError ( const FBError& aError );

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow * const m_ui;

    FBSession * const m_fbSession;
    FBLoginDialog *m_fbLoginDialog;
    NewsFeedModel * const m_newsFeedModel;
    FacebookAccountModel * const m_facebookAccountModel;
};

#endif // MAINWINDOW_H
