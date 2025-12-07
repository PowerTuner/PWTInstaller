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

#include <QObject>
#include <QSharedPointer>

#include "../../InstallData.h"

namespace PWTI {
    class InstallWorker final: public QObject {
        Q_OBJECT

    private:
        QSharedPointer<installData> data;

        [[nodiscard]] bool copyFiles(const QString &qrcDir, const QString &basePath);
        [[nodiscard]] quint64 getInstallFilesTotalSize(const QString &qrcDir) const;
        [[nodiscard]] quint64 getEstimatedInstallSize() const;
        [[nodiscard]] bool createStartMenuLink(const QString &exe, const QString &menuPath);
        [[nodiscard]] bool hasEnoughDiskSpace();
        [[nodiscard]] bool createInstallDir();
        [[nodiscard]] bool installBase();
        [[nodiscard]] bool installBaseClient();
        [[nodiscard]] bool installDesktop();
        [[nodiscard]] bool installConsole();
        [[nodiscard]] bool installCli();
        [[nodiscard]] bool installService();
        [[nodiscard]] bool createUninstall();
        void downloadFile(const QString &url, const QString &outPath);
        void linkToStartMenu();
        void addToPath();

    private slots:
        void onDownloadError();
        void onLogSent(const QString &msg);

    public slots:
        void doInstall(const QSharedPointer<installData> &installData);

    signals:
        void resultReady(bool res);
        void logSent(const QString &msg);
        void workerDownloadFile(const QString &url, const QString &outPath);
    };
}
