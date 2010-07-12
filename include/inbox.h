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

public slots:
    void requestFailedWithFacebookError(const FBError& error, bool handled = false);

private slots:
    void fetchMessages();
    void inboxLoaded(const QVariant &);
    void inboxLoadingError(const FBError &error);


    /** Called when a request to elevate permissions fails
      * @param error Why the request to elevate permissions failed
      */
    void permissionRequestFailedWithError(const FBError &error);

    /** Called when request to elevate permissions succeeds
      */
    void permissionGranted();

    /** Called when request to elevate permissions is denied or cancelled
      */
    void permissionDeniedOrCancelled();

private:
    Ui::Inbox *m_ui;
    NewsFeedModel * const m_newsFeedModel;

    bool m_updatingInbox;
    long long m_lastUpdatedInbox;


};

#endif // INBOX_H
