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
#include <QBoxLayout>
#include <QDir>
#include <QMenu>
#include <QStackedWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>

#include "kntabbar.h"
#include "knmainwindow.h"
#include "knuimanager.h"
#include "kntexteditor.h"
#include "knglobal.h"
#include "kneditmenu.h"
#include "knsearchmenu.h"
#include "knviewmenu.h"
#include "kncodecmenu.h"
#include "knsearchbar.h"
#include "knutil.h"
#include "knsearchbar.h"
#include "kntoolmenu.h"
#include "knwindowsmenu.h"
#include "knrecentfilerecorder.h"
#include "kntabswitcher.h"
#include "knfolderpanel.h"

#include "knfilemanager.h"

KNFileManager::KNFileManager(QWidget *parent) : QWidget(parent),
    m_menu(new QMenu(this)),
    m_editMenu(new KNEditMenu(this)),
    m_searchMenu(new KNSearchMenu(this)),
    m_viewMenu(new KNViewMenu(parentWidget())),
    m_codecMenu(new KNCodecMenu(this)),
    m_toolMenu(new KNToolMenu(this)),
    m_recent(new KNRecentFileRecorder(this)),
    m_tabBar(new KNTabBar(this)),
    m_windowsMenu(new KNWindowsMenu(m_tabBar, this)),
    m_editorPanel(new QStackedWidget(this)),
    m_tabSwitcher(new KNTabSwitcher(parent)),
    m_newCounter(0)
{
    //Set properties.
    setAcceptDrops(true);
    setContentsMargins(0, 0, 0, 0);
    //Construct the layout.
    QBoxLayout *mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    //Add tab manager to manager.
    mainLayout->addWidget(m_tabBar);
    mainLayout->addWidget(m_editorPanel, 1);
    mainLayout->addWidget(m_searchMenu->searchBar());
    //Link the search bar.
    connect(m_searchMenu->searchBar(), &KNSearchBar::requireSetFocus,
            this, &KNFileManager::setFocusOnEditor);
    connect(m_searchMenu->searchBar(), &KNSearchBar::requireQuickSearch,
            this, &KNFileManager::onQuickSearch);
    //Hide the search bar at default.
    m_searchMenu->searchBar()->hide();
    //Connect the tab bar with the panel.
    connect(m_tabBar, &KNTabBar::currentChanged,
            this, &KNFileManager::onTabChange);
    connect(m_tabBar, &KNTabBar::tabCloseRequested,
            this, &KNFileManager::closeEditor);
    connect(m_tabBar, &KNTabBar::tabMoved,
            [=](int from, int to)
    {
        //Remove the from widget.
        QWidget *widget = m_editorPanel->widget(from);
        m_editorPanel->removeWidget(widget);
        //Insert the panel to the new position.
        m_editorPanel->insertWidget(to, widget);
    });
    //Create the tab switcher shown shortcut.
    QAction *showSwitcher = new QAction(knGlobal->mainWindow());
    showSwitcher->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_Tab));
    showSwitcher->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(showSwitcher, &QAction::triggered, this, &KNFileManager::onShowSwitcher);
    knGlobal->mainWindow()->addAction(showSwitcher);
    //Link the switcher.
    connect(m_tabSwitcher, &KNTabSwitcher::requireShowEditor,
            this, &KNFileManager::onShowEditor);
    //Link the view menu.
    connect(m_viewMenu, &KNViewMenu::requireToTab,
            this, &KNFileManager::setCurrentTab);
    connect(m_viewMenu, &KNViewMenu::requireToNextTab,
            m_tabBar, &KNTabBar::goToNextTab);
    connect(m_viewMenu, &KNViewMenu::requireToPrevTab,
            m_tabBar, &KNTabBar::goToPrevTab);
    connect(m_viewMenu, &KNViewMenu::requireMoveBackward,
            m_tabBar, &KNTabBar::moveBackward);
    connect(m_viewMenu, &KNViewMenu::requireMoveForward,
            m_tabBar, &KNTabBar::moveForward);
    connect(m_viewMenu, &KNViewMenu::requireToShowSummary,
            this, &KNFileManager::showCurrentSummary);
    //Link the folder panel.
    KNFolderPanel *folderPanel = m_viewMenu->folderPanel();
    folderPanel->setFileManager(this);
    connect(folderPanel, &KNFolderPanel::requireOpen,
            this, &KNFileManager::openFile);
    connect(m_tabBar, &KNTabBar::currentChanged,
            folderPanel, &KNFolderPanel::onTraceFile);
    //Link the codec menu.
    connect(m_codecMenu, &KNCodecMenu::requireLoadAs,
            this, &KNFileManager::onReloadUseCodec);
    connect(m_codecMenu, &KNCodecMenu::requireSetCodec,
            this, &KNFileManager::onSetCodec);
    //Link the recent manager.
    connect(m_recent, &KNRecentFileRecorder::requireOpen,
            this, &KNFileManager::openFile);
    connect(m_recent, &KNRecentFileRecorder::requireOpenList,
            this, &KNFileManager::openFiles);
    //Link the translation.
    knUi->addTranslate(this, &KNFileManager::retranslate);

    //Construct the menu items.
    for (int i=0; i<MenuItemCount; ++i)
    {
        //Construct the menu item.
        m_menuItems[i] = new QAction(this);
    }
    for (int i=0; i<SubMenuCount; ++i)
    {
        m_subMenus[i] = new QMenu(this);
    }
    //Set the icon of the icon.
    m_menuItems[New]->setIcon(QIcon(":/icons/newFile.png"));
    m_menuItems[Open]->setIcon(QIcon(":/icons/openFile.png"));
    m_menuItems[Save]->setIcon(QIcon(":/icons/saveFile.png"));
    m_menuItems[SaveAll]->setIcon(QIcon(":/icons/saveAll.png"));
    m_menuItems[Close]->setIcon(QIcon(":/icons/closeFile.png"));
    m_menuItems[CloseAll]->setIcon(QIcon(":/icons/closeAll.png"));
    m_menuItems[Print]->setIcon(QIcon(":/icons/print.png"));
    //Add widget to menu.
    m_menu->addAction(m_menuItems[New]);
    m_menu->addAction(m_menuItems[Open]);
    m_menu->addAction(m_menuItems[ShowInExplorer]);
    m_menu->addAction(m_menuItems[OpenWithDefault]);
    m_menu->addAction(m_menuItems[OpenFolder]);
    m_menu->addAction(m_menuItems[ReloadFile]);
    m_menu->addAction(m_menuItems[Save]);
    m_menu->addAction(m_menuItems[SaveAs]);
    m_menu->addAction(m_menuItems[SaveACopyAs]);
    m_menu->addAction(m_menuItems[SaveAll]);
    m_menu->addAction(m_menuItems[Rename]);
    m_menu->addAction(m_menuItems[Close]);
    m_menu->addAction(m_menuItems[CloseAll]);
    m_menu->addMenu(m_subMenus[CloseMore]);
    m_subMenus[CloseMore]->addAction(m_menuItems[CloseAllElse]);
    m_subMenus[CloseMore]->addAction(m_menuItems[CloseAllLeft]);
    m_subMenus[CloseMore]->addAction(m_menuItems[CloseAllRight]);
    m_subMenus[CloseMore]->addAction(m_menuItems[CloseAllSaved]);
    m_menu->addSeparator();
    m_menu->addAction(m_menuItems[LoadSession]);
    m_menu->addAction(m_menuItems[SaveSession]);
    m_menu->addSeparator();
    m_menu->addAction(m_menuItems[Print]);
    m_menu->addAction(m_menuItems[PrintNow]);
    m_menu->addAction(m_recent->fileSeperator());
    for(int i=0; i<MAX_RECORDS; ++i)
    {
        m_menu->addAction(m_recent->fileAction(i));
    }
    m_menu->addAction(m_recent->opSeperator());
    for(int i=0; i<KNRecentFileRecorder::RecentActionCount; ++i)
    {
        m_menu->addAction(m_recent->opAction(i));
    }
    m_menu->addSeparator();
    m_menu->addAction(m_menuItems[Exit]);
    //Set shortcuts.
    m_menuItems[New]->setShortcut(QKeySequence::New);
    m_menuItems[Open]->setShortcut(QKeySequence::Open);
    m_menuItems[Save]->setShortcut(QKeySequence::Save);
    m_menuItems[SaveAs]->setShortcut(QKeySequence::SaveAs);
    m_menuItems[SaveAll]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_S));
    m_menuItems[Close]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_W));
    m_menuItems[CloseAll]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_W));
    m_menuItems[Print]->setShortcut(QKeySequence::Print);
    //Link actions.
    connect(m_menuItems[New], &QAction::triggered,
            this, &KNFileManager::createTab);
    connect(m_menuItems[Open], &QAction::triggered,
            this, &KNFileManager::openTabs);
    connect(m_menuItems[ShowInExplorer], &QAction::triggered, [=]
    {
        //Get the current editor.
        auto editor = currentEditor();
        if(editor->isOnDisk())
        {
            //Call the default editor.
            KNUtil::showInGraphicalShell(editor->filePath());
        }
    });
    connect(m_menuItems[OpenFolder], &QAction::triggered, this, &KNFileManager::onOpenFolder);
    connect(m_menuItems[OpenWithDefault], &QAction::triggered, [=]
    {
        //Get the current editor.
        auto editor = currentEditor();
        if(editor->isOnDisk())
        {
            //Call the default editor.
            KNUtil::openLocalFile(editor->filePath());
        }
    });
    connect(m_menuItems[Save], &QAction::triggered,
            this, &KNFileManager::save);
    connect(m_menuItems[SaveAs], &QAction::triggered,
            this, &KNFileManager::saveAs);
    connect(m_menuItems[SaveACopyAs], &QAction::triggered,
            this, &KNFileManager::saveCopyAs);
    connect(m_menuItems[SaveAll], &QAction::triggered,
            this, &KNFileManager::saveAll);
    connect(m_menuItems[Close], &QAction::triggered,
            this, &KNFileManager::closeTab);
    connect(m_menuItems[CloseAll], &QAction::triggered,
            [=]
    {
        closeAllTab();
        ensureEmptyTab();
    });
    connect(m_menuItems[CloseAllElse], &QAction::triggered,
            this, &KNFileManager::closeAllElse);
    connect(m_menuItems[CloseAllLeft], &QAction::triggered,
            this, &KNFileManager::closeAllLeft);
    connect(m_menuItems[CloseAllRight], &QAction::triggered,
            this, &KNFileManager::closeAllRight);
    connect(m_menuItems[CloseAllSaved], &QAction::triggered,
            this, &KNFileManager::closeAllSaved);
    connect(m_menuItems[ReloadFile], &QAction::triggered,
            this, &KNFileManager::onReloadCurrent);
    connect(m_menuItems[LoadSession], &QAction::triggered,
            this, &KNFileManager::onLoadSession);
    connect(m_menuItems[SaveSession], &QAction::triggered,
            this, &KNFileManager::onSaveSession);
    connect(m_menuItems[Print], &QAction::triggered,
            this, &KNFileManager::printCurrent);
    connect(m_menuItems[PrintNow], &QAction::triggered,
            this, &KNFileManager::printUseDefault);
    connect(m_menuItems[Exit], &QAction::triggered,
            knMainWindow, &KNMainWindow::close);
}

QString KNFileManager::currentWindowTitle() const
{
    //Extract the editor.
    KNTextEditor *editor = currentEditor();
    if(editor)
    {
        //Check the editor file path.
        if(editor->filePath().isEmpty())
        {
            return editor->documentTitle();
        }
        return QDir::toNativeSeparators(editor->filePath());
    }
    return QString();
}

QMenu *KNFileManager::fileMenu() const
{
    return m_menu;
}

QAction *KNFileManager::fileMenuItem(int index)
{
    return m_menuItems[index];
}

QMenu *KNFileManager::editMenu() const
{
    return m_editMenu;
}

QAction *KNFileManager::editMenuItem(int index)
{
    return m_editMenu->menuItem(index);
}

QMenu *KNFileManager::searchMenu() const
{
    return m_searchMenu;
}

QAction *KNFileManager::searchMenuItem(int index)
{
    return m_searchMenu->menuItem(index);
}

void KNFileManager::createTab()
{
    //Create the text editor for the panel.
    KNTextEditor *editor = new KNTextEditor(
                tr("new %1").arg(QString::number(++m_newCounter)),
                QString(), "UTF-8", this);
    //Create an editor tab.
    int tabId = createEditorTab(editor);
    //Switch to tab id.
    m_tabBar->setCurrentIndex(tabId);
}

void KNFileManager::openTabs()
{
    //Get the file path.
    QStringList filePaths = QFileDialog::getOpenFileNames(
                knMainWindow, tr("Open"), startPath(),
                knGlobal->fileDialogSuffix());
    //Open the file paths.
    openFiles(filePaths);
}

void KNFileManager::save()
{
    //Check whether the file is saved before.
    saveEditor(currentEditor());
}

void KNFileManager::saveAs()
{
    //Call the save as of the text editor.
    saveAsEditor(currentEditor());
}

void KNFileManager::saveCopyAs()
{
    //Get the new file path.
    KNTextEditor *editor=currentEditor();
    if(editor)
    {
        //Get the file path.
        QString filePath = QFileDialog::getSaveFileName(
                    knMainWindow, tr("Save Copy To"), startPath(),
                    knGlobal->fileDialogSuffix());
        if(!filePath.isEmpty())
        {
            editor->saveToFile(filePath);
        }
    }
}

void KNFileManager::saveAll()
{
    //Loop and save all the editor.
    for(int i=0; i<m_tabBar->count(); ++i)
    {
        //Save the editor.
        saveEditor(editorAt(i));
    }
}

void KNFileManager::closeTab()
{
    //Close the current editor.
    closeEditor(m_editorPanel->currentIndex());
}

bool KNFileManager::closeAllTab()
{
    //Close the entire range.
    return closeTabRange(0, m_tabBar->count());
}

void KNFileManager::closeAllElse()
{
    //Check tab bar index first.
    if(m_tabBar->count() == 1)
    {
        return;
    }
    //Close all the other tabs.
    QVector<int> tabIds;
    tabIds.reserve(m_tabBar->count() - 1);
    int currentId = m_tabBar->currentIndex();
    for(int i=0; i<m_tabBar->count(); ++i)
    {
        if(currentId != i)
        {
            tabIds.append(i);
        }
    }
    //Close all the tabs.
    closeTabs(tabIds);
}

void KNFileManager::closeAllLeft()
{
    //Check tab bar index first.
    if(m_tabBar->currentIndex() == 0)
    {
        return;
    }
    //Close the left range tabs.
    closeTabRange(0, m_tabBar->currentIndex());
}

void KNFileManager::closeAllRight()
{
    //Check tab bar index first.
    if(m_tabBar->currentIndex() == m_tabBar->count() - 1)
    {
        return;
    }
    //Close the left range tabs.
    closeTabRange(m_tabBar->currentIndex() + 1,
                  m_tabBar->count() - m_tabBar->currentIndex() - 1);
}

void KNFileManager::closeAllSaved()
{
    QVector<int> savedIds;
    //Loop for all the tabs.
    for(int i=m_editorPanel->count() - 1; i>-1; --i)
    {
        //Check whether the document is modified.
        KNTextEditor *editor = static_cast<KNTextEditor *>(
                    m_editorPanel->widget(i));
        //Check whether the document is modified.
        if(!editor->document()->isModified())
        {
            //Remove the tab and editor.
            removeEditorAndTab(i);
        }
    }
}

void KNFileManager::setCurrentTab(int tabId)
{
    //Change the tab bar index.
    m_tabBar->setCurrentIndex(tabId);
}

void KNFileManager::setFocusOnEditor()
{
    //Set the focus.
    auto editor = currentEditor();
    if(editor)
    {
        editor->setFocus();
    }
}

void KNFileManager::showCodecDialog()
{
    //Check the current editor is saved.
    auto editor = currentEditor();
    if(editor->isOnDisk())
    {
        //Show the codec dialog.
        m_codecMenu->showCodecDialog();
    }
}

void KNFileManager::showEvent(QShowEvent *event)
{
    //Check the tab bar count.
    if(!m_tabBar->count())
    {
        //Create a new file.
        createTab();
    }
    //Show the file manager.
    QWidget::showEvent(event);
}

void KNFileManager::dragEnterEvent(QDragEnterEvent *event)
{
    auto data = event->mimeData();
    if(data->hasUrls())
    {
        //We have to check whether those urls are local files.
        for(auto url : data->urls())
        {
            if(url.isLocalFile())
            {
                event->acceptProposedAction();
                return;
            }
        }
    }
}

void KNFileManager::dropEvent(QDropEvent *event)
{
    //Load the file from the drop event.
    auto urls = event->mimeData()->urls();
    QStringList paths;
    paths.reserve(urls.size());
    for(auto url : urls)
    {
        //Check whether the url is local file.
        if(url.isLocalFile())
        {
            paths.append(url.toLocalFile());
        }
    }
    //Open the file paths.
    openFiles(paths);
}

void KNFileManager::retranslate()
{
    //Update the text.
    m_menu->setTitle(tr("&File"));
    //Update the action labels.
    m_menuItems[New]->setText(tr("&New"));
    m_menuItems[Open]->setText(tr("&Open..."));
    m_menuItems[ShowInExplorer]->setText(tr("Open Containing Folder"));
    m_menuItems[OpenWithDefault]->setText(tr("Open in Default Viewer"));
    m_menuItems[OpenFolder]->setText(tr("Open Folder..."));
    m_menuItems[ReloadFile]->setText(tr("Re&load from Disk"));
    m_menuItems[Save]->setText(tr("&Save"));
    m_menuItems[SaveAs]->setText(tr("Save &As..."));
    m_menuItems[SaveACopyAs]->setText(tr("Save a Copy As..."));
    m_menuItems[SaveAll]->setText(tr("Sav&e All"));
    m_menuItems[Rename]->setText(tr("Rename..."));
    m_menuItems[Close]->setText(tr("&Close"));
    m_menuItems[CloseAll]->setText(tr("Clo&se All"));
    m_menuItems[CloseAllElse]->setText(tr("Close All but Active Document"));
    m_menuItems[CloseAllLeft]->setText(tr("Close All to the Left"));
    m_menuItems[CloseAllRight]->setText(tr("Close All to the Right"));
    m_menuItems[CloseAllSaved]->setText(tr("Close All Unchanged"));
    m_menuItems[LoadSession]->setText(tr("Load Session..."));
    m_menuItems[SaveSession]->setText(tr("Save Session..."));
    m_menuItems[Print]->setText(tr("Print..."));
    m_menuItems[PrintNow]->setText(tr("Print Now"));
    m_menuItems[Exit]->setText(tr("E&xit"));

    m_subMenus[CloseMore]->setTitle(tr("Close More"));
}

void KNFileManager::onOpenFolder()
{
    //Open the folder.
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"));
    if(folderPath.isEmpty())
    {
        return;
    }
    //Set the folder panel path.
    m_viewMenu->setAndShowFolder(folderPath);
}

void KNFileManager::onShowSwitcher()
{
    //Check editor count.
    if(m_tabBar->count() > 1)
    {
        //Reposition the tab switcher.
        KNMainWindow *mainWindow = knGlobal->mainWindow();
        m_tabSwitcher->resize(m_tabSwitcher->sizeHint());
        m_tabSwitcher->move(
                    mainWindow->x() + ((mainWindow->width() - m_tabSwitcher->width()) >> 1),
                    mainWindow->y() + ((mainWindow->height() - m_tabSwitcher->height()) >> 1));
        //Show the switcher and set focus.
        m_tabSwitcher->show();
        m_tabSwitcher->activateWindow();
        m_tabSwitcher->setFocus();
        m_tabSwitcher->setCurrentIndex(m_tabSwitcher->model()->index(1, 0));
    }
}

void KNFileManager::onShowEditor(KNTextEditor *editor)
{
    //Find the editor in the panel.
    int editorId = m_editorPanel->indexOf(editor);
    if(Q_UNLIKELY(editorId != -1))
    {
        //Show the editor.
        setCurrentTab(editorId);
    }
}

void KNFileManager::onReloadCurrent()
{
    //Check the current editor.
    auto editor = currentEditor();
    if(editor)
    {
        //Document edited check.
        if(editor->isOnDisk() && editor->document()->isModified())
        {
            //Ask whether should we still reload.
            int result = QMessageBox::question(
                        this, tr("Reload"),
                        tr("Are you sure you want to reload the current file "
                           "and lose the changes?"));
            if(result == QMessageBox::No)
            {
                return;
            }
        }
        //Now start reload the data.
        editor->loadFrom(editor->filePath());
    }
}

void KNFileManager::onTabChange(int index)
{
    if(index > -1)
    {
        //Switch the widget.
        m_editorPanel->setCurrentIndex(index);
        //Set focus to the widget.
        auto editor = currentEditor();
        //Update the editor with the global settings.
        editor->setFocus();
        editor->activateWindow();
        //Move the editor to the top.
        m_tabSwitcher->topEditor(editor);
        //Update the menu target.
        m_editMenu->setEditor(editor);
        m_searchMenu->setEditor(editor);
        m_viewMenu->setEditor(editor);
        //Update reload state.
        updateFileItemsEnabled(editor->isOnDisk());
    }
    //Emit the title change signal.
    emit titleChange(currentWindowTitle());
    emit editorChanged();
}

void KNFileManager::onEditorTitleChange()
{
    //Cast sender from as a text editor.
    KNTextEditor *editor = static_cast<KNTextEditor *>(sender());
    //Update the editor index.
    int editorId = m_editorPanel->indexOf(editor);
    //Change the title.
    m_tabBar->setTabText(editorId, editor->documentTitle());
    //Check whether this is the current one.
    if(editorId == m_editorPanel->currentIndex())
    {
        emit titleChange(currentWindowTitle());
    }
}

void KNFileManager::onEditorModified(bool modified)
{
    //Cast sender from as a text editor.
    KNTextEditor *editor = static_cast<KNTextEditor *>(sender());
    //Update the editor index.
    int editorId = m_editorPanel->indexOf(editor);
    //Update the tab icon.
    m_tabBar->setModifiedState(editorId, modified);
    //Check the editor state.
    if(editor == currentEditor())
    {
        //Update the reload state.
        updateFileItemsEnabled(editor->isOnDisk());
    }
}

void KNFileManager::onEditorReadOnly(bool isReadOnly)
{
    //Cast sender from as a text editor.
    KNTextEditor *editor = static_cast<KNTextEditor *>(sender());
    //Update the editor index.
    int editorId = m_editorPanel->indexOf(editor);
    //Update the tab icon.
    if(isReadOnly)
    {
        m_tabBar->setReadOnlyState(editorId);
    }
    else
    {
        //Set the modified state.
        m_tabBar->setModifiedState(editorId, editor->document()->isModified());
    }
}

KNTextEditor *KNFileManager::currentEditor() const
{
    return editorAt(m_tabBar->currentIndex());
}

KNTextEditor *KNFileManager::locateEditor(const QString &pathOrTitle)
{
    //Check the file path exist.
    QFileInfo pathInfo(pathOrTitle);
    if(pathInfo.isFile())
    {
        //Find the path in all the editor.
        for(int i=0; i<m_editorPanel->count(); ++i)
        {
            //Check the title.
            auto editor = editorAt(i);
            if(editor->filePath() == pathOrTitle)
            {
                //Switch to the editor.
                setCurrentTab(i);
                return editor;
            }
        }
        //The editor does not exist, then we open the file.
        int tabId = createFileTab(pathOrTitle);
        setCurrentTab(tabId);
        //Extract the editor from the tab id.
        return editorAt(tabId);
    }
    //Or else, it is the title and not saved, check the title.
    for(int i=0; i<m_editorPanel->count(); ++i)
    {
        //Check the title.
        auto editor = editorAt(i);
        if(editor->documentTitle() == pathOrTitle)
        {
            setCurrentTab(i);
            return editor;
        }
    }
    //Then, the editor does not exist.
    return nullptr;
}

QVector<KNTextEditor *> KNFileManager::allEditors() const
{
    //Construct the editor the list.
    QVector<KNTextEditor *> editors;
    editors.reserve(m_editorPanel->count());
    //Append the editor to the list.
    for(int i=0; i<m_editorPanel->count(); ++i)
    {
        editors.append(editorAt(i));
    }
    return editors;
}

KNTextEditor *KNFileManager::editorAt(int index) const
{
    return static_cast<KNTextEditor *>(m_editorPanel->widget(index));
}

void KNFileManager::linkEditor(KNTextEditor *editor)
{
    //Link the editor.
    editor->addLink(connect(editor, &KNTextEditor::documentTitleChange,
                            this, &KNFileManager::onEditorTitleChange));
    editor->addLink(connect(editor, &KNTextEditor::modificationChanged,
                            this, &KNFileManager::onEditorModified));
    editor->addLink(connect(editor, &KNTextEditor::readOnlyChange,
                            this, &KNFileManager::onEditorReadOnly));
}

void KNFileManager::updateFileItemsEnabled(bool isSaved)
{
    //Update the menu items.
    m_menuItems[OpenWithDefault]->setEnabled(isSaved);
    m_menuItems[ShowInExplorer]->setEnabled(isSaved);
    m_menuItems[ReloadFile]->setEnabled(isSaved);
}

bool KNFileManager::isEditorNewEmpty(KNTextEditor *editor) const
{
    return !editor->isOnDisk() && !editor->document()->isModified();
}

int KNFileManager::createEditorTab(KNTextEditor *editor)
{
    //Link the editor.
    linkEditor(editor);
    //Add editor to panel.
    m_editorPanel->addWidget(editor);
    //Add editor to tab switcher.
    m_tabSwitcher->addEditor(editor);
    //Add a new tab.
    return m_tabBar->createTab(editor->documentTitle());
}

bool KNFileManager::saveEditor(KNTextEditor *editor)
{
    if(editor)
    {
        //Check whether the editor is saved.
        if(editor->isOnDisk())
        {
            //Directly save the file.
            return editor->save();
        }
        //Need to save as.
        return saveAsEditor(editor);
    }
    return false;
}

bool KNFileManager::saveAsEditor(KNTextEditor *editor)
{
    if(editor)
    {
        //Get the file path.
        QString filePath = QFileDialog::getSaveFileName(
                    knMainWindow, tr("Save As"), startPath(),
                    knGlobal->fileDialogSuffix());
        if(!filePath.isEmpty())
        {
            return editor->saveAs(filePath);
        }
    }
    return false;
}

int KNFileManager::openFileAt(const QString &filePath)
{
    //Check whether the file is already opened.
    for(int i=0; i<m_editorPanel->count(); ++i)
    {
        KNTextEditor *editor = editorAt(i);
        if(editor->filePath() == filePath)
        {
            //Show the tab.
            return i;
        }
    }
    //Truly open the file and switch to the tab.
    return createFileTab(filePath);
}

QString KNFileManager::startPath() const
{
    QString startPath;
    auto editor = currentEditor();
    if(editor && !editor->filePath().isEmpty())
    {
        startPath = QFileInfo(editor->filePath()).absolutePath();
    }
    return startPath;
}

QMenu *KNFileManager::windowsMenu() const
{
    return m_windowsMenu;
}

QMenu *KNFileManager::toolMenu() const
{
    return m_toolMenu;
}

QMenu *KNFileManager::viewMenu() const
{
    return m_viewMenu;
}

QAction *KNFileManager::viewMenuItem(int index)
{
    return m_viewMenu->menuItem(index);
}

QMenu *KNFileManager::codecMenu() const
{
    return m_codecMenu;
}

bool KNFileManager::closeEditor(int editorId)
{
    //Get the editor.
    auto editor = editorAt(editorId);
    if(Q_UNLIKELY(editor == nullptr))
    {
        return true;
    }
    //If this is an empty new file and it is the last one, do not close.
    if(isEditorNewEmpty(editor) && m_tabBar->count() == 1)
    {
        //Treat as already closed.
        return true;
    }
    //Check whether the editor is modified or not.
    if(editor->document()->isModified())
    {
        //Emit the signal here.
        auto saveCheck =
                QMessageBox::question(
                    this, tr("Save"),
                    tr("Save file %1").arg(editor->documentTitle()),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                    QMessageBox::Yes);
        switch(saveCheck)
        {
        case QMessageBox::Cancel:
            return false;
        case QMessageBox::Yes:
        {
            if(!saveEditor(editor))
            {
                //Failed to save the file, cannot close the file.
                return false;
            }
        }
        default:
            break;
        }
    }
    //Push the path to recorder.
    if(!editor->filePath().isEmpty())
    {
        m_recent->pushRecord(QDir::toNativeSeparators(editor->filePath()));
    }
    //Push the file path to recent records.
    removeEditorAndTab(editorId);
    //Check whether there is no editor existed.
    if(m_tabBar->count() == 0)
    {
        //Create a new empty tab.
        createTab();
    }
    return true;
}

void KNFileManager::printCurrent()
{
    //Call the print dialog.
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    //Show the print dialog.
    if(dialog.exec() == QDialog::Accepted)
    {
        //Use the text editor built-in print.
        auto editor = currentEditor();
        if(editor)
        {
            editor->print(&printer);
        }
    }
}

void KNFileManager::printUseDefault()
{
    //Use the printer info to get the default configure.
    QPrinterInfo printerInfo = QPrinterInfo::defaultPrinter();
    if(printerInfo.isNull())
    {
        return;
    }
    //Use the text editor built-in print.
    QPrinter printer(printerInfo);
    auto editor = currentEditor();
    if(editor)
    {
        editor->print(&printer);
    }
}

int KNFileManager::createFileTab(const QString &filePath)
{
    //Check whether the current text editor is an empty file.
    auto editor = currentEditor();
    if(editor && !editor->isOnDisk() && !editor->document()->isModified())
    {
        //We could use the current editor.
        editor->loadFrom(filePath);
        return m_tabBar->currentIndex();
    }
    //Update the tab id.
    return createEditorTab(new KNTextEditor(
                               QString(), filePath, QString(), this));
}

void KNFileManager::openFile(const QString &filePath)
{
    //Open the file, switch to the tab.
    m_tabBar->setCurrentIndex(openFileAt(filePath));
}

void KNFileManager::openFiles(const QStringList &filePaths)
{
    //Construct the file path set.
    QHash<QString, int> holdingPaths;
    for(int i=0; i<m_editorPanel->count(); ++i)
    {
        KNTextEditor *editor = editorAt(i);
        if(!editor->filePath().isEmpty())
        {
            holdingPaths.insert(editor->filePath(), i);
        }
    }
    int tabId = -1;
    for(QString filePath : filePaths)
    {
        //Check whether the file already opened.
        int holdingId = holdingPaths.value(filePath, -1);
        if(holdingId != -1)
        {
            //Update the tab Id.
            tabId = holdingPaths.value(filePath);
            continue;
        }
        //Open each file and update the last tab id.
        tabId = createFileTab(filePath);
    }
    //Check the tab Id.
    if(tabId != -1)
    {
        //Switch to the file.
        m_tabBar->setCurrentIndex(tabId);
    }
}

void KNFileManager::showCurrentSummary()
{
    //Check whether the editor is null.
    auto editor = currentEditor();
    if(editor)
    {
        editor->showSummary();
    }
}

void KNFileManager::onReloadUseCodec(const QByteArray &codecName)
{
    //Set the codec name.
    auto editor = currentEditor();
    if(editor)
    {
        editor->loadUseCodec(codecName);
    }
}

void KNFileManager::onSetCodec(const QByteArray &codecName)
{
    //Set the codec name.
    auto editor = currentEditor();
    if(editor)
    {
        editor->setCodecName(codecName);
    }
}

void KNFileManager::onQuickSearch(const QString &keywords,
                                  Qt::CaseSensitivity cs, int position)
{
    //Check the current editor.
    auto editor = currentEditor();
    if(editor)
    {
        //Do quick search with the cursor position.
        editor->quickSearch(keywords, cs, position);
    }
}

void KNFileManager::onLoadSession()
{
    //Get the session file path.
    QString sessionPath = QFileDialog::getOpenFileName(
                knMainWindow, tr("Load Session From"), startPath());
    if(sessionPath.isEmpty())
    {
        return;
    }
    //Load the json object from the file.
    QFile sessionFile(sessionPath);
    if(!sessionFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Load failed"),
                                 tr("Please check if session file is opened in another program."));
        return;
    }
    //Now Load the content.
    QJsonObject sessionObj = QJsonDocument::fromJson(sessionFile.readAll()).object();
    sessionFile.close();
    int currentId = sessionObj.value("Current").toInt();
    //Parse the session object and load the data.
    QJsonArray tabInfo = sessionObj.value("Tabs").toArray();
    for(int i=0; i<tabInfo.size(); ++i)
    {
        QJsonObject editorInfo = tabInfo.at(i).toObject();
        //Open the file from the tab info.
        int tabId = openFileAt(editorInfo.value("Path").toString());
        //Restore the session states.
        editorAt(tabId)->loadSessionStates(editorInfo);
        //Check the i is the current id.
        if(i == currentId)
        {
            m_tabBar->setCurrentIndex(tabId);
        }
    }
}

void KNFileManager::onSaveSession()
{
    //Get the session file path.
    QString sessionPath = QFileDialog::getSaveFileName(
                knMainWindow, tr("Save Session To"), startPath());
    if(sessionPath.isEmpty())
    {
        return;
    }
    //Construct the session file.
    QJsonArray tabInfo;
    for(int i=0; i<m_tabBar->count(); ++i)
    {
        //Write the current tab information to file manager.
        tabInfo.append(editorAt(i)->sessionObject());
    }
    QJsonObject sessionObj;
    sessionObj.insert("Current", m_tabBar->currentIndex());
    sessionObj.insert("Tabs", tabInfo);
    //Write tab info to the session file.
    QFile sessionFile(sessionPath);
    if(!sessionFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Save failed"),
                                 tr("Please check if session file is opened in another program."));
        return;
    }
    //Now Save the content.
    sessionFile.write(QJsonDocument(sessionObj).toJson());
    sessionFile.close();
}

void KNFileManager::removeEditorAndTab(int index)
{
    //Extract the editor widget.
    auto editor = static_cast<KNTextEditor *>(m_editorPanel->widget(index));
    //Remove the tab.
    m_tabBar->removeTab(index);
    //Remove the editor.
    m_editorPanel->removeWidget(editor);
    //Remove the editor from the switcher.
    m_tabSwitcher->removeEditor(editor);
    //Disconnect all the editor connections.
    editor->removeAllLinks();
    //Recover the editor memory.
    editor->deleteLater();
}

void KNFileManager::ensureEmptyTab()
{
    //Check whether the tab bar is already empty.
    if(m_tabBar->count() == 0)
    {
        createTab();
    }
}

bool KNFileManager::closeTabRange(int start, int length)
{
    QVector<int> tabIds;
    for(int i=start; i<start+length; ++i)
    {
        tabIds.append(i);
    }
    return closeTabs(tabIds);
}

bool KNFileManager::closeTabs(QVector<int> tabIndexs)
{
    QVector<int> unsavedIds;
    //Save all first.
    for(int i=0; i<tabIndexs.size(); ++i)
    {
        if(editorAt(tabIndexs.at(i))->document()->isModified())
        {
            unsavedIds.append(tabIndexs.at(i));
        }
    }
    //Check the unsaved ID length.
    int allOption = 0;
    //Ask all the unsaved Ids.
    for(int i=0; i<unsavedIds.length(); ++i)
    {
        auto editor = editorAt(unsavedIds.at(i));
        //Check for default options.
        if(allOption == QMessageBox::YesToAll)
        {
            //Just save the document.
            if(editor->isOnDisk())
            {
                editor->save();
            }
            else
            {
                //Show the current tab.
                setCurrentTab(unsavedIds.at(i));
                //Save the editor.
                if(!saveAsEditor(editor))
                {
                    //Failed to save all.
                    return false;
                }
            }
        }
        //Close the tab.
        if(i != unsavedIds.length() - 1)
        {
            //Ask for options.
            if(allOption == 0)
            {
                //Only ask about the target file.
                setCurrentTab(unsavedIds.at(i));
                //Ask for the option.
                auto saveCheck =
                        QMessageBox::question(
                            this, tr("Save"),
                            tr("Save file\n%1").arg(
                                editor->filePath().isEmpty() ?
                                    editor->documentTitle() :
                                    editor->filePath()),
                            QMessageBox::Yes | QMessageBox::No |
                            QMessageBox::Cancel |
                            QMessageBox::YesToAll | QMessageBox::NoToAll,
                            QMessageBox::Yes);

                switch(saveCheck)
                {
                case QMessageBox::Cancel:
                    //Pause all.
                    return false;
                case QMessageBox::YesToAll:
                case QMessageBox::Yes:
                {
                    if(!saveEditor(editor))
                    {
                        //Failed to save the file, cannot close the file.
                        return false;
                    }
                    if(saveCheck == QMessageBox::YesToAll)
                    {
                        allOption = saveCheck;
                    }
                    break;
                }
                case QMessageBox::No:
                    continue;
                case QMessageBox::NoToAll:
                    allOption = saveCheck;
                    break;
                default:
                    break;
                }
            }
            //Check for default options.
            if(allOption == QMessageBox::NoToAll)
            {
                //Only ask about the target file.
                setCurrentTab(unsavedIds.at(i));
                break;
            }
        }
        else
        {
            //Switch and save the editor.
            setCurrentTab(unsavedIds.at(i));
            //Save the editor.
            if(!closeEditor(unsavedIds.at(i)))
            {
                return false;
            }
        }
    }
    //If this is an empty new file and it is the last one, do not close.
    if(m_tabBar->count() == 1 && isEditorNewEmpty(editorAt(0)))
    {
        //Treat as already closed.
        return true;
    }
    //Sort all the index.
    std::sort(tabIndexs.begin(), tabIndexs.end());
    //Close all the tabs from the back.
    QStringList filePaths;
    for(auto i=tabIndexs.size() - 1; i > -1; --i)
    {
        //Fetch the tab id.
        int tabId = tabIndexs.at(i);
        //Append the file path.
        auto editor = static_cast<KNTextEditor *>(m_editorPanel->widget(tabId));
        if(!editor->filePath().isEmpty())
        {
            filePaths.append(editor->filePath());
        }
        //Remove the tab and editor.
        removeEditorAndTab(tabId);
    }
    //Append the file paths.
    m_recent->pushRecords(filePaths);
    //Close all the tab successfully, then complete.
    return true;
}
