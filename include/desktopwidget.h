#ifndef DESKTOPWIDGET_H
#define DESKTOPWIDGET_H

#include <QWidget>

namespace Ui {
    class DesktopWidget;
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

private:
    Ui::DesktopWidget *m_ui;
    //void timerEvent(QTimerEvent *);

};

#endif // DESKTOPWIDGET_H
