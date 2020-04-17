/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

// Module
#include "network.h"
// C++/Qt
#include <QtWidgets>
#include <QUrl>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QtNetwork>
#include <QJsonDocument>

HttpNetwork::HttpNetwork(QObject *parent)
    : QObject(parent)
    , isConnected_(false)
    , httpRequestAborted(false)

{
    connect(&qnam, &QNetworkAccessManager::authenticationRequired,
            this, &HttpNetwork::slotAuthenticationRequired);
#ifndef QT_NO_SSL
    connect(&qnam, &QNetworkAccessManager::sslErrors,
            this, &HttpNetwork::sslErrors);
#endif

}

QNetworkReply* HttpNetwork::sending(QNetworkRequest & request, QByteArray data)
{
   // connect(qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkData(QNetworkReply*)));
    // connect(qnam,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(handleSSLErrors(QNetworkReply*)));

    // Build your JSON string as usual


    // QByteArray jsn = "{\"patrols\":[{\"name\": \"Patrol1\",\"type\": \"1\",\"points\":[{\"priority\": \"1\",\"position\":{\"x\": \"411\",\"y\": \"530\",\"z\": \"0\"}}]}]}";
    // QByteArray jsn = data.simplified();
    //
    // QJsonDocument jdoc = QJsonDocument::fromJson(jsn);
    QByteArray jsonString = data.simplified();
    // if(!jdoc.isNull())
    // {
    //     QJsonObject json = jdoc.object();
    //     QJsonDocument doc(json);
    //     jsonString = doc.toJson();
    // }
    // jsonString = jsn;

    // For your "Content-Length" header
    QByteArray postDataSize = QByteArray::number(jsonString.size());

    // Time for building your request
    // QUrl serviceURL("http://192.168.1.20:40054/patrol");
   //  QNetworkRequest request(serviceURL);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("User-Agent", "My app name v0.1");
    request.setRawHeader("X-Custom-User-Agent", "My app name v0.1");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", postDataSize);

    // Use QNetworkReply * QNetworkAccessManager::post(const QNetworkRequest & request, const QByteArray & data); to send your request. Qt will rearrange everything correctly.
    QNetworkReply * reply = qnam.post(request, jsonString);
    return reply;
}

bool HttpNetwork::sendRequest(QString const& urlSpec, HttpNetwork::E_ACTION action, QByteArray data)
{
    if (urlSpec.isEmpty())
    {
        emit invalidUrl();
        return false;
    }

    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid())
    {
        emit invalidUrl();
        return false;
    }
    // schedule the request
    url = newUrl;
    // startRequest
    httpRequestAborted = false;
    QNetworkReply* reply = nullptr;
    QNetworkRequest request = QNetworkRequest(url);

    switch(action)
    {
    case HttpNetwork::E_ACTION::GET :
         reply = qnam.get(request);
    break;
    case HttpNetwork::E_ACTION::POST :
        reply = sending(request, data);
    break;
    default:
        qInfo() << "Unknown action " ;
        break; // do nothing
    }

    if(nullptr == reply)
    {
        qWarning() << "HttpNetwork::SendRequest - reply nullptr  O get 1 post : " << static_cast<int>(action);
        return false;
    }


    connect(reply, &QNetworkReply::finished, this, &HttpNetwork::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &HttpNetwork::httpReadyRead);

    // connect(reply, &QNetworkReply::finished, this, &HttpNetwork::httpFinished);
    // connect(&qnam, SIGNAL(finished(QNetworkReply*)),this,SLOT(httpFinished(QNetworkReply*)));
    // connect(reply, &QIODevice::readyRead, this, &HttpNetwork::httpReadyRead);
    return true;
}

bool HttpNetwork::Get(QString const& urlSpec)
{
    return sendRequest(urlSpec,E_ACTION::GET);
}

bool HttpNetwork::Post(QString const& urlSpec,QByteArray data)
{
    return sendRequest(urlSpec,E_ACTION::POST,data);
}


void HttpNetwork::httpFinished(/*QNetworkReply* reply*/)
{
    QNetworkReply* reply;
    if(nullptr == (reply = dynamic_cast<QNetworkReply*>(sender())))
    {
        return;
    }

    if (httpRequestAborted)
    {
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!redirectionTarget.isNull())
    {
        //const QString requestType =  path.mid(path.lastIndexOf("/"));
        qDebug() << "Redirection Target setted ";
    }
    if (reply->error())
    {
        qDebug() << "Redirection Target setted " << reply->errorString();

        if(QNetworkReply::ConnectionRefusedError == reply->error())
        {
            if(isConnected_)
            {
                isConnected_ = false;
                emit onConnectionLost();
            }
        }
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }
    else
    {
        auto iter = buffer_.find(reply);
        if(iter != buffer_.end())
        {
            QUrl url = reply->request().url();
            emit onDataAvailable(buffer_[reply],url);
            buffer_.erase(iter);
        }
    }

    reply->deleteLater();
    reply = Q_NULLPTR;
}

void HttpNetwork::httpReadyRead()
{
    // this slot gets called every time the QReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QReply
    QNetworkReply* reply;
    if(nullptr != (reply = dynamic_cast<QNetworkReply*>(sender())))
    {
        if(!isConnected_)
        {
            isConnected_ = true;
            emit onConnected();
        }
        buffer_[reply] = buffer_[reply] + reply->readAll();
//        QUrl url = reply->request().url();
//        auto path = reply->request().url().path();
//        const QString requestType =  path.mid(path.lastIndexOf("/"));
//        const QByteArray data = reply->readAll();
//        emit onDataAvailable(data,requestType);
    }
}

void HttpNetwork::slotAuthenticationRequired(QNetworkReply*,QAuthenticator *authenticator)
{
}

#ifndef QT_NO_SSL
void HttpNetwork::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors)
    {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }
}
#endif
