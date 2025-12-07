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
#include <QMessageBox>

#include "FinishPage.h"
#include "Include/InstallServiceThread.h"

namespace PWTI {
    FinishPage::FinishPage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();

        if (installData->installService) {
            startServiceChkb = new QCheckBox("Start PowerTuner service");
            msgLbl = new QLabel();

            startServiceChkb->setChecked(true);

            lyt->addStretch();
            lyt->addWidget(startServiceChkb);
            lyt->addSpacing(6);
            lyt->addWidget(msgLbl);
        }

        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);
    }

    void FinishPage::onExitBtnClicked() const {
        if (data->installService) {
            const bool startSvc = startServiceChkb->isChecked();
            InstallServiceThread *installThd = new InstallServiceThread(QString("%1\\PowerTunerDaemon.exe").arg(data->path), startSvc);

            msgLbl->setText(startSvc ? "starting.." : "installing Windows service..");

            QObject::connect(installThd, &InstallServiceThread::resultReady, this, &FinishPage::onInstallServiceResult);
            QObject::connect(installThd, &InstallServiceThread::finished, installThd, &QObject::deleteLater);

            exitBtn->setEnabled(false);
            installThd->start();
            return;
        }

        Page::onExitBtnClicked();
    }

    void FinishPage::onInstallServiceResult(const QString &res) {
        if (!res.isEmpty())
            QMessageBox::information(this, "PowerTuner installer", res);

        Page::onExitBtnClicked();
    }
}
