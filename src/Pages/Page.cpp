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
#include <QApplication>

#include "Page.h"

namespace PWTI {
    Page::Page(const QSharedPointer<installData> &installData) {
        data = installData;
        buttonsLyt = new QHBoxLayout();

        buttonsLyt->addStretch();

        QWidget::setVisible(false);
    }

    void Page::setNavigation(Page *prev, Page *next, const bool hasExit) {
        prevPage = prev;
        nextPage = next;

        if (prev != nullptr) {
            prevBtn = new QPushButton("Previous");

            buttonsLyt->addWidget(prevBtn);
            QObject::connect(prevBtn, &QPushButton::clicked, this, &Page::onPrevBtnClicked);
        }

        if (next != nullptr) {
            nextBtn = new QPushButton("Next");

            buttonsLyt->addWidget(nextBtn);
            QObject::connect(nextBtn, &QPushButton::clicked, this, &Page::onNextBtnClicked);
        }

        if (hasExit) {
            exitBtn = new QPushButton("Exit");

            buttonsLyt->addWidget(exitBtn);
            QObject::connect(exitBtn, &QPushButton::clicked, this, &Page::onExitBtnClicked);
        }
    }

    void Page::onPrevBtnClicked() {
        setVisible(false);
        prevPage->setVisible(true);
    }

    void Page::onNextBtnClicked() {
        setVisible(false);
        nextPage->setVisible(true);
    }

    void Page::onExitBtnClicked() const {
        QApplication::quit();
    }
}
