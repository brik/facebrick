/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef FB_DIALOG_H_
#define FB_DIALOH_H_

#include <QDialog>
#include <QWebView>

#include "fbconnectglobal.h"

// FORWARD DECLARATIONS
class FBSession;
class FBError;
class QNetworkReply;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FBCustomNetworkAccessManagerObserver
{
public:
    virtual void shouldLoadUrl ( const QUrl& aUrl ) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * Used to handle fbconnect:// scheme !
  */
class FBCustomNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    FBCustomNetworkAccessManager() : iObserver(NULL) {}
    inline void setObserver(FBCustomNetworkAccessManagerObserver* aObserver) { iObserver = aObserver; }

protected:
    QNetworkReply * createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );

    FBCustomNetworkAccessManagerObserver* iObserver;

};

/**
  * Just used to get some questions answered
  */
class QFACEBOOKCONNECTSHARED_EXPORT FBDialogObserver
{
public:
    /**
      * called to ask the client app if the link clicked in the web page be opened in the system browser app or not.
      * @param aUrl the url
      * @return true if you want to open link in the external browser.
      */
    virtual bool shouldOpenLinkInExternalBrowser(const QUrl& aUrl) = 0;
};

/**
  * The base Dialoggg
  */
class QFACEBOOKCONNECTSHARED_EXPORT FBDialog : public QDialog, public FBCustomNetworkAccessManagerObserver
{
    Q_OBJECT

protected:

    /**
    * The session for which the login is taking place.
    */
    FBSession*	iSession;
    QWebView	iWebView;
    QString	iLoadingUrl;
    FBDialogObserver*  iObserver;
    bool        iIgnorePageLoadCompleteEvent;

signals:

    /**
     * Called when the dialog succeeds and is about to be dismissed.
     */
    void dialogDidSucceed ();

    /**
     * Called when the dialog is cancelled and is about to be dismissed.
     */
    void dialogDidCancel();

    /**
     * Called when dialog failed to load due to an error.
     */
    void dialogDidFailWithError ( const FBError& error );

    /**
     * Called when facebook has returned a URL with the code needed to grab access token
     */
    void codeFound(QUrl& aUrl);




private slots:
    void cancel();

    /* slots for signals from QWebView.page() */
    void linkClicked ( const QUrl & url );
    void loadStarted ();
    void loadFinished ( bool ok );

public:
    /**
    * Creates the view but does not display it.
    */
    FBDialog(FBSession* aSession);
    FBDialog();

    /**
      * @param aObserver, used to ask questions from client
      */
    void setObserver( FBDialogObserver* aObserver );


    /**
    * Displays the view with an animation.
    *
    * The view will be added to the top of the current key window.
    */
    void show();

    /** Displays the first page of the dialog.
     *
     * Do not ever call this directly.  It is intended to be overriden by subclasses.
     */
    virtual void load ();

    /**
     * Displays a URL in the dialog.
     */
    void loadURL(const QString& aUrl, QNetworkAccessManager::Operation aMethod, const QHash<QString, QString>& aGetParams,
                 const QHash<QString, QString>&  aPostParams);


    void dismiss(bool aAnimated);

    /**
     * Hides the view and notifies delegates of success or cancellation.
     */
    void dismissWithSuccess (bool aSuccess, bool aAnimated);

    /**
    * Hides the view and notifies delegates of an error.
    */
    void dismissWithError (const FBError& aError, bool aAnimated);

    /**
    * Subclasses may override to perform actions just prior to showing the dialog.
    */
    virtual void dialogWillAppear();

    /**
     * Subclasses may override to perform actions just after the dialog is hidden.
     */
    virtual void dialogWillDisappear();

    /**
     * Subclasses should override to process data returned from the server in a 'fbconnect' url.
     *
     * Implementations must call dismissWithSuccess:YES at some point to hide the dialog.
     */
    virtual void dialogDidSucceed(const QUrl& aUrl);

    QString title() const;
    void setTitle ( const QString& aTitle );

protected:
    /* from FBCustomNetworkAccessManagerObserver */
    void shouldLoadUrl (const QUrl& aUrl );
    void checkForCode(QUrl aUrl);

    QString m_clientId;
private:
    void createControls();
    void placeControls( const QSize & );

    QString generateURL( const QString& aUrl, const QHash<QString, QString>& aParams) const;
    QByteArray generatePostBody (const QHash<QString, QString>& aParams) const;

    void postDismissCleanup();

    /* called on widget resize */
    void resizeEvent ( QResizeEvent * event ) ;

 };
#endif
