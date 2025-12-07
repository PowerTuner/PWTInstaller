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
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

#include "FolderPage.h"

namespace PWTI {
    FolderPage::FolderPage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();
        QHBoxLayout *pathLyt = new QHBoxLayout();
        QPushButton *browseBtn = new QPushButton("Browse");

        installPath = new QLineEdit();

        installPath->setText(installData->path);

        pathLyt->addWidget(installPath);
        pathLyt->addSpacing(5);
        pathLyt->addWidget(browseBtn);
        lyt->addWidget(new QLabel("Setup will install PowerTuner to the following folder."));
        lyt->addSpacing(15);
        lyt->addLayout(pathLyt);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);

        QObject::connect(installPath, &QLineEdit::textChanged, this, &FolderPage::onPathTextChanged);
        QObject::connect(browseBtn, &QPushButton::clicked, this, &FolderPage::onBrowseBtnClicked);
    }

    void FolderPage::onPathTextChanged(const QString &text) const {
        data->path = text;
    }

    void FolderPage::onBrowseBtnClicked() {
        QString dest = QFileDialog::getExistingDirectory(qobject_cast<QWidget *>(this), "Select install folder", data->path);
        const QSignalBlocker sblock {installPath};

        if (dest.isEmpty())
            return;

        data->path = dest.endsWith("PowerTuner") ? dest : dest.append("/PowerTuner");
        installPath->setText(dest);
    }

    void FolderPage::onNextBtnClicked() {
        if (data->path.isEmpty()) {
            QMessageBox::information(this, "Install", "Installation path cannot be empty.");
            return;
        }

        if (!data->path.endsWith("PowerTuner"))
            data->path.append("/PowerTuner");

        Page::onNextBtnClicked();
    }
}
