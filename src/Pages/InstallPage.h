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
#pragma once

#include <QTextEdit>
#include <QThread>

#include "Page.h"

namespace PWTI {
    class InstallPage final: public Page {
        Q_OBJECT

    private:
        QTextEdit *logTx = nullptr;
        QThread *installThread = nullptr;

        bool installed = false;

        void install();

    public:
        explicit InstallPage(const QSharedPointer<installData> &installData);
        ~InstallPage() override;

    private slots:
        void onInstallResultReady(bool res) const;
        void onInstallLogSent(const QString &msg) const;

    protected slots:
        void onNextBtnClicked() override;

    signals:
        void doInstall(const QSharedPointer<installData> &data);
    };
}
