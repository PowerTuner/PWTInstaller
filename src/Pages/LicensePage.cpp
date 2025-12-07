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
#include <QCheckBox>
#include <QTextBrowser>
#include <QFile>
#include <QMessageBox>
#include <QScroller>

#include "LicensePage.h"

namespace PWTI {
    LicensePage::LicensePage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();
        QTextBrowser *licenseTx = new QTextBrowser();
        QCheckBox *agreeChkb = new QCheckBox("I agree");
        QFile licenseF {":/lic/license"};

        licenseTx->setOpenExternalLinks(true);
        licenseTx->setMinimumHeight(330);
        QScroller::grabGesture(licenseTx->viewport(), QScroller::LeftMouseButtonGesture);

        if (licenseF.open(QFile::ReadOnly | QFile::Text))
            licenseTx->setMarkdown(licenseF.readAll());
        else
            licenseTx->setPlainText("Failed to load GPLv3 license text.");

        lyt->addWidget(licenseTx);
        lyt->addSpacing(8);
        lyt->addWidget(agreeChkb);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);

        QObject::connect(agreeChkb, &QCheckBox::checkStateChanged, this, &LicensePage::onAgreeStateChanged);
    }

    void LicensePage::onAgreeStateChanged(const Qt::CheckState state) const {
        data->licenseAccepted = state == Qt::Checked;
    }

    void LicensePage::onNextBtnClicked() {
        if (!data->licenseAccepted) {
            QMessageBox::information(this, "Install", "You must agree to the license to proceed");
            return;
        }

        Page::onNextBtnClicked();
    }
}
