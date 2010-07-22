#include "desktopwidget.h"
#include "ui_desktopwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>

#include "newsfeed.h"
#include "facebrick.h"
#include "newsfeeddelegate.h"
#include "newsfeedmodel.h"
#include "fberror.h"

DesktopWidget::DesktopWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DesktopWidget)
{
    m_ui->setupUi(this);

    m_ui->postsListView->setStyleSheet("background-color: rgba( 255, 255, 255, 0% );");

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_X11NetWmWindowTypeDialog);

    setWindowFlags(windowFlags() | Qt::Dialog);

    Atom window_type = XInternAtom (QX11Info::display(), "_NET_WM_WINDOW_TYPE", False);
    Atom hildonwinType = XInternAtom (QX11Info::display(), "_HILDON_WM_WINDOW_TYPE_HOME_APPLET", False);
    XChangeProperty(QX11Info::display(), winId(), window_type, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &hildonwinType, 1);

    m_ui->postsListView->setModel(FaceBrick::instance()->m_newsFeedModel);

    NewsFeedDelegate *delegate = new NewsFeedDelegate(this);
    delegate->setWidgetFontSize(-4);
    m_ui->postsListView->setItemDelegate(delegate);

    connect(m_ui->downButton, SIGNAL(clicked()), this, SLOT(onDownButtonClicked()));
    connect(m_ui->upButton, SIGNAL(clicked()), this, SLOT(onUpButtonClicked()));
    connect(m_ui->refreshButton, SIGNAL(clicked()), this, SLOT(onRefreshButtonClicked()));

    connect(NewsFeed::instance(), SIGNAL(newsFeedLoadingErrorSignal()), this, SLOT(newsFeedRefreshError()));
    connect(NewsFeed::instance(), SIGNAL(newsFeedLoaded()), this, SLOT(newsFeedLoaded()));

    timerEvent(0);
    startTimer(4000);
}

DesktopWidget::~DesktopWidget()
{
    delete m_ui;
}

void DesktopWidget::onDownButtonClicked()
{
    QModelIndex index = m_ui->postsListView->currentIndex();

    if (index.row() < m_ui->postsListView->model()->rowCount(index) - 1)
    {
        index = index.sibling(index.row() + 1, index.column());

        m_ui->postsListView->scrollTo(index, QAbstractItemView::EnsureVisible);
        m_ui->postsListView->setCurrentIndex(index);
    }    
}

void DesktopWidget::onUpButtonClicked()
{
    QModelIndex index = m_ui->postsListView->currentIndex();

    if (index.row() > 0)
    {
        index = index.sibling(index.row() - 1, index.column());
        qDebug() << "Index: " + QString::number(index.row());
    }

    m_ui->postsListView->scrollTo(index, QAbstractItemView::EnsureVisible);
    m_ui->postsListView->setCurrentIndex(index);
}

void DesktopWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DesktopWidget::paintEvent ( QPaintEvent * event ){
    QPainter painter(this);
    painter.fillRect(event->rect(),QColor ( 0,0,0,200));
}

void DesktopWidget::onRefreshButtonClicked()
{
    NewsFeed::instance()->fetchNewsFeed();
    m_ui->refreshButton->setEnabled(false);
}

void DesktopWidget::newsFeedRefreshError()
{
    m_ui->refreshButton->setEnabled(true);
}

void DesktopWidget::newsFeedLoaded()
{
    m_ui->refreshButton->setEnabled(true);
}
