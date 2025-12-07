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
#include "SummaryPage.h"

namespace PWTI {
    SummaryPage::SummaryPage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();
        QHBoxLayout *instPathLyt = new QHBoxLayout();
        QHBoxLayout *instDeskLyt = new QHBoxLayout();
        QHBoxLayout *instConsoleLyt = new QHBoxLayout();
        QHBoxLayout *instCliLyt = new QHBoxLayout();
        QHBoxLayout *instSvcLyt = new QHBoxLayout();
        QHBoxLayout *addStartLyt = new QHBoxLayout();
        QHBoxLayout *addEnvLyt = new QHBoxLayout();
        QLabel *title = new QLabel("Summary");
        QFont titleFont = title->font();

        instPath = new QLabel();
        instDesktop = new QLabel();
        instConsole = new QLabel();
        instCli = new QLabel();
        instService = new QLabel();
        addStart = new QLabel();
        addEnv = new QLabel();

        titleFont.setBold(true);
        title->setAlignment(Qt::AlignCenter);
        title->setFont(titleFont);

        instPathLyt->addWidget(new QLabel(QString("Installation path:")));
        instPathLyt->addWidget(instPath);
        instDeskLyt->addWidget(new QLabel(QString("Install desktop client:")));
        instDeskLyt->addWidget(instDesktop);
        instConsoleLyt->addWidget(new QLabel(QString("Install console client:")));
        instConsoleLyt->addWidget(instConsole);
        instCliLyt->addWidget(new QLabel(QString("Install CLI client:")));
        instCliLyt->addWidget(instCli);
        instSvcLyt->addWidget(new QLabel(QString("Install service:")));
        instSvcLyt->addWidget(instService);
        addStartLyt->addWidget(new QLabel(QString("Add PowerTuner to start menu:")));
        addStartLyt->addWidget(addStart);
        addEnvLyt->addWidget(new QLabel(QString("Add to PATH environment variable:")));
        addEnvLyt->addWidget(addEnv);
        lyt->addWidget(title);
        lyt->addSpacing(8);
        lyt->addLayout(instPathLyt);
        lyt->addSpacing(2);
        lyt->addLayout(instDeskLyt);
        lyt->addSpacing(2);
        lyt->addLayout(instConsoleLyt);
        lyt->addSpacing(2);
        lyt->addLayout(instCliLyt);
        lyt->addSpacing(2);
        lyt->addLayout(instSvcLyt);
        lyt->addSpacing(2);
        lyt->addLayout(addStartLyt);
        lyt->addSpacing(2);
        lyt->addLayout(addEnvLyt);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);
    }

    void SummaryPage::showEvent(QShowEvent *event) {
        if (event->spontaneous())
            return QWidget::showEvent(event);

        instPath->setText(data->path);
        instDesktop->setText(data->installDesktop ? "Yes":"No");
        instConsole->setText(data->installConsole ? "Yes":"No");
        instCli->setText(data->installCli ? "Yes":"No");
        instService->setText(data->installService ? "Yes":"No");
        addStart->setText(data->addToStart ? "Yes":"No");
        addEnv->setText(data->addEnvPath == 0 ? "No" : (data->addEnvPath == 1 ? "My user":"All users"));
    }
}
