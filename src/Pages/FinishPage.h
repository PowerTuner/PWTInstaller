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
#include <QLabel>

#include "Page.h"

namespace PWTI {
    class FinishPage final: public Page {
        Q_OBJECT

    private:
        QCheckBox *startServiceChkb = nullptr;
        QLabel *msgLbl = nullptr;

    public:
        explicit FinishPage(const QSharedPointer<installData> &installData);

    private slots:
        void onInstallServiceResult(const QString &res);

    protected slots:
        void onExitBtnClicked() const override;
    };
}
