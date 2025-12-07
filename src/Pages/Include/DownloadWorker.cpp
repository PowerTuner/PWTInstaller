/*
 * This file is part of PWTInstaller.
 * Copyright (C) 2025 kylon
 *
 * PWTInstaller is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PWTInstaller is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QNetworkReply>
#include <QThread>
#include <QFile>

#include "DownloadWorker.h"

namespace PWTI {
    void DownloadWorker::downloadFile(const QString &url, const QString &outPath) {
        QNetworkRequest req = QNetworkRequest(QUrl(url));
        QNetworkReply *reply;

        netAccess.reset(new QNetworkAccessManager);
        outfilePath = outPath;

        netAccess->setAutoDeleteReplies(true);
        req.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:145.0) Gecko/20100101 Firefox/145.0");

        QObject::connect(netAccess.get(), &QNetworkAccessManager::finished, this, &DownloadWorker::onDownloadFinisched);

        reply = netAccess->get(req);

        QObject::connect(reply, &QNetworkReply::errorOccurred, this, &DownloadWorker::onReplyError);
    }

    void DownloadWorker::onDownloadFinisched(QNetworkReply *reply) {
        if (reply->error() != QNetworkReply::NoError) {
            emit failed();
            QThread::currentThread()->quit();
            return;
        }

        const QByteArray fileContent = reply->readAll();
        QFile out(outfilePath);

        if (!fileContent.startsWith(QByteArray::fromHex("4D5A900003"))) {
            emit failed();
            QThread::currentThread()->quit();
            return;
        }

        if (!out.open(QFile::WriteOnly)) {
            emit failed();
            emit logSent(QString("failed to open file for write: %1").arg(out.errorString()));
            QThread::currentThread()->quit();
            return;
        }

        if (out.write(fileContent) == -1) {
            emit failed();
            emit logSent(QString("failed to write file: %1").arg(out.errorString()));
            QThread::currentThread()->quit();
            return;
        }

        emit logSent(QString("writing %1..").arg(outfilePath));

        out.flush();
        out.close();
        QThread::currentThread()->quit();
    }

    void DownloadWorker::onReplyError(const QNetworkReply::NetworkError code) {
        emit failed();
        emit logSent(QString("download failed, code: %1").arg(code));
        QThread::currentThread()->quit();
    }
}
