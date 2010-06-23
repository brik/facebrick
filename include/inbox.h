#ifndef INBOX_H
#define INBOX_H

#include <QMainWindow>

namespace Ui {
    class Inbox;
}
class NewsFeedModel;
class FacebookAccountModel;
class FBError;
class FBSession;
class FBLoginDialog;

#include "fbconnectglobal.h"

class Inbox : public QMainWindow
{
    Q_OBJECT

public:
    explicit Inbox(QWidget *parent = 0);
    ~Inbox();

protected:
    void changeEvent(QEvent *e);

private slots:
    void fetchMessages();

private:
    Ui::Inbox *m_ui;
    bool m_updatingInbox;
    long long m_lastUpdatedInbox;
};

#endif // INBOX_H
