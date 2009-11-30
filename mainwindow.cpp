#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iFBSession = FBSession::sessionForApplication("PUT SOMETHING HERE", "PUT SOMETHING HERE", QString());
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
