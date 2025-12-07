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
#include "pwtWin32/win32Svc.h"
#include "pwtWin32/win32Reg.h"

#include <shellapi.h>
#include <fileapi.h>
#include <ktmw32.h>
#include <QDir>
#include <QFile>
#include <QThread>
#include <QDate>

#include "InstallWorker.h"
#include "DownloadWorker.h"

namespace PWTI {
    bool InstallWorker::hasEnoughDiskSpace() {
        const quint64 installSize = getEstimatedInstallSize();
        ULARGE_INTEGER freeBytes;

        emit logSent(QString("checking free space on disk, required %1..").arg(QLocale().formattedDataSize(installSize, 2, QLocale::DataSizeTraditionalFormat)));

        if (GetDiskFreeSpaceEx(data->path.first(3).toStdWString().c_str(), nullptr, nullptr, &freeBytes) == FALSE) {
            emit logSent(QString("failed to get disk space information, code %1").arg(GetLastError()));
            return false;
        }

        return freeBytes.QuadPart > installSize;
    }

    bool InstallWorker::createInstallDir() {
        const std::wstring args = QString(R"(Add-MpPreference -ExclusionPath \"%1\")").arg(data->path).toStdWString();
        SHELLEXECUTEINFO execInfo {
            .cbSize = sizeof(SHELLEXECUTEINFO),
            .fMask = SEE_MASK_NOCLOSEPROCESS,
            .hwnd = nullptr,
            .lpVerb = L"runas",
            .lpFile = L"powershell.exe",
            .lpParameters = args.c_str(),
            .lpDirectory = nullptr,
            .nShow = SW_HIDE,
            .hInstApp = nullptr
        };
        QDir installDir = QDir(data->path);

        emit logSent("removing old files..");

        if (installDir.exists() && !installDir.removeRecursively()) {
            emit logSent(QString("failed to remove old installation %1").arg(data->path));
            return false;
        }

        if (!QDir().mkdir(data->path)) {
            emit logSent(QString("failed to create install folder: %1").arg(data->path));
            return false;
        }

        emit logSent("creating winring0 defender exception for PowerTuner..");

        if (ShellExecuteEx(&execInfo) == TRUE && execInfo.hProcess != nullptr) {
            WaitForSingleObject(execInfo.hProcess, 10 * 1000);
            CloseHandle(execInfo.hProcess);

        } else {
            emit logSent(QString("failed to create winring0 exception, code %1").arg(GetLastError()));
            return false;
        }

        return true;
    }

    quint64 InstallWorker::getInstallFilesTotalSize(const QString &qrcDir) const {
        quint64 size = 0;

        for (const QDirListing::DirEntry &entry: QDirListing(qrcDir, QDirListing::IteratorFlag::Recursive))
            size += entry.size();

        return size;
    }

    quint64 InstallWorker::getEstimatedInstallSize() const {
        quint64 installSize = getInstallFilesTotalSize(":/baseins");

        if (data->installDesktop || data->installCli || data->installConsole)
            installSize += getInstallFilesTotalSize(":/baseclientins");

        if (data->installDesktop)
            installSize += getInstallFilesTotalSize(":/clientins");

        if (data->installConsole)
            installSize += getInstallFilesTotalSize(":/consoleins");

        if (data->installCli)
            installSize += getInstallFilesTotalSize(":/cliins");

        if (data->installService)
            installSize += getInstallFilesTotalSize(":/servins");

        return installSize;
    }

    bool InstallWorker::copyFiles(const QString &qrcDir, const QString &basePath) {
        for (const QDirListing::DirEntry &entry: QDirListing(qrcDir, QDirListing::IteratorFlag::Recursive)) {
            QString baseFile = entry.absoluteFilePath().remove(basePath);

            if (baseFile.startsWith('/') || baseFile.startsWith('\\'))
                baseFile.slice(1);

            if (baseFile.isEmpty())
                continue;

            const QString destFile = QString("%1\\%2").arg(data->path, baseFile);

            if (entry.isDir()) {
                const QDir qdir {};

                if (!qdir.exists(destFile) && !qdir.mkdir(destFile)) {
                    emit logSent(QString("failed to create folder: %1").arg(destFile));
                    return false;
                }
            } else {
                QFile destF {destFile};

                if (destF.exists()) {
                    destF.setPermissions(QFile::ReadOther | QFile::WriteOther);
                    emit logSent(QString("removing old %1..").arg(baseFile));

                    if (!destF.remove()) {
                        emit logSent(QString("failed to remove old file: %1\n%2").arg(destFile, destF.errorString()));
                        return false;
                    }
                }

                emit logSent(QString("copying %1..").arg(baseFile));

                if (!QFile::copy(entry.absoluteFilePath(), destFile)) {
                    emit logSent(QString("failed to copy new file: %1").arg(destFile));
                    return false;
                }
            }
        }

        return true;
    }

    bool InstallWorker::createStartMenuLink(const QString &exe, const QString &menuPath) {
        QFile exeF {QString("%1\\%2.exe").arg(data->path, exe)};
        QFile lnkF {QString("%1\\%2.lnk").arg(menuPath, exe)};

        if (lnkF.exists()) {
            lnkF.setPermissions(QFile::ReadOther | QFile::WriteOther);

            if (!lnkF.remove()) {
                emit logSent(QString("failed to remove old link: %1").arg(lnkF.fileName()));
                return false;
            }
        }

        return exeF.link(lnkF.fileName());
    }

    bool InstallWorker::installBase() {
        return copyFiles(":/baseins", ":/baseins/release");
    }

    bool InstallWorker::installBaseClient() {
        if (!data->installDesktop && !data->installConsole && !data->installCli)
            return true;

        return copyFiles(":/baseclientins", ":/baseclientins/release");
    }

    bool InstallWorker::installDesktop() {
        if (!data->installDesktop)
            return true;

        return copyFiles(":/clientins", ":/clientins/release");
    }

    bool InstallWorker::installConsole() {
        if (!data->installConsole)
            return true;

        return copyFiles(":/consoleins", ":/consoleins/release");
    }

    bool InstallWorker::installCli() {
        if (!data->installCli)
            return true;

        return copyFiles(":/cliins", ":/cliins/release");
    }

    bool InstallWorker::installService() {
        if (!data->installService)
            return true;

        return copyFiles(":/servins", ":/servins/release");
    }

    bool InstallWorker::createUninstall() {
        const QDate today = QDate::currentDate();
        const DWORD estimatedSize = getEstimatedInstallSize() / 1024;
        const std::wstring installLoc = data->path.toStdWString();
        const std::wstring installDate = std::format(L"{}{}{}", today.year(), today.month(), today.day());
        const std::wstring uninstallPath = std::format(LR"("{}\PWTUninstall.exe")", installLoc);
        const DWORD majorVer = 1;
        const DWORD minorVer = 0;
        const DWORD dtrue = 1;
        constexpr TCHAR versionStr[] = L"1.x";
        constexpr size_t versionStrSz = (std::wstring_view(versionStr).size() + 1) * sizeof(wchar_t);
        constexpr TCHAR name[] = L"PowerTuner";
        constexpr size_t nameSz = (std::wstring_view(name).size() + 1) * sizeof(wchar_t);
        constexpr TCHAR publisher[] = L"kylon";
        constexpr size_t publisherSz = (std::wstring_view(publisher).size() + 1) * sizeof(wchar_t);
        constexpr TCHAR urlInfo[] = LR"(https://github.com/PowerTuner)";
        constexpr size_t urlInfoSz = (std::wstring_view(urlInfo).size() + 1) * sizeof(wchar_t);
        constexpr TCHAR urlUpdate[] = LR"(https://github.com/PowerTuner/release-channel)";
        constexpr size_t urlUpdateSz = (std::wstring_view(urlUpdate).size() + 1) * sizeof(wchar_t);
        constexpr TCHAR comment[] = L"PowerTuner multi-platform CPU/GPU/OS tuning app";
        constexpr size_t commentSz = (std::wstring_view(comment).size() + 1) * sizeof(wchar_t);
        TCHAR transactionDesc[] = L"PowerTuner create uninstall transaction";
        std::wstring displayIcon;
        HANDLE transaction;
        HKEY rkey;
        LSTATUS ret;

        if (data->installDesktop)
            displayIcon = std::format(L"{}\\PowerTunerClient.exe", installLoc);
        else if (data->installConsole)
            displayIcon = std::format(L"{}\\PowerTunerConsole.exe", installLoc);
        else if (data->installCli)
            displayIcon = std::format(L"{}\\PowerTunerCLI.exe", installLoc);
        else
            displayIcon = std::format(L"{}\\PowerTunerDaemon.exe", installLoc);

        transaction = CreateTransaction(nullptr, nullptr, 0, 0, 0, 0, transactionDesc);
        if (transaction == INVALID_HANDLE_VALUE) {
            emit logSent(QString("failed to create transaction for uninstall registry, code %1").arg(GetLastError()));
            return false;
        }

        ret = RegCreateKeyTransacted(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PowerTuner)", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &rkey, nullptr, transaction, nullptr);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to open uninstall registry, code %1").arg(ret));
            CloseHandle(transaction);
            return false;
        }

        ret = RegSetValueEx(rkey, L"DisplayName", 0, REG_SZ, reinterpret_cast<const BYTE *>(name), nameSz);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set display name, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"DisplayVersion", 0, REG_SZ, reinterpret_cast<const BYTE *>(versionStr), versionStrSz);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set display version, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"DisplayIcon", 0, REG_SZ, reinterpret_cast<const BYTE *>(displayIcon.c_str()), (displayIcon.size() + 1) * sizeof(wchar_t));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set display icon, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"EstimatedSize", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&estimatedSize), sizeof(DWORD));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set estimated size, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"InstallDate", 0, REG_SZ, reinterpret_cast<const BYTE *>(installDate.c_str()), (installDate.size() + 1) * sizeof(wchar_t));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set install date, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"InstallLocation", 0, REG_SZ, reinterpret_cast<const BYTE *>(installLoc.c_str()), (installLoc.size() + 1) * sizeof(wchar_t));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set install location, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"NoModify", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&dtrue), sizeof(DWORD));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set nomodify, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"NoRepair", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&dtrue), sizeof(DWORD));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set norepair, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"Publisher", 0, REG_SZ, reinterpret_cast<const BYTE *>(publisher), publisherSz);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set publisher, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"UninstallString", 0, REG_SZ, reinterpret_cast<const BYTE *>(uninstallPath.c_str()), (uninstallPath.size() + 1) * sizeof(wchar_t));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set uninstall string, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"VersionMajor", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&majorVer), sizeof(DWORD));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set major version, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"VersionMinor", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&minorVer), sizeof(DWORD));
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set minor version, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"URLInfoAbout", 0, REG_SZ, reinterpret_cast<const BYTE *>(urlInfo), urlInfoSz);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set url info about, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"URLUpdateInfo", 0, REG_SZ, reinterpret_cast<const BYTE *>(urlUpdate), urlUpdateSz);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set url update info, code %1").arg(ret));
            goto err_abort;
        }

        ret = RegSetValueEx(rkey, L"Comments", 0, REG_SZ, reinterpret_cast<const BYTE *>(comment), commentSz);
        if (ret != ERROR_SUCCESS) {
            emit logSent(QString("failed to set comments, code %1").arg(ret));
            goto err_abort;
        }

        if (CommitTransaction(transaction) == FALSE) {
            emit logSent(QString("failed to commit uninstaller data, code %1").arg(GetLastError()));
            goto err_abort;
        }

        CloseHandle(transaction);
        RegCloseKey(rkey);
        return true;

    err_abort:
        RollbackTransaction(transaction);
        CloseHandle(transaction);
        RegCloseKey(rkey);
        return false;
    }

    void InstallWorker::downloadFile(const QString &url, const QString &outPath) {
        DownloadWorker *downloadWorker = new DownloadWorker();
        QThread *downloadThd = new QThread();

        downloadWorker->moveToThread(downloadThd);

        QObject::connect(downloadThd, &QThread::finished, downloadWorker, &QObject::deleteLater);
        QObject::connect(this, &InstallWorker::workerDownloadFile, downloadWorker, &DownloadWorker::downloadFile);
        QObject::connect(downloadWorker, &DownloadWorker::logSent, this, &InstallWorker::onLogSent);
        QObject::connect(downloadWorker, &DownloadWorker::failed, this, &InstallWorker::onDownloadError);

        downloadThd->start();
        emit workerDownloadFile(url, outPath);
        downloadThd->wait();
        downloadThd->deleteLater();
    }

    void InstallWorker::linkToStartMenu() {
        if (!data->addToStart || !(data->installDesktop || data->installConsole))
            return;

        const QString startMenuPath = R"(C:\ProgramData\Microsoft\Windows\Start Menu\Programs\PowerTuner)";
        const QDir qdir {};

        emit logSent("creating start menu folder..");

        if (!qdir.exists(startMenuPath) && !qdir.mkdir(startMenuPath)) {
            emit logSent(QString("failed to create start menu folder: %1").arg(startMenuPath));
            return;
        }

        if (data->installDesktop && !createStartMenuLink("PowerTunerClient", startMenuPath)) {
            emit logSent("failed to create desktop client link");
            return;
        }

        if (data->installConsole && !createStartMenuLink("PowerTunerConsole", startMenuPath))
            emit logSent("failed to create console client link");
    }

    void InstallWorker::addToPath() {
        if (data->addEnvPath == 0)
            return;

        HKEY rkey = data->addEnvPath == 1 ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE;
        const TCHAR *subKey = data->addEnvPath == 1 ? L"Environment" : L"System\\CurrentControlSet\\Control\\Session Manager\\Environment";
        QString errStr;
        QString userPath;

        if (!PWTW32::regReadSZ(rkey, subKey, L"Path", userPath, errStr)) {
            emit logSent(QString("failed to read PATH\n%1").arg(errStr));
            return;
        }

        emit logSent("adding to PATH environment variable..");

        for (const QString &path: userPath.split(';', Qt::SkipEmptyParts)) {
            if (path.contains("PowerTuner"))
                return;
        }

        userPath.append(QString("%1%2").arg(userPath.endsWith(';') ? "":";", data->path));

        if (!PWTW32::regWriteSZ(rkey, subKey, L"Path", REG_EXPAND_SZ, userPath, errStr)) {
            emit logSent(QString("failed to add PowerTuner to PATH env variable\n%1").arg(errStr));

        } else {
            if (SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0, 0, SMTO_NORMAL, nullptr) == 0)
                emit logSent("failed to broadcast wm_settingchange event, PATH env changes will take effect on next reboot");
        }
    }

    void InstallWorker::doInstall(const QSharedPointer<installData> &installData) {
        data = installData;

        const QString winringSysPath = QString("%1/WinRing0x64.sys").arg(data->path);
        const QString winringDllPath = QString("%1/WinRing0x64.dll").arg(data->path);
        QString errStr;

        emit logSent("starting installation..");

        if (!hasEnoughDiskSpace()) {
            emit logSent("not enough free space");
            emit resultReady(false);
            return;
        }

        emit logSent("stopping service..");
        if (!PWTW32::stopService(errStr))
            goto err_abort;

        emit logSent("deleting old service..");
        if (!PWTW32::deleteService(errStr))
            goto err_abort;

        emit logSent("closing running PowerTuner apps..");
        if (!PWTW32::closeRunningApps(errStr))
            goto err_abort;

        if (!(createInstallDir() &&
            installBase() &&
            installBaseClient() &&
            installDesktop() &&
            installConsole() &&
            installCli() &&
            installService() &&
            createUninstall()))
        {
            emit resultReady(false);
            return;
        }

        // temp ugly workaround till a new driver is ready
        emit logSent(QString("downloading %1..").arg(winringSysPath));
        downloadFile("https://github.com/PowerTuner/RyzenAdj/raw/refs/heads/main/win32/WinRing0x64.sys", winringSysPath);

        emit logSent(QString("downloading %1..").arg(winringDllPath));
        downloadFile("https://github.com/PowerTuner/RyzenAdj/raw/refs/heads/main/win32/WinRing0x64.dll", winringDllPath);

        linkToStartMenu();
        addToPath();

        emit resultReady(true);
        return;

    err_abort:
        emit logSent(errStr);
        emit resultReady(false);
    }

    void InstallWorker::onDownloadError() {
        emit logSent("failed to download winring0 driver file, please download it from git repo and place it in installed folder");
    }

    void InstallWorker::onLogSent(const QString &msg) {
        emit logSent(msg);
    }
}
