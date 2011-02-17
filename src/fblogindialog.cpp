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

#include "fblogindialog.h"
#include "facebrick.h"

#include <QDebug>

FBLogin::FBLogin(QString clientId)
{
    m_clientId = clientId;
}

static const QString kLoginURL = "https://www.facebook.com/dialog/oauth";

void FBLogin::loadLoginPage()
{
    Dictionary getParams, postParams;

    getParams["display"] = "touch";
    getParams["client_id"] = m_clientId;
    getParams["redirect_uri"] = "http://www.facebook.com/apps/application.php?id=110513258970087";
    getParams["scope"] = "read_stream";
    getParams["response_type"] = "token";

    loadURL(kLoginURL,QNetworkAccessManager::GetOperation, getParams, postParams);

}
///////////////////////////////////////////////////////////////////////////////////////////////////
// FBDialog

void FBLogin::load()
{
    loadLoginPage();
}

void FBLogin::dialogWillDisappear()
{
    qDebug() << "dialog gone";
}

void FBLogin::dialogDidSucceed(const QUrl& aUrl)
{
        const QString authToken = "access_token=";
    const QString url ( aUrl.toString() );

    int start = url.indexOf(authToken);
    qDebug() << start;
    if (start != -1)
    {
        QString token;
        int end = url.indexOf("&", start);
        int delta = start + authToken.size();
        if (end != -1)
        {
            token = url.mid(delta, end - delta );
        }
        else
        {
            // -1 because delta assumes & character which is in this case missing
            token = url.right(delta - 1);
        }
        qDebug() << token;

        // TODO: needs changing
        FaceBrick::instance()->setToken(token);
        dismiss(true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// slots for signals from FBRequest

void FBLogin::requestDidLoad(const QVariant& aResult)
{
    qDebug() << "::::::::::::::::::::RESULT::::::::::::::::::::::::::";
    qDebug() << aResult;
    /*bool conversionError = false;
    QVariantHash object = aResult.toHash();
    QVariant ui = object.value("uid");
    FBUID uid = object.value("uid").toULongLong(&conversionError);
    QString sessionKey = object.value("session_key").toString();
    QString sessionSecret = object.value("secret").toString();

    QVariant ex = object.value("expires");
    uint expires = object.value("expires").toUInt(&conversionError);
    QDateTime expiration; expiration.setTime_t( expires );

    iSession->beginSession(uid,sessionKey, sessionSecret,expiration);
    iSession->resume();

    dismissWithSuccess(true, true);*/
}

void FBLogin::requestFailedWithFacebookError (const FBError& aCode )
{
    dismissWithError(aCode, true);
}

void FBLogin::requestFailedWithNetworkError( QNetworkReply::NetworkError aCode )
{
    qDebug() << "error  network";
    //dismissWithError(aCode, true);
}
