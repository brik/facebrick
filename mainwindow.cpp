#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iFBSession = FBSession::sessionForApplication("b4db7267221ecf6333e60c1213d724cb", "f054071e18bd026c0b2e2f97ff68a7c5", QString());
    connect (iFBSession,SIGNAL(sessionDidLogin(FBSession*,FBUID)), this, SLOT(sessionDidLogin(FBSession*,FBUID)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    iLoginDialog = new FBLoginDialog();
    iLoginDialog->setFixedSize(640,320);
    iLoginDialog->show();
}

void MainWindow::sessionDidLogin(FBSession*,FBUID)
{
    qDebug() << "yipee";

}
