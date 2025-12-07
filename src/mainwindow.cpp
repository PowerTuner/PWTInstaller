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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Pages/WelcomePage.h"
#include "Pages/LicensePage.h"
#include "Pages/WinringPage.h"
#include "Pages/FolderPage.h"
#include "Pages/CustomizePage.h"
#include "Pages/SummaryPage.h"
#include "Pages/InstallPage.h"
#include "Pages/FinishPage.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    const QSharedPointer<PWTI::installData> installData = QSharedPointer<PWTI::installData>::create();
    QVBoxLayout *lyt = new QVBoxLayout();
    QLabel *title = new QLabel("PowerTuner Installer");
    QFont mainFont = font();
    QFont titleFnt = title->font();
    PWTI::WelcomePage *welcomePage = new PWTI::WelcomePage();
    PWTI::LicensePage *licensePage = new PWTI::LicensePage(installData);
    PWTI::WinringPage *winringPage = new PWTI::WinringPage(installData);
    PWTI::FolderPage *folderPage = new PWTI::FolderPage(installData);
    PWTI::CustomizePage *customizePage = new PWTI::CustomizePage(installData);
    PWTI::SummaryPage *summaryPage = new PWTI::SummaryPage(installData);
    PWTI::InstallPage *installPage = new PWTI::InstallPage(installData);
    PWTI::FinishPage *finishPage = new PWTI::FinishPage(installData);

    mainFont.setPointSize(11);
    setWindowTitle("PowerTuner Installer");
    setWindowIcon(QIcon(":/ico/PWT"));
    setMinimumSize(600, 500);
    setMaximumSize(800, 700);
    resize(600, 500);
    setFont(mainFont);

    titleFnt.setBold(true);
    titleFnt.setPointSize(18);
    title->setFont(titleFnt);
    title->setAlignment(Qt::AlignCenter);
    welcomePage->setNavigation(nullptr, licensePage, true);
    licensePage->setNavigation(welcomePage, winringPage, true);
    winringPage->setNavigation(licensePage, folderPage, true);
    folderPage->setNavigation(winringPage, customizePage, true);
    customizePage->setNavigation(folderPage, summaryPage, true);
    summaryPage->setNavigation(customizePage, installPage, true);
    installPage->setNavigation(summaryPage, finishPage, true);
    finishPage->setNavigation(nullptr, nullptr, true);
    welcomePage->setVisible(true);

    lyt->addWidget(title);
    lyt->addSpacing(15);
    lyt->addWidget(welcomePage);
    lyt->addWidget(licensePage);
    lyt->addWidget(winringPage);
    lyt->addWidget(folderPage);
    lyt->addWidget(customizePage);
    lyt->addWidget(summaryPage);
    lyt->addWidget(installPage);
    lyt->addWidget(finishPage);

    ui->centralwidget->setLayout(lyt);
}

MainWindow::~MainWindow() {
    delete ui;
}
