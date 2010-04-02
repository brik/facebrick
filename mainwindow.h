#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class NewsFeedModel;
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
    void friendsRequestLoaded(const QVariant&);
    void sessionDidLogout();

    void requestFailedWithFacebookError ( const FBError& aError );

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    FBSession*      iFBSession;
    FBLoginDialog*  iLoginDialog;
    NewsFeedModel * const m_newsFeedModel;
};

#endif // MAINWINDOW_H
