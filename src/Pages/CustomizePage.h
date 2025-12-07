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

#include <QCheckBox>

#include "Page.h"

namespace PWTI {
    class CustomizePage final: public Page {
        Q_OBJECT

    private:
        QCheckBox *clientChkb = nullptr;
        QCheckBox *consoleChkb = nullptr;
        QCheckBox *cliChkb = nullptr;
        QCheckBox *serviceChkb = nullptr;
        QCheckBox *addToStartChkb = nullptr;

    public:
        explicit CustomizePage(const QSharedPointer<installData> &installData);

    private slots:
        void onClientStateChanged(Qt::CheckState state) const;
        void onConsoleStateChanged(Qt::CheckState state) const;
        void onCliStateChanged(Qt::CheckState state) const;
        void onServiceStateChanged(Qt::CheckState state) const;
        void onAddToStartStateChanged(Qt::CheckState state) const;
        void onEnvPathChanged(int idx) const;

    protected slots:
        void onNextBtnClicked() override;
    };
}
