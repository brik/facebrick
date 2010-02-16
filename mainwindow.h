#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fbsession.h"
#include "fblogindialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_buttonForget_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void sessionDidLogin(FBUID);
    void requestDidLoad(const QVariant&);
    void sessionDidLogout();

    void requestFailedWithFacebookError ( const FBError& aError );

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    FBSession*      iFBSession;
    FBLoginDialog*  iLoginDialog;
};

#endif // MAINWINDOW_H
