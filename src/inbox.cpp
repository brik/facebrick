#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "inbox.h"
#include "ui_inbox.h"
#include "newsfeedmodel.h"

Inbox::Inbox(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::Inbox),
    m_newsFeedModel(new NewsFeedModel(this, true)),
    m_updatingInbox(false),
    m_lastUpdatedInbox(0)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
    m_ui->setupUi(this);

    fetchMessages();
}

Inbox::~Inbox()
{
    delete m_ui;
}

void Inbox::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
