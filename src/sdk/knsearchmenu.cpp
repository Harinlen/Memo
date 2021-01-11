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
#include <QAction>
#include <QApplication>
#include <QClipboard>

#include "kntextblockdata.h"
#include "kntexteditor.h"
#include "knuimanager.h"
#include "knglobal.h"
#include "kngotowindow.h"
#include "knfindwindow.h"
#include "knsearchbar.h"
#include "knmainwindow.h"
#include "knsearchresult.h"

#include "knsearchmenu.h"

KNSearchMenu::KNSearchMenu(QWidget *parent) : QMenu(parent),
    m_searchResult(new KNSearchResult(parentWidget())),
    m_searchWindow(new KNFindWindow(m_searchResult, parent)),
    m_gotoWindow(new KNGotoWindow(parent)),
    m_searchBar(new KNSearchBar(parent)),
    m_editor(nullptr)
{
    //Construct actions and items.
    for(int i=0; i<SearchMenuItemCount; ++i)
    {
        m_menuItems[i] = new QAction(parentWidget());
    }
    for(int i=0; i<SearchSubMenuCount; ++i)
    {
        m_subMenus[i] = new QMenu(this);
    }
    //Add to main window.
    knGlobal->mainWindow()->addDockWidget(Qt::BottomDockWidgetArea,
                                          m_searchResult);
    //Add actions.
    addAction(m_menuItems[Find]);
    addAction(m_menuItems[FindInFiles]);
    addAction(m_menuItems[FindNext]);
    addAction(m_menuItems[FindPrev]);
    addAction(m_menuItems[SelectAndFindNext]);
    addAction(m_menuItems[SelectAndFindPrev]);
    addAction(m_menuItems[FindVolatileNext]);
    addAction(m_menuItems[FindVolatilePrev]);
    addAction(m_menuItems[Replace]);
    addAction(m_menuItems[IncrementSearch]);
    addAction(m_menuItems[SearchResults]);
    addAction(m_menuItems[NextResult]);
    addAction(m_menuItems[PrevResult]);
    addAction(m_menuItems[GoTo]);
    addAction(m_menuItems[GoToMatch]);
    addAction(m_menuItems[SelectBetween]);
//    addAction(m_menuItems[Mark]);
//    addSeparator();
//    addMenu(m_subMenus[MarkAll]);
//    addMenu(m_subMenus[UnmarkAll]);
//    addMenu(m_subMenus[JumpUp]);
//    addMenu(m_subMenus[JumpDown]);
    addSeparator();
    addMenu(m_subMenus[Bookmark]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkToggle]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkNext]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkPrev]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkClear]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkCut]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkCopy]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkReplace]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkRemoveAll]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkRemoveLeft]);
    m_subMenus[Bookmark]->addAction(m_menuItems[BookmarkInverse]);
    addSeparator();
    addAction(m_menuItems[FindChars]);
    //Link translator.
    knUi->addTranslate(this, &KNSearchMenu::retranslate);
    //Set icons.
    m_menuItems[Find]->setIcon(QIcon(":/icons/find.png"));
    m_menuItems[Replace]->setIcon(QIcon(":/icons/replace.png"));
    //Set shortcuts.
    m_menuItems[Find]->setShortcut(QKeySequence::Find);
    m_menuItems[FindNext]->setShortcut(QKeySequence::FindNext);
    m_menuItems[FindPrev]->setShortcut(QKeySequence::FindPrevious);
    m_menuItems[FindVolatileNext]->setShortcut(QKeySequence(KNG::CTRL | KNG::ALT | Qt::Key_F3));
    m_menuItems[FindVolatilePrev]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | KNG::ALT | Qt::Key_F3));
    m_menuItems[Replace]->setShortcut(QKeySequence::Replace);
    m_menuItems[SelectAndFindNext]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_F3));
    m_menuItems[SelectAndFindPrev]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_F3));
    m_menuItems[IncrementSearch]->setShortcut(QKeySequence(KNG::CTRL | KNG::ALT | Qt::Key_I));
    m_menuItems[GoTo]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_G));
    m_menuItems[BookmarkToggle]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_F2));
    m_menuItems[BookmarkNext]->setShortcut(QKeySequence(Qt::Key_F2));
    m_menuItems[BookmarkPrev]->setShortcut(QKeySequence(KNG::SHIFT | Qt::Key_F2));
    //Link actions.
    connect(m_menuItems[Find], &QAction::triggered, this, &KNSearchMenu::onFind);
    connect(m_menuItems[FindInFiles], &QAction::triggered, this, &KNSearchMenu::onFindInFiles);
    connect(m_menuItems[FindNext], &QAction::triggered, m_searchWindow, &KNFindWindow::findNext);
    connect(m_menuItems[FindPrev], &QAction::triggered, m_searchWindow, &KNFindWindow::findPrevious);
    connect(m_menuItems[FindVolatileNext], &QAction::triggered, m_searchBar, &KNSearchBar::findNext);
    connect(m_menuItems[FindVolatilePrev], &QAction::triggered, m_searchBar, &KNSearchBar::findPrevious);
    connect(m_menuItems[Replace], &QAction::triggered, this, &KNSearchMenu::onReplace);
    connect(m_menuItems[IncrementSearch], &QAction::triggered, m_searchBar, &KNSearchBar::showAndSetFocus);
    connect(m_menuItems[GoTo], &QAction::triggered, this, &KNSearchMenu::onGoto);
//    connect(m_menuItems[Mark], &QAction::triggered, this, &KNSearchMenu::onMark);
    connect(m_menuItems[BookmarkToggle], &QAction::triggered, this, &KNSearchMenu::onBookmarkToggle);
    connect(m_menuItems[BookmarkNext], &QAction::triggered, this, &KNSearchMenu::onBookmarkNext);
    connect(m_menuItems[BookmarkPrev], &QAction::triggered, this, &KNSearchMenu::onBookmarkPrev);
    connect(m_menuItems[BookmarkClear], &QAction::triggered, this, &KNSearchMenu::onBookmarkClear);
    connect(m_menuItems[BookmarkCut], &QAction::triggered, this, &KNSearchMenu::onBookmarkCut);
    connect(m_menuItems[BookmarkCopy], &QAction::triggered, this, &KNSearchMenu::onBookmarkCopy);
    connect(m_menuItems[BookmarkReplace], &QAction::triggered, this, &KNSearchMenu::onBookmarkReplace);
    connect(m_menuItems[BookmarkRemoveAll], &QAction::triggered, [=] { onBookmarkRemove(false); });
    connect(m_menuItems[BookmarkRemoveLeft], &QAction::triggered, [=] { onBookmarkRemove(true); });
    connect(m_menuItems[BookmarkInverse], &QAction::triggered, this, &KNSearchMenu::onBookmarkInverse);
}

void KNSearchMenu::retranslate()
{
    setTitle(tr("&Search"));
    //Update the actions.
    m_menuItems[Find]->setText(tr("&Find..."));
    m_menuItems[FindInFiles]->setText(tr("Find in Files..."));
    m_menuItems[FindNext]->setText(tr("Find &Next"));
    m_menuItems[FindPrev]->setText(tr("Find &Previous"));
    m_menuItems[SelectAndFindNext]->setText(tr("Select and Find Next"));
    m_menuItems[SelectAndFindPrev]->setText(tr("Select and Find Previous"));
    m_menuItems[FindVolatileNext]->setText(tr("Find (Volatile) Next"));
    m_menuItems[FindVolatilePrev]->setText(tr("Find (Volatile) Previous"));
    m_menuItems[Replace]->setText(tr("&Replace..."));
    m_menuItems[IncrementSearch]->setText(tr("&Increamental Search"));
    m_menuItems[SearchResults]->setText(tr("Search Results Window"));
    m_menuItems[NextResult]->setText(tr("Next Search Result"));
    m_menuItems[PrevResult]->setText(tr("Previous Search Result"));
    m_menuItems[GoTo]->setText(tr("&Go to..."));
    m_menuItems[GoToMatch]->setText(tr("Go to Matching Brace"));
    m_menuItems[SelectBetween]->setText(tr("Select All Between Matching Braces"));
    m_menuItems[Mark]->setText(tr("Mar&k..."));
    m_menuItems[FindChars]->setText(tr("Find characters in range..."));
    m_subMenus[MarkAll]->setTitle(tr("Mark All"));
    m_subMenus[UnmarkAll]->setTitle(tr("Unmark All"));
    m_subMenus[JumpUp]->setTitle(tr("Jump up"));
    m_subMenus[JumpDown]->setTitle(tr("Jump down"));
    m_subMenus[Bookmark]->setTitle(tr("Bookmark"));
    m_menuItems[BookmarkToggle]->setText(tr("Toggle Bookmark"));
    m_menuItems[BookmarkNext]->setText(tr("Next Bookmark"));
    m_menuItems[BookmarkPrev]->setText(tr("Previous Bookmark"));
    m_menuItems[BookmarkClear]->setText(tr("Clear All Bookmarks"));
    m_menuItems[BookmarkCut]->setText(tr("Cut Bookmarked Lines"));
    m_menuItems[BookmarkCopy]->setText(tr("Copy Bookmarked Lines"));
    m_menuItems[BookmarkReplace]->setText(tr("Paste to (Replace) Bookmarked Lines"));
    m_menuItems[BookmarkRemoveAll]->setText(tr("Remove Bookmarked Lines"));
    m_menuItems[BookmarkRemoveLeft]->setText(tr("Remove Unmarked Lines"));
    m_menuItems[BookmarkInverse]->setText(tr("Inverse Bookmark"));
}

void KNSearchMenu::onFind()
{
    if(m_editor)
    {
        //Show the widget in find mode.
        m_searchWindow->showMode(KNFindWindow::ModeFind);
    }
}

void KNSearchMenu::onFindInFiles()
{
    if(m_editor)
    {
        //Show the widget in find mode.
        m_searchWindow->showMode(KNFindWindow::ModeFindInFiles);
    }
}

void KNSearchMenu::onGoto()
{
    if(m_editor)
    {
        //Show the goto dialog.
        int result = m_gotoWindow->showGoto(m_editor);
        //Check the result.
        if(result == QDialog::Rejected)
        {
            return;
        }
        //Based on the existed result.
        int targetValue = m_gotoWindow->targetValue();
        QTextCursor tc = m_editor->textCursor();
        auto doc = m_editor->document();
        //Check the mode.
        if(m_gotoWindow->isLine())
        {
            //Move the block.
            QTextBlock block = doc->findBlockByNumber(targetValue - 1);
            tc.setPosition(block.position());
        }
        else
        {
            //Directly move to the position.
            tc.setPosition(targetValue);
        }
        m_editor->setTextCursor(tc);
    }
}

void KNSearchMenu::onSelectFindNext()
{
    if(m_editor)
    {
        //Get the current selection.
        QTextCursor tc = m_editor->textCursor();
        if(tc.hasSelection())
        {
            //Set the selection text to search window.
            m_searchWindow->setSearchText(tc.selectedText());
            //Perform the search.
            m_searchWindow->findNext();
        }
    }
}

void KNSearchMenu::onSelectFindPrevious()
{
    if(m_editor)
    {
        //Get the current selection.
        QTextCursor tc = m_editor->textCursor();
        if(tc.hasSelection())
        {
            //Set the selection text to search window.
            m_searchWindow->setSearchText(tc.selectedText());
            //Perform the search.
            m_searchWindow->findPrevious();
        }
    }
}

void KNSearchMenu::onReplace()
{
    if(m_editor)
    {
        //Show the widget in find mode.
        m_searchWindow->showMode(KNFindWindow::ModeReplace);
    }
}

void KNSearchMenu::onMark()
{
    if(m_editor)
    {
        //Show the widget in find mode.
        m_searchWindow->showMode(KNFindWindow::ModeMark);
    }
}

void KNSearchMenu::onBookmarkToggle()
{
    if(m_editor)
    {
        //Change the bookmark status.
        auto blockData = m_editor->currentBlockData();
        //Change the block data information.
        blockData->hasBookmark = !blockData->hasBookmark;
        //Update the entire editor.
        m_editor->update();
    }
}

static bool checkBookmark(KNTextEditor *editor, QTextBlock &block,
                          QTextCursor &cursor)
{
    //Check the block data.
    if(editor->blockData(block)->hasBookmark)
    {
        //Stop and move the cursor to the position.
        cursor.setPosition(block.position());
        editor->setTextCursor(cursor);
        //Ensure the cursor to be visible.
        editor->ensureCursorVisible();
        return true;
    }
    return false;
}

void KNSearchMenu::onBookmarkNext()
{
    if(m_editor)
    {
        //Loop from the current block to the end of the document.
        QTextCursor tc = m_editor->textCursor();
        auto doc = m_editor->document();
        QTextBlock block = tc.block();
        int currentBlock = block.blockNumber();
        block = block.next();
        while(block.isValid())
        {
            //Check the block data.
            if(checkBookmark(m_editor, block, tc))
            {
                return;
            }
            //Find the next item.
            block = block.next();
        }
        //Or else we have to find from the start.
        block = doc->firstBlock();
        while(block.blockNumber() < currentBlock)
        {
            //Check the current block data.
            if(checkBookmark(m_editor, block, tc))
            {
                return;
            }
            block = block.next();
        }
    }
}

void KNSearchMenu::onBookmarkPrev()
{
    if(m_editor)
    {
        //Loop from the current block to the end of the document.
        QTextCursor tc = m_editor->textCursor();
        auto doc = m_editor->document();
        QTextBlock block = tc.block();
        int currentBlock = block.blockNumber();
        block = block.previous();
        while(block.isValid())
        {
            //Check the block data.
            if(checkBookmark(m_editor, block, tc))
            {
                return;
            }
            //Find the next item.
            block = block.previous();
        }
        //Or else we have to find from the start.
        block = doc->lastBlock();
        while(block.blockNumber() > currentBlock)
        {
            //Check the current block data.
            if(checkBookmark(m_editor, block, tc))
            {
                return;
            }
            block = block.previous();
        }
    }
}

void KNSearchMenu::onBookmarkClear()
{
    if(m_editor)
    {
        //Loop from the current block to the end of the document.
        QTextBlock block = m_editor->document()->firstBlock();
        while(block.isValid())
        {
            //Update the block.
            m_editor->blockData(block)->hasBookmark = false;
            //Move to the next block.
            block = block.next();
        }
        //Update the editor.
        m_editor->update();
    }
}

void KNSearchMenu::onBookmarkCut()
{
    //First do copy, then remove all.
    onBookmarkCopy();
    onBookmarkRemove();
}

void KNSearchMenu::onBookmarkCopy()
{
    if(m_editor)
    {
        //Loop from the top, extract the block that marked.
        QTextBlock block = m_editor->document()->firstBlock();
        QStringList content;
        while(block.isValid())
        {
            //Check the block status.
            if(m_editor->blockData(block)->hasBookmark)
            {
                //Append the content to string list.
                content.append(block.text());
            }
            //Move to the next block.
            block = block.next();
        }
        //Check the clip length.
        if(!content.isEmpty())
        {
            //Copy the content.
            knGlobal->copyText(content.join("\n"));
        }
    }
}

void KNSearchMenu::onBookmarkReplace()
{
    auto clipboard = QApplication::clipboard();
    //Replace the line with the content.
    if(m_editor && m_editor->canPaste())
    {
        //Loop from the top of the document.
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        QTextBlock block = doc->firstBlock();
        tc.beginEditBlock();
        while(block.isValid())
        {
            //Check whether the block has the mark.
            if(m_editor->blockData(block)->hasBookmark)
            {
                //Backup the next block.
                auto nextBlock = block.next();
                //Replace the entire line with the clipboard data.
                tc.setPosition(block.position());
                tc.movePosition(QTextCursor::EndOfBlock,
                                QTextCursor::KeepAnchor);
                tc.insertText(clipboard->text());
                //Move to the next block.
                block = nextBlock;
                continue;
            }
            //Skip the current block.
            block = block.next();
        }
        tc.endEditBlock();
    }
}

void KNSearchMenu::onBookmarkRemove(bool reverse)
{
    if(m_editor)
    {
        //Loop from the top of the document.
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        tc.beginEditBlock();
        QTextBlock block = doc->lastBlock();
        while(block.isValid())
        {
            //Check whether the block has the mark.
            bool blockCheck = m_editor->blockData(block)->hasBookmark;
            if(reverse)
            {
                blockCheck = !blockCheck;
            }
            if(blockCheck)
            {
                //Remove the entire line.
                tc.setPosition(block.position());
                tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                tc.removeSelectedText();
                if(!block.next().isValid())
                {
                    //This is the last block, remove the current block.
                    tc.deletePreviousChar();
                    //Update the block to be the new last block.
                    block = doc->lastBlock();
                    continue;
                }
                //Or else, just delete the current block.
                auto prevBlock = block.previous();
                tc.deleteChar();
                //Set the previous block.
                block = prevBlock;
                continue;
            }
            //Ignore the current block.
            block = block.previous();
        }
        tc.endEditBlock();
    }
}

void KNSearchMenu::onBookmarkInverse()
{
    if(m_editor)
    {
        //Loop from the top of the document.
        auto doc = m_editor->document();
        QTextBlock block = doc->firstBlock();
        while(block.isValid())
        {
            //Inverse the block has bookmark flag.
            auto blockData = m_editor->blockData(block);
            blockData->hasBookmark = !blockData->hasBookmark;
            //Move to the next block.
            block = block.next();
        }
        //Update all the text.
        m_editor->update();
    }
}

KNSearchBar *KNSearchMenu::searchBar() const
{
    return m_searchBar;
}

QAction *KNSearchMenu::menuItem(int index)
{
    return m_menuItems[index];
}

void KNSearchMenu::setEditor(KNTextEditor *editor)
{
    m_editor = editor;
}
