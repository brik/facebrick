#ifndef DESKTOPWIDGET_H
#define DESKTOPWIDGET_H

#include <QWidget>
#include <QModelIndex>

#include "fbconnectglobal.h"
namespace Ui {
    class DesktopWidget;
    class FBError;
}

class DesktopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DesktopWidget(QWidget *parent = 0);
    ~DesktopWidget();

protected:
    void changeEvent(QEvent *e);
    virtual void paintEvent(QPaintEvent *);

private slots:
    void onDownButtonClicked();
    void onUpButtonClicked();
    void onRefreshButtonClicked();
    void newsFeedRefreshError();
    void newsFeedLoaded();
    void newsFeedListClicked(QModelIndex index);
    void newsFeedLoading();

private:
    Ui::DesktopWidget *m_ui;
    //void timerEvent(QTimerEvent *);

};

#endif // DESKTOPWIDGET_H
