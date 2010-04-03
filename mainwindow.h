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
