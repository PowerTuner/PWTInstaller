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

#include "WelcomePage.h"

namespace PWTI {
    WelcomePage::WelcomePage(): Page(nullptr) {
        QVBoxLayout *lyt = new QVBoxLayout();
        QLabel *contentTx = new QLabel("Welcome to PowerTuner installer, the world first multiplatform tuning app!\n\n");

        lyt->addWidget(contentTx);
        lyt->addStretch();
        lyt->addLayout(buttonsLyt);

        setLayout(lyt);
    }
}
