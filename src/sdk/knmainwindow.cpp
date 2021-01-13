/*
 * Copyright (C) Kreogist Dev Team
 *
 * You can redistribute this software and/or modify it under the
 * terms of the HARERU Software License; either version 1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * license file for more details.
 */
#include <QCloseEvent>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QBoxLayout>

#include "knfilemanager.h"
#include "knstatusbar.h"
#include "knglobal.h"
#include "knuimanager.h"
#include "kneditmenu.h"
#include "knsearchmenu.h"
#include "knviewmenu.h"
#include "knhelpmenu.h"

#include "knmainwindow.h"

KNMainWindow::KNMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_toolbar(nullptr),
    m_fileManager(nullptr),
    m_statusBar(nullptr)
{
    //Update the window icon.
    setWindowIcon(QIcon(":/memo.png"));
    //Initial the global instance.
    KNGlobal::initial(this);
    //Set the title name.
    knUi->addTranslate(this, &KNMainWindow::retranslate);
}

void KNMainWindow::initalize()
{
    //Construct widgets.
    m_fileManager = new KNFileManager(this);
    m_toolbar = new QToolBar(this);
    m_statusBar = new KNStatusBar(this);
    //Construct the main menu.
    addToolBar(Qt::TopToolBarArea, m_toolbar);
    setCentralWidget(m_fileManager);
    setStatusBar(m_statusBar);
    //Link file manager with other widgets.
    connect(m_fileManager, &KNFileManager::titleChange,
            this, &KNMainWindow::updateTitle);
    connect(m_fileManager, &KNFileManager::editorChanged,
            this, &KNMainWindow::onEditorChanged);
    //Link the status bar.
    connect(m_statusBar, &KNStatusBar::requireGoto,
            m_fileManager->searchMenuItem(KNSearchMenu::GoTo), &QAction::trigger);
    connect(m_statusBar, &KNStatusBar::requireSummary,
            m_fileManager->viewMenuItem(KNViewMenu::Summary), &QAction::trigger);
    connect(m_statusBar, &KNStatusBar::requireCodec,
            m_fileManager, &KNFileManager::showCodecDialog);
    //Add menu.
    menuBar()->addMenu(m_fileManager->fileMenu());
    menuBar()->addMenu(m_fileManager->editMenu());
    menuBar()->addMenu(m_fileManager->searchMenu());
    menuBar()->addMenu(m_fileManager->viewMenu());
    menuBar()->addMenu(m_fileManager->codecMenu());
    menuBar()->addMenu(m_fileManager->toolMenu());
    menuBar()->addMenu(m_fileManager->windowsMenu());
    menuBar()->addMenu(new KNHelpMenu(this));
    //Configure the toolbar.
    m_toolbar->setMovable(false);
    m_toolbar->setIconSize(knUi->size(16, 16));
    //Add actions to toolbar.
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::New));
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::Open));
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::Save));
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::SaveAll));
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::Close));
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::CloseAll));
    m_toolbar->addAction(m_fileManager->fileMenuItem(KNFileManager::Print));
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_fileManager->editMenuItem(KNEditMenu::Cut));
    m_toolbar->addAction(m_fileManager->editMenuItem(KNEditMenu::Copy));
    m_toolbar->addAction(m_fileManager->editMenuItem(KNEditMenu::Paste));
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_fileManager->editMenuItem(KNEditMenu::Undo));
    m_toolbar->addAction(m_fileManager->editMenuItem(KNEditMenu::Redo));
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_fileManager->searchMenuItem(KNSearchMenu::Find));
    m_toolbar->addAction(m_fileManager->searchMenuItem(KNSearchMenu::Replace));
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_fileManager->viewMenuItem(KNViewMenu::ZoomIn));
    m_toolbar->addAction(m_fileManager->viewMenuItem(KNViewMenu::ZoomOut));
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_fileManager->viewMenuItem(KNViewMenu::WordWrap));
    m_toolbar->addAction(m_fileManager->viewMenuItem(KNViewMenu::ShowAllChars));
}

void KNMainWindow::closeEvent(QCloseEvent *event)
{
    if(!m_fileManager->closeAllTab())
    {
        //Ignore the close event.
        event->ignore();
        return;
    }
    //Do the close event.
    QMainWindow::closeEvent(event);
}

void KNMainWindow::retranslate()
{
    //Update the window title text.
    m_title = tr("Memo");
    //Update the window title.
    updateTitle(m_fileManager->currentWindowTitle());
}

void KNMainWindow::updateTitle(const QString &fileTitle)
{
    //Check the file title.
    if(fileTitle.isEmpty())
    {
        setWindowTitle(m_title);
    }
    else
    {
        //Build the title.
        setWindowTitle(QString("%1 - %2").arg(fileTitle, m_title));
    }
}

void KNMainWindow::onEditorChanged()
{
    // Get the current editor.
    auto editor = m_fileManager->currentEditor();
    //Link the status bar.
    m_statusBar->linkEditor(editor);
}
