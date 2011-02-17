/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebFrame>
#include <QFile>
#include <QDesktopServices>
#include <QResizeEvent>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QPropertyAnimation>

#include "fbdialog.h"
#include "fberror.h"

static const QString kDefaultTitle = "Connect to Facebook";
static const QString kStringBoundary = "3i2ndDfv2rTHiSisAbouNdArYfORhtTPEefj3q2f";
static const QSize kDefaultDialogSize = QSize(640,320);

static FBCustomNetworkAccessManager namanager;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Used to handle fbconnect:// scheme !

QNetworkReply* FBCustomNetworkAccessManager::createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData )
{
    if (iObserver)
    {
        const QUrl url ( req.url() );
        iObserver->shouldLoadUrl(url);
    }

    return QNetworkAccessManager::createRequest( op, req, outgoingData);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

FBDialog::FBDialog() : iWebView ( this  ), iObserver ( NULL ), iIgnorePageLoadCompleteEvent( false )
{
    createControls();
}

void FBDialog::setObserver( FBDialogObserver* aObserver )
{
    iObserver=aObserver;
}


void FBDialog::createControls()
{
    setFixedSize(kDefaultDialogSize);
    setWindowTitle(kDefaultTitle);
    namanager.setObserver(this);
    iWebView.page()->setNetworkAccessManager(&namanager);

    iWebView.page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect( iWebView.page(), SIGNAL(linkClicked(const QUrl &)),
                    this, SLOT(linkClicked(const QUrl &)));

    connect ( iWebView.page(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    connect ( iWebView.page(), SIGNAL(loadStarted()), this, SLOT ( loadStarted()));
}

void FBDialog::placeControls(const QSize& /*aSize*/)
{
    iWebView.setGeometry(this->rect());
}

QString FBDialog::generateURL( const QString& aUrl, const QHash<QString, QString>& aParams) const
{
    QString url ( aUrl );

    QStringList pairs;
    QHashIterator<QString, QString> i(aParams);

    while (i.hasNext()) {
        i.next();

        QUrl url (i.value());
        QString pair = i.key() + "=" + url.toEncoded();
        pairs << pair.toUtf8();
    }

    if (pairs.count())
    {
        url = url + "?" + pairs.join("&");
    }

    return url;

}

QByteArray FBDialog::generatePostBody (const QHash<QString, QString>& aParams) const
{
    QByteArray body;

    if (!aParams.count())
        return body;


    QString endLine = "\r\n--" + kStringBoundary + "\r\n", kStringBoundary;

    QString tmp = "--" + kStringBoundary + "\r\n";
    body.append(tmp);


    QHashIterator<QString, QString> i(aParams);
    while (i.hasNext()) {
        i.next();

        tmp = "Content-Disposition: form-data; name=\"" + i.key().toUtf8() + "\"\r\n\r\n" ;
        body.append(tmp);
        body.append(i.value().toUtf8());
        body.append(endLine);
    }

    return body;
}

void FBDialog::postDismissCleanup()
{
    accept();
    // could also be reject()?
}

void FBDialog::dismiss (bool /*aAnimated*/) {
    dialogWillDisappear();
    iLoadingUrl.clear();

    //todo: do some animations if aAnimated == true !
    postDismissCleanup();
}


void FBDialog::dismissWithSuccess( bool aSuccess, bool aAnimated)
{
  if (aSuccess) {
        emit dialogDidSucceed();
  } else {
        emit dialogDidCancel();
  }

  dismiss(aAnimated);
}

void FBDialog::dismissWithError (const FBError& aError, bool aAnimated)
{
  emit dialogDidFailWithError( aError );
  dismiss(aAnimated);
}

void FBDialog::cancel()
{}

void FBDialog::load() {}

void FBDialog::show()
{
    load();
    dialogWillAppear();

    int result = exec();
    qDebug() << result;
}

void FBDialog::loadURL(const QString& aUrl, QNetworkAccessManager::Operation aMethod, const QHash<QString, QString>& aGetParams, const QHash<QString, QString>&  aPostParams)
{
    iIgnorePageLoadCompleteEvent = false;

    QNetworkCookieJar* cookieJar = iWebView.page()->networkAccessManager()->cookieJar();
    QByteArray body;

    iLoadingUrl = generateURL(aUrl, aGetParams);

    // This "test cookie" is required by login.php, or it complains that you need to enable JavaScript
    QNetworkCookie testCookie ("test_cookie", "1");
    testCookie.setDomain ( ".facebook.com" );
    testCookie.setPath ( "/" );

    QList<QNetworkCookie> cookieList;
    cookieList.append(testCookie);

    cookieJar->setCookiesFromUrl ( cookieList, QUrl(iLoadingUrl) );

    QUrl url (iLoadingUrl);
    QNetworkRequest request(url);

#if QT_VERSION >= 0x040600
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
#endif

    if (aMethod == QNetworkAccessManager::PostOperation)
    {
        const QString contentType = "multipart/form-data; boundary=" + kStringBoundary;
        request.setHeader (QNetworkRequest::ContentTypeHeader, contentType);
        body = generatePostBody (aPostParams);
    }

    iWebView.load ( request, aMethod, body);

    qDebug() << "Loading url: " << iLoadingUrl;
}

void FBDialog::dialogWillAppear() {}

void FBDialog::dialogWillDisappear() {}

void FBDialog::dialogDidSucceed (const QUrl& /*aUrl*/) {
  dismissWithSuccess(true,true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FBDialog::resizeEvent ( QResizeEvent * event )
{
    placeControls(event->size());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FBDialog::linkClicked ( const QUrl & url )
 {
    if (iObserver && iObserver->shouldOpenLinkInExternalBrowser(url))
    {
        qDebug() << "Opening the link in external browser: " << url;
        QDesktopServices::openUrl(url);
    }
    else
    {
        qDebug() << "Loading the url: " <<  url;
        iWebView.load(url);
    }
}

void FBDialog::loadStarted()
{
    qDebug() << "Load started: " << iWebView.url();

    qDebug() << iWebView.url().hasQueryItem("code");
    //QString code = iWebView.url().queryItemValue("code");
    //qDebug() << code;
}

void FBDialog::loadFinished ( bool ok )
{
    qDebug() << "Load " << (ok ? "" : "un") << "successfull for: " << iWebView.url();

    checkForCode(iWebView.url());

    if (iWebView.url().hasQueryItem("code"))
        iIgnorePageLoadCompleteEvent = true;

    if (ok)
    {
        setWindowTitle( iWebView.page()->mainFrame()->title() );
    }
    else
    {
        if (iIgnorePageLoadCompleteEvent)
            return;

 //       FBError err;
   //     dismissWithError(err, true);
    }
}

void FBDialog::shouldLoadUrl ( const QUrl& aUrl )
{
    if (aUrl.hasFragment()) {
        qDebug() << "LOL";
        dialogDidSucceed(aUrl);
    }
    if (aUrl.scheme().compare("access_token") == 0)
    {
        const QString encodedPath ( aUrl.encodedPath());
        if (encodedPath.compare("cancel")==0)
        {
            dismissWithSuccess(false, true);
        }
        else
        {
            iIgnorePageLoadCompleteEvent = true;
            dialogDidSucceed(aUrl);
        }
    }
}

void FBDialog::checkForCode(QUrl aUrl)
{
    if (aUrl.hasQueryItem("code"))
    {
        qDebug() << "SUCC";

        QString code = aUrl.queryItemValue("code");
        qDebug() << code;

        //emit codeFound(aUrl);
    }
}
