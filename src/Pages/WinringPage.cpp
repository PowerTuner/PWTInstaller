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
#include <QTextBrowser>
#include <QCheckBox>
#include <QMessageBox>
#include <QFile>
#include <QScroller>

#include "WinringPage.h"

namespace PWTI {
    WinringPage::WinringPage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();
        QTextBrowser *warningTxt = new QTextBrowser();
        QCheckBox *agreeChkb = new QCheckBox("I agree");
        QFile warnF {":/ring/winring"};

        warningTxt->setMinimumHeight(330);
        warningTxt->setOpenExternalLinks(true);
        QScroller::grabGesture(warningTxt->viewport(), QScroller::LeftMouseButtonGesture);

        if (warnF.open(QFile::ReadOnly | QFile::Text))
            warningTxt->setMarkdown(warnF.readAll());
        else
            warningTxt->setPlainText("Failed to load text.");

        lyt->addWidget(warningTxt);
        lyt->addSpacing(8);
        lyt->addWidget(agreeChkb);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);

        QObject::connect(agreeChkb, &QCheckBox::checkStateChanged, this, &WinringPage::onAgreeStateChanged);
    }

    void WinringPage::onAgreeStateChanged(const Qt::CheckState state) const {
        data->winringAccepted = state == Qt::Checked;
    }

    void WinringPage::onNextBtnClicked() {
        if (!data->winringAccepted) {
            QMessageBox::information(this, "Install", "You must agree to proceed");
            return;
        }

        Page::onNextBtnClicked();
    }
}
