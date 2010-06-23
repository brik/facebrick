#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "inbox.h"
#include "ui_inbox.h"

Inbox::Inbox(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::Inbox)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
    m_ui->setupUi(this);
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
