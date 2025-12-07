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
#include <QLabel>
#include <QMessageBox>
#include <QComboBox>

#include "CustomizePage.h"

namespace PWTI {
    CustomizePage::CustomizePage(const QSharedPointer<installData> &installData): Page(installData) {
        QVBoxLayout *lyt = new QVBoxLayout();
        QHBoxLayout *envLyt = new QHBoxLayout();
        QComboBox *addEnvPathCombo = new QComboBox();
        QLabel *componentsTitle = new QLabel("Components");
        QLabel *optionsTitle = new QLabel("Options");
        QFont componentsFont = componentsTitle->font();

        clientChkb = new QCheckBox("PowerTuner desktop client");
        consoleChkb = new QCheckBox("PowerTuner console client");
        cliChkb = new QCheckBox("PowerTuner CLI client");
        serviceChkb = new QCheckBox("PowerTuner service");
        addToStartChkb = new QCheckBox("Add PowerTuner to Start menu");

        clientChkb->setChecked(installData->installDesktop);
        consoleChkb->setChecked(installData->installConsole);
        cliChkb->setChecked(installData->installCli);
        serviceChkb->setChecked(installData->installService);
        addToStartChkb->setChecked(installData->addToStart);
        addEnvPathCombo->addItems({
            "Don't add PowerTuner to PATH environment variable",
            "add PowerTuner to PATH env variable for my user",
            "add PowerTuner to PATH env variable for all users"
        });
        componentsFont.setBold(true);
        componentsTitle->setFont(componentsFont);
        componentsTitle->setAlignment(Qt::AlignCenter);
        optionsTitle->setFont(componentsFont);
        optionsTitle->setAlignment(Qt::AlignCenter);

        envLyt->addWidget(addEnvPathCombo);
        envLyt->addStretch();
        lyt->addWidget(componentsTitle);
        lyt->addSpacing(6);
        lyt->addWidget(clientChkb);
        lyt->addWidget(consoleChkb);
        lyt->addWidget(cliChkb);
        lyt->addWidget(serviceChkb);
        lyt->addSpacing(6);
        lyt->addWidget(optionsTitle);
        lyt->addSpacing(6);
        lyt->addWidget(addToStartChkb);
        lyt->addSpacing(4);
        lyt->addLayout(envLyt);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);

        QObject::connect(clientChkb, &QCheckBox::checkStateChanged, this, &CustomizePage::onClientStateChanged);
        QObject::connect(consoleChkb, &QCheckBox::checkStateChanged, this, &CustomizePage::onConsoleStateChanged);
        QObject::connect(cliChkb, &QCheckBox::checkStateChanged, this, &CustomizePage::onCliStateChanged);
        QObject::connect(serviceChkb, &QCheckBox::checkStateChanged, this, &CustomizePage::onServiceStateChanged);
        QObject::connect(addToStartChkb, &QCheckBox::checkStateChanged, this, &CustomizePage::onAddToStartStateChanged);
        QObject::connect(addEnvPathCombo, &QComboBox::currentIndexChanged, this, &CustomizePage::onEnvPathChanged);
    }

    void CustomizePage::onClientStateChanged(const Qt::CheckState state) const {
        data->installDesktop = state == Qt::Checked;
    }

    void CustomizePage::onConsoleStateChanged(const Qt::CheckState state) const {
        data->installConsole = state == Qt::Checked;
    }

    void CustomizePage::onCliStateChanged(const Qt::CheckState state) const {
        data->installCli = state == Qt::Checked;
    }

    void CustomizePage::onServiceStateChanged(const Qt::CheckState state) const {
        data->installService = state == Qt::Checked;
    }

    void CustomizePage::onAddToStartStateChanged(const Qt::CheckState state) const {
        data->addToStart = state == Qt::Checked;
    }

    void CustomizePage::onEnvPathChanged(const int idx) const {
        data->addEnvPath = idx;
    }

    void CustomizePage::onNextBtnClicked() {
        if (!data->installDesktop && !data->installConsole && !data->installCli && !data->installService) {
            QMessageBox::information(this, "Install", "At least one component must be selected");
            return;
        }

        Page::onNextBtnClicked();
    }
}
