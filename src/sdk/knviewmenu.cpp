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
#include <QActionGroup>
#include <QDockWidget>

#include "knuimanager.h"
#include "knglobal.h"
#include "knmainwindow.h"
#include "kntexteditor.h"
#include "kndocumentmap.h"
#include "knfolderpanel.h"

#include "knviewmenu.h"

KNViewMenu::KNViewMenu(QWidget *parent) :
    QMenu(parent),
    m_mapDockWidget(new QDockWidget(parent)),
    m_folderDockWidget(new QDockWidget(parent)),
    m_docMap(new KNDocumentMap(this)),
    m_folderPanel(new KNFolderPanel(this))
{
    //Add dock widgets.
    knGlobal->mainWindow()->addDockWidget(Qt::RightDockWidgetArea, m_mapDockWidget);
    m_mapDockWidget->setWidget(m_docMap);
    knGlobal->mainWindow()->addDockWidget(Qt::LeftDockWidgetArea, m_folderDockWidget);
    m_folderDockWidget->setWidget(m_folderPanel);
    //Construct the actions.
    for(int i=0; i<ViewMenuItemCount; ++i)
    {
        m_menuItems[i] = new QAction(this);
    }
    for(int i=0; i<ViewSubMenuCount; ++i)
    {
        m_subMenus[i] = new QMenu(this);
    }
    //Add actions to the menu.
    addAction(m_menuItems[AlwaysOnTop]);
    addAction(m_menuItems[FullScreen]);
    addSeparator();
    addMenu(m_subMenus[ShowSymbol]);
    m_subMenus[ShowSymbol]->addAction(m_menuItems[ShowWhiteSpace]);
    m_subMenus[ShowSymbol]->addAction(m_menuItems[ShowEOF]);
    m_subMenus[ShowSymbol]->addAction(m_menuItems[ShowAllChars]);
    addMenu(m_subMenus[Zoom]);
    m_subMenus[Zoom]->addAction(m_menuItems[ZoomIn]);
    m_subMenus[Zoom]->addAction(m_menuItems[ZoomOut]);
    m_subMenus[Zoom]->addAction(m_menuItems[ZoomReset]);
    addMenu(m_subMenus[Tab]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo1]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo2]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo3]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo4]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo5]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo6]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo7]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo8]);
    m_subMenus[Tab]->addAction(m_menuItems[TabNo9]);
    m_subMenus[Tab]->addSeparator();
    m_subMenus[Tab]->addAction(m_menuItems[TabNext]);
    m_subMenus[Tab]->addAction(m_menuItems[TabPrev]);
    m_subMenus[Tab]->addSeparator();
    m_subMenus[Tab]->addAction(m_menuItems[TabMoveForward]);
    m_subMenus[Tab]->addAction(m_menuItems[TabMoveBackward]);
    addAction(m_menuItems[WordWrap]);
    addSeparator();
    addAction(m_menuItems[FoldAll]);
    addAction(m_menuItems[UnfoldAll]);
    addAction(m_menuItems[CollapseCurrentLevel]);
    addAction(m_menuItems[UncollapseCurrentLevel]);
    addMenu(m_subMenus[CollapseLevel]);
    addMenu(m_subMenus[UncollapseLevel]);
    addSeparator();
    addAction(m_menuItems[Summary]);
    addSeparator();
    addAction(m_menuItems[FolderPanel]);
    addAction(m_menuItems[DocumentMap]);
    addAction(m_menuItems[FunctionList]);
    addSeparator();
    addAction(m_menuItems[TextDirectionRTL]);
    addAction(m_menuItems[TextDirectionLTR]);
    addSeparator();
    addAction(m_menuItems[Monitoring]);
    //Link the translator.
    knUi->addTranslate(this, &KNViewMenu::retranslate);
    //Change action states.
    m_menuItems[AlwaysOnTop]->setCheckable(true);
    m_menuItems[FullScreen]->setCheckable(true);
    m_menuItems[ShowWhiteSpace]->setCheckable(true);
    m_menuItems[ShowEOF]->setCheckable(true);
    m_menuItems[ShowAllChars]->setCheckable(true);
    m_menuItems[WordWrap]->setCheckable(true);
    m_menuItems[FolderPanel]->setCheckable(true);
    //Set the shortcut of the menu.
    m_menuItems[FullScreen]->setShortcut(QKeySequence::FullScreen);
    m_menuItems[ZoomIn]->setShortcut(QKeySequence::ZoomIn);
    m_menuItems[ZoomOut]->setShortcut(QKeySequence::ZoomOut);
    m_menuItems[ZoomReset]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_0));
    m_menuItems[TabNo1]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_1));
    m_menuItems[TabNo2]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_2));
    m_menuItems[TabNo3]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_3));
    m_menuItems[TabNo4]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_4));
    m_menuItems[TabNo5]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_5));
    m_menuItems[TabNo6]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_6));
    m_menuItems[TabNo7]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_7));
    m_menuItems[TabNo8]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_8));
    m_menuItems[TabNo9]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_9));
    m_menuItems[TabNext]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_PageDown));
    m_menuItems[TabPrev]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_PageUp));
    m_menuItems[TabMoveForward]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_PageDown));
    m_menuItems[TabMoveBackward]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_PageUp));
    //Set icons.
    m_menuItems[ZoomIn]->setIcon(QIcon(":/icons/zoomIn.png"));
    m_menuItems[ZoomOut]->setIcon(QIcon(":/icons/zoomOut.png"));
    m_menuItems[ShowAllChars]->setIcon(QIcon(":/icons/invisibleChar.png"));
    m_menuItems[WordWrap]->setIcon(QIcon(":/icons/wrap.png"));
    //Link actions.
    connect(m_menuItems[AlwaysOnTop], &QAction::triggered, this, &KNViewMenu::onAlwaysOnTopToggle);
    connect(m_menuItems[FullScreen], &QAction::triggered, this, &KNViewMenu::onFullScreenToggle);
    connect(m_menuItems[ShowWhiteSpace], &QAction::triggered, this, &KNViewMenu::onShowWhitespace);
    connect(m_menuItems[ShowEOF], &QAction::triggered, this, &KNViewMenu::onShowEof);
    connect(m_menuItems[ShowAllChars], &QAction::triggered, this, &KNViewMenu::onShowAll);
    connect(m_menuItems[WordWrap], &QAction::triggered, this, &KNViewMenu::onWordWrap);
    connect(m_menuItems[ZoomIn], &QAction::triggered, knGlobal, &KNGlobal::zoomIn);
    connect(m_menuItems[ZoomOut], &QAction::triggered, knGlobal, &KNGlobal::zoomOut);
    connect(m_menuItems[ZoomReset], &QAction::triggered, knGlobal, &KNGlobal::zoomReset);
    connect(m_menuItems[TabNo1], &QAction::triggered, [=]{ emit requireToTab(0); });
    connect(m_menuItems[TabNo2], &QAction::triggered, [=]{ emit requireToTab(1); });
    connect(m_menuItems[TabNo3], &QAction::triggered, [=]{ emit requireToTab(2); });
    connect(m_menuItems[TabNo4], &QAction::triggered, [=]{ emit requireToTab(3); });
    connect(m_menuItems[TabNo5], &QAction::triggered, [=]{ emit requireToTab(4); });
    connect(m_menuItems[TabNo6], &QAction::triggered, [=]{ emit requireToTab(5); });
    connect(m_menuItems[TabNo7], &QAction::triggered, [=]{ emit requireToTab(6); });
    connect(m_menuItems[TabNo8], &QAction::triggered, [=]{ emit requireToTab(7); });
    connect(m_menuItems[TabNo9], &QAction::triggered, [=]{ emit requireToTab(8); });
    connect(m_menuItems[TabNext], &QAction::triggered, this, &KNViewMenu::requireToNextTab);
    connect(m_menuItems[TabPrev], &QAction::triggered, this, &KNViewMenu::requireToPrevTab);
    connect(m_menuItems[TabMoveForward], &QAction::triggered, this, &KNViewMenu::requireMoveForward);
    connect(m_menuItems[TabMoveBackward], &QAction::triggered, this, &KNViewMenu::requireMoveBackward);
    connect(m_menuItems[Summary], &QAction::triggered, this, &KNViewMenu::requireToShowSummary);
    connect(m_menuItems[FolderPanel], &QAction::triggered, m_folderDockWidget, &QDockWidget::setVisible);
    connect(m_folderDockWidget, &QDockWidget::visibilityChanged, m_menuItems[FolderPanel], &QAction::setChecked);
    connect(m_menuItems[TextDirectionRTL], &QAction::triggered, [=]{ knGlobal->setAlignLeft(false); });
    connect(m_menuItems[TextDirectionLTR], &QAction::triggered, [=]{ knGlobal->setAlignLeft(true); });
    //Hide panels at default.
    m_mapDockWidget->close();
    m_folderDockWidget->close();
}

QAction *KNViewMenu::menuItem(int index)
{
    return m_menuItems[index];
}

void KNViewMenu::setEditor(KNTextEditor *editor)
{
    //Set the document to map.
    //    m_docMap->setDocument(editor ? editor->document() : nullptr);
}

void KNViewMenu::setAndShowFolder(const QString &folderPath)
{
    //Set the folder path.
    m_folderPanel->setFolder(folderPath);
    //Show the panel.
    m_folderDockWidget->show();
}

KNFolderPanel *KNViewMenu::folderPanel() const
{
    return m_folderPanel;
}

void KNViewMenu::retranslate()
{
    //Update the title of the menu.
    setTitle(tr("&View"));
    //Retranslate all the actions.
    m_menuItems[AlwaysOnTop]->setText(tr("Always on Top"));
    m_menuItems[FullScreen]->setText(tr("Toggle Full Screen Mode"));
    m_menuItems[WordWrap]->setText(tr("Word wrap"));
    m_menuItems[FoldAll]->setText(tr("Fold All"));
    m_menuItems[UnfoldAll]->setText(tr("Unfold All"));
    m_menuItems[CollapseCurrentLevel]->setText(tr("Collapse Current Level"));
    m_menuItems[UncollapseCurrentLevel]->setText(tr("Uncollapse Current Level"));
    m_menuItems[Summary]->setText(tr("Summary..."));
    m_menuItems[FolderPanel]->setText(tr("Folder Panel"));
    m_menuItems[DocumentMap]->setText(tr("Document Map"));
    m_menuItems[FunctionList]->setText(tr("Function List"));
    m_menuItems[TextDirectionRTL]->setText(tr("Text Direction RTL"));
    m_menuItems[TextDirectionLTR]->setText(tr("Text Direction LTR"));
    m_menuItems[Monitoring]->setText(tr("Monitoring (tail -f)"));
    m_menuItems[ShowWhiteSpace]->setText(tr("Show White Space and TAB"));
    m_menuItems[ShowEOF]->setText(tr("Show End of Line"));
    m_menuItems[ShowAllChars]->setText(tr("Show All Characters"));
    m_menuItems[ZoomIn]->setText(tr("Zoom &In"));
    m_menuItems[ZoomOut]->setText(tr("Zoom &Out"));
    m_menuItems[ZoomReset]->setText(tr("Restore Default Zoom"));
    m_menuItems[TabNo1]->setText(tr("1st Tab"));
    m_menuItems[TabNo2]->setText(tr("2nd Tab"));
    m_menuItems[TabNo3]->setText(tr("3rd Tab"));
    m_menuItems[TabNo4]->setText(tr("4th Tab"));
    m_menuItems[TabNo5]->setText(tr("5th Tab"));
    m_menuItems[TabNo6]->setText(tr("6th Tab"));
    m_menuItems[TabNo7]->setText(tr("7th Tab"));
    m_menuItems[TabNo8]->setText(tr("8th Tab"));
    m_menuItems[TabNo9]->setText(tr("9th Tab"));
    m_menuItems[TabNext]->setText(tr("Next Tab"));
    m_menuItems[TabPrev]->setText(tr("Previous Tab"));
    m_menuItems[TabMoveForward]->setText(tr("Move Tab Forward"));
    m_menuItems[TabMoveBackward]->setText(tr("Move Tab Backward"));
    //Retranslate all the menus.
    m_subMenus[ShowSymbol]->setTitle(tr("Show Symbol"));
    m_subMenus[Zoom]->setTitle(tr("Zoom"));
    m_subMenus[Tab]->setTitle(tr("Tab"));
    m_subMenus[CollapseLevel]->setTitle(tr("Collapse Level"));
    m_subMenus[UncollapseLevel]->setTitle(tr("Uncollapse Level"));
    //Configure the dock widget.
    m_mapDockWidget->setWindowTitle(tr("Document Map"));
    m_folderDockWidget->setWindowTitle(tr("Folders"));
}

void KNViewMenu::onAlwaysOnTopToggle(bool checked)
{
    //Change the main window stay on top flag.
    auto flags = knGlobal->mainWindow()->windowFlags();
    //For checked.
    if(checked)
    {
        flags |= Qt::WindowStaysOnTopHint;
    }
    else
    {
        flags &= ~(Qt::WindowStaysOnTopHint);
    }
    knGlobal->mainWindow()->setWindowFlags(flags);
    //Show the window (why it needs to be hide?).
    knGlobal->mainWindow()->show();
}

void KNViewMenu::onFullScreenToggle(bool checked)
{
    if(checked)
    {
        //Show the full screen.
        knGlobal->mainWindow()->showFullScreen();
    }
    else
    {
        //Show the normal state.
        knGlobal->mainWindow()->showNormal();
    }
}

void KNViewMenu::onWordWrap(bool checked)
{
    knGlobal->setWrap(checked);
}

void KNViewMenu::onShowWhitespace(bool checked)
{
    //Disable all the other mode.
    knGlobal->blockSignals(true);
    m_menuItems[ShowEOF]->setChecked(false);
    m_menuItems[ShowAllChars]->setChecked(false);
    knGlobal->blockSignals(false);
    //Check the checked.
    if(checked)
    {
        knGlobal->setSymbolDisplayMode(KNTextEditor::SymbolSpaces);
    }
    else
    {
        knGlobal->setSymbolDisplayMode(KNTextEditor::SymbolNormal);
    }
}

void KNViewMenu::onShowEof(bool checked)
{
    //Disable all the other mode.
    knGlobal->blockSignals(true);
    m_menuItems[ShowWhiteSpace]->setChecked(false);
    m_menuItems[ShowAllChars]->setChecked(false);
    knGlobal->blockSignals(false);
    if(checked)
    {
        knGlobal->setSymbolDisplayMode(KNTextEditor::SymbolEndOfBlock);
    }
    else
    {
        knGlobal->setSymbolDisplayMode(KNTextEditor::SymbolNormal);
    }
}

void KNViewMenu::onShowAll(bool checked)
{
    //Disable all the other mode.
    knGlobal->blockSignals(true);
    m_menuItems[ShowWhiteSpace]->setChecked(false);
    m_menuItems[ShowEOF]->setChecked(false);
    knGlobal->blockSignals(false);
    if(checked)
    {
        knGlobal->setSymbolDisplayMode(KNTextEditor::SymbolAll);
    }
    else
    {
        knGlobal->setSymbolDisplayMode(KNTextEditor::SymbolNormal);
    }
}
