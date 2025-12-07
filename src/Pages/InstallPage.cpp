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
#include <QScroller>
#include <QScrollBar>

#include "InstallPage.h"
#include "Include/InstallWorker.h"

namespace PWTI {
    InstallPage::InstallPage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();

        logTx = new QTextEdit();

        logTx->setReadOnly(true);
        logTx->append("Press next to start the installation.");
        QScroller::grabGesture(logTx->viewport(), QScroller::LeftMouseButtonGesture);

        lyt->addWidget(logTx);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);
    }

    InstallPage::~InstallPage() {
        if (installThread) {
            installThread->quit();
            installThread->wait();
            delete installThread;
        }
    }

    void InstallPage::install() {
        prevBtn->setVisible(false);
        nextBtn->setVisible(false);
        exitBtn->setVisible(false);
        logTx->clear();

        InstallWorker *iw = new InstallWorker();

        installed = true;
        installThread = new QThread();

        iw->moveToThread(installThread);

        QObject::connect(installThread, &QThread::finished, iw, &QObject::deleteLater);
        QObject::connect(this, &InstallPage::doInstall, iw, &InstallWorker::doInstall);
        QObject::connect(iw, &InstallWorker::resultReady, this, &InstallPage::onInstallResultReady);
        QObject::connect(iw, &InstallWorker::logSent, this, &InstallPage::onInstallLogSent);

        installThread->start();
        emit doInstall(data);
    }

    void InstallPage::onInstallLogSent(const QString &msg) const {
        logTx->append(msg);
    }

    void InstallPage::onInstallResultReady(const bool res) const {
        installThread->quit();

        if (res)
            nextBtn->setVisible(true);
        else
            exitBtn->setVisible(true);

        logTx->verticalScrollBar()->setValue(logTx->verticalScrollBar()->maximum());
    }

    void InstallPage::onNextBtnClicked() {
        if (!installed) {
            install();
            return;
        }

        Page::onNextBtnClicked();
    }
}
