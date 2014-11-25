//
//  HMDToolsDialog.cpp
//  interface/src/ui
//
//  Created by Brad Hefta-Gaub on 7/19/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QFormLayout>
#include <QGuiApplication>
#include <QDialogButtonBox>

#include <QDesktopWidget>
#include <QPushButton>
#include <QString>
#include <QScreen>
#include <QWindow>

#include <VoxelConstants.h>

#include "Menu.h"
#include "devices/OculusManager.h"
#include "ui/HMDToolsDialog.h"


HMDToolsDialog::HMDToolsDialog(QWidget* parent) :
    QDialog(parent, Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint) 
{
    this->setWindowTitle("HMD Tools");

    // Create layouter
    QFormLayout* form = new QFormLayout();

    // Add a button to enter
    QPushButton* enterModeButton = new QPushButton("Enter HMD Mode");
    form->addRow("", enterModeButton);
    connect(enterModeButton,SIGNAL(clicked(bool)),this,SLOT(enterModeClicked(bool)));

    // Add a button to leave
    QPushButton* leaveModeButton = new QPushButton("Leave HMD Mode");
    form->addRow("", leaveModeButton);
    connect(leaveModeButton,SIGNAL(clicked(bool)),this,SLOT(leaveModeClicked(bool)));

    this->QDialog::setLayout(form);


    _wasMoved = false;
    
    _previousRect = Application::getInstance()->getWindow()->rect();
    

    Application::getInstance()->getWindow()->activateWindow();
}

HMDToolsDialog::~HMDToolsDialog() {
}

void HMDToolsDialog::enterModeClicked(bool checked) {
    qDebug() << "enterModeClicked";

    int hmdScreen = OculusManager::getHMDScreen();
    
    // hack to test...
    if (hmdScreen == -1) {
        hmdScreen = 0;
    }
    
    if (hmdScreen >= 0) {
        QWindow* mainWindow = Application::getInstance()->getWindow()->windowHandle();
        _hmdScreen = QGuiApplication::screens()[hmdScreen];
        
        _previousRect = Application::getInstance()->getWindow()->rect();
        qDebug() << "_previousRect:" << _previousRect;
        _previousRect = QRect(mainWindow->mapToGlobal(_previousRect.topLeft()), 
                                mainWindow->mapToGlobal(_previousRect.bottomRight()));
        qDebug() << "after mapping... _previousRect:" << _previousRect;
        
        _previousScreen = mainWindow->screen();
        qDebug() << "_previousScreen:" << _previousScreen;

        QRect rect = QApplication::desktop()->screenGeometry(hmdScreen);

        qDebug() << "about to move to:" << rect.topLeft();

        mainWindow->setScreen(_hmdScreen);
        mainWindow->setGeometry(rect);

        _wasMoved = true;
    }
    Application::getInstance()->setFullscreen(true);
    Application::getInstance()->setEnableVRMode(true);
    
    const int SLIGHT_DELAY = 500;
    QTimer::singleShot(SLIGHT_DELAY, this, SLOT(activateWindowAfterEnterMode()));
}

void HMDToolsDialog::activateWindowAfterEnterMode() {
    qDebug() << "activateWindowAfterEnterMode";
    Application::getInstance()->getWindow()->activateWindow();

    QWindow* mainWindow = Application::getInstance()->getWindow()->windowHandle();
    QPoint windowCenter = mainWindow->geometry().center();
    //QPoint desktopCenter = mainWindow->mapToGlobal(windowCenter);
    qDebug() << "windowCenter:" << windowCenter;
    QCursor::setPos(_hmdScreen, windowCenter);
}


void HMDToolsDialog::leaveModeClicked(bool checked) {
    qDebug() << "leaveModeClicked";

    Application::getInstance()->setFullscreen(false);
    Application::getInstance()->setEnableVRMode(false);
    Application::getInstance()->getWindow()->activateWindow();

    if (_wasMoved) {
        QWindow* mainWindow = Application::getInstance()->getWindow()->windowHandle();
        mainWindow->setScreen(_previousScreen);
        mainWindow->setGeometry(_previousRect);
    
        const int SLIGHT_DELAY = 1500;
        QTimer::singleShot(SLIGHT_DELAY, this, SLOT(moveWindowAfterLeaveMode()));
    }
    _wasMoved = false;
}

void HMDToolsDialog::moveWindowAfterLeaveMode() {
    qDebug() << "moveWindowAfterLeaveMode";
    QWindow* mainWindow = Application::getInstance()->getWindow()->windowHandle();
    mainWindow->setScreen(_previousScreen);
    mainWindow->setGeometry(_previousRect);
    Application::getInstance()->getWindow()->activateWindow();
}


void HMDToolsDialog::reject() {
    // Just regularly close upon ESC
    this->QDialog::close();
}

void HMDToolsDialog::closeEvent(QCloseEvent* event) {
    this->QDialog::closeEvent(event);
    emit closed();
}


