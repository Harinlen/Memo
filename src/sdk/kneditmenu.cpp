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
#include <QDir>
#include <QFileInfo>
#include <QTextCursor>
#include <QTextBlock>
#include <QClipboard>
#include <QMimeData>
#include <QDockWidget>

#include "knutil.h"
#include "kntexteditor.h"
#include "knglobal.h"
#include "knuimanager.h"
#include "kncharpanel.h"
#include "knmainwindow.h"
#include "knclipboardhistory.h"

#include "kneditmenu.h"

KNEditMenu::KNEditMenu(QWidget *parent) : QMenu(parent),
    m_editor(nullptr),
    m_charPanel(new QDockWidget(this)),
    m_clipboardPanel(new KNClipboardHistory(this))
{
    //Link the clipboard check.
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &KNEditMenu::editorCanPasteCheck);
    //Construct actions.
    for(int i=0; i<EditMenuItemCount; ++i)
    {
        m_menuItems[i] = new QAction(parentWidget());
        m_menuItems[i]->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }
    for(int i=0; i<EditSubMenuCount; ++i)
    {
        m_subMenus[i] = new QMenu(this);
    }
    //Add dock widget.
    KNCharPanel *panel = new KNCharPanel(m_charPanel);
    connect(panel, &KNCharPanel::requireInsert,
            this, &KNEditMenu::onInsertText);
    m_charPanel->setWidget(panel);
    knGlobal->mainWindow()->addDockWidget(Qt::RightDockWidgetArea, m_charPanel);
    connect(m_clipboardPanel, &KNClipboardHistory::requireInsert,
            this, &KNEditMenu::onInsertText);
    knGlobal->mainWindow()->addDockWidget(Qt::RightDockWidgetArea, m_clipboardPanel);
    m_charPanel->close();
    m_clipboardPanel->close();
    //Configure the selection item.
    m_menuItems[BeginEndSelect]->setCheckable(true);
    //Set icons.
    m_menuItems[Undo]->setIcon(QIcon(":/icons/undo.png"));
    m_menuItems[Redo]->setIcon(QIcon(":/icons/redo.png"));
    m_menuItems[Cut]->setIcon(QIcon(":/icons/cut.png"));
    m_menuItems[Copy]->setIcon(QIcon(":/icons/copy.png"));
    m_menuItems[Paste]->setIcon(QIcon(":/icons/paste.ico"));
    //Add actions to menu.
    addAction(m_menuItems[Undo]);
    addAction(m_menuItems[Redo]);
    addSeparator();
    addAction(m_menuItems[Cut]);
    addAction(m_menuItems[Copy]);
    addAction(m_menuItems[Paste]);
    addAction(m_menuItems[Delete]);
    addAction(m_menuItems[SelectAll]);
    addAction(m_menuItems[BeginEndSelect]);
    addSeparator();
    addMenu(m_subMenus[CopyToClipboard]);
    m_subMenus[CopyToClipboard]->addAction(m_menuItems[CopyFilePath]);
    m_subMenus[CopyToClipboard]->addAction(m_menuItems[CopyFileName]);
    m_subMenus[CopyToClipboard]->addAction(m_menuItems[CopyFileDir]);
    addMenu(m_subMenus[Indent]);
    m_subMenus[Indent]->addAction(m_menuItems[IndentInc]);
    m_subMenus[Indent]->addAction(m_menuItems[IndentDec]);
    addMenu(m_subMenus[ConvertCase]);
    m_subMenus[ConvertCase]->addAction(m_menuItems[CaseUpper]);
    m_subMenus[ConvertCase]->addAction(m_menuItems[CaseLower]);
    m_subMenus[ConvertCase]->addAction(m_menuItems[CaseProper]);
    m_subMenus[ConvertCase]->addAction(m_menuItems[CaseReverse]);
    m_subMenus[ConvertCase]->addAction(m_menuItems[CaseRandom]);
    addMenu(m_subMenus[LineOps]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineDuplicate]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineRemoveDuplicate]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineJoin]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineMoveUp]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineMoveDown]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineRemoveEmpty]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineRemoveBlank]);
    m_subMenus[LineOps]->addSeparator();
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortLexAsc]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortIntAsc]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortDecCommaAsc]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortDecDotAsc]);
    m_subMenus[LineOps]->addSeparator();
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortLexDec]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortIntDec]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortDecCommaDec]);
    m_subMenus[LineOps]->addAction(m_menuItems[LineSortDecDotDec]);
    addMenu(m_subMenus[Comment]);
    addMenu(m_subMenus[AutoComplete]);
    addMenu(m_subMenus[EOLConvert]);
    addMenu(m_subMenus[BlankOperations]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimRight]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimLeft]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimAll]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimEolToSpace]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimAllAndEol]);
    m_subMenus[BlankOperations]->addSeparator();
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimTabToSpace]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimSpaceToTab]);
    m_subMenus[BlankOperations]->addAction(m_menuItems[TrimSpaceToTabLead]);
    addMenu(m_subMenus[PasteSpecial]);
    m_subMenus[PasteSpecial]->addAction(m_menuItems[PasteHtml]);
    m_subMenus[PasteSpecial]->addAction(m_menuItems[PasteRtf]);
    addMenu(m_subMenus[OnSelection]);
    m_subMenus[OnSelection]->addAction(m_menuItems[SelectOpen]);
    m_subMenus[OnSelection]->addAction(m_menuItems[SelectShowInExplorer]);
    m_subMenus[OnSelection]->addSeparator();
    m_subMenus[OnSelection]->addAction(m_menuItems[SelectSearchOnInternet]);
    addSeparator();
    addAction(m_menuItems[ColumnEditor]);
    addAction(m_menuItems[CharacterPanel]);
    addAction(m_menuItems[ClipboardHistory]);
    addSeparator();
    addAction(m_menuItems[SetReadOnly]);
    addAction(m_menuItems[ClearReadOnly]);
    //Set sequences.
    m_menuItems[Undo]->setShortcut(QKeySequence::Undo);
    m_menuItems[Redo]->setShortcut(QKeySequence::Redo);
    m_menuItems[Cut]->setShortcut(QKeySequence::Cut);
    m_menuItems[Copy]->setShortcut(QKeySequence::Copy);
    m_menuItems[Paste]->setShortcut(QKeySequence::Paste);
    m_menuItems[Delete]->setShortcut(QKeySequence::Delete);
    m_menuItems[SelectAll]->setShortcut(QKeySequence::SelectAll);
    m_menuItems[IndentInc]->setShortcut(QKeySequence(Qt::Key_Tab));
    m_menuItems[IndentDec]->setShortcut(QKeySequence(KNG::SHIFT | Qt::Key_Tab));
    m_menuItems[LineDuplicate]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_D));
    m_menuItems[LineJoin]->setShortcut(QKeySequence(KNG::CTRL | Qt::Key_J));
    m_menuItems[LineMoveUp]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_Up));
    m_menuItems[LineMoveDown]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_Down));
    //Link actions.
    connect(m_menuItems[Undo], &QAction::triggered, this, &KNEditMenu::onUndo);
    connect(m_menuItems[Redo], &QAction::triggered, this, &KNEditMenu::onRedo);
    connect(m_menuItems[Cut], &QAction::triggered, this, &KNEditMenu::onCut);
    connect(m_menuItems[Copy], &QAction::triggered, this, &KNEditMenu::onCopy);
    connect(m_menuItems[Paste], &QAction::triggered, this, &KNEditMenu::onPaste);
    connect(m_menuItems[Delete], &QAction::triggered, this, &KNEditMenu::onDelete);
    connect(m_menuItems[SelectAll], &QAction::triggered, this, &KNEditMenu::onSelectAll);
    connect(m_menuItems[BeginEndSelect], &QAction::triggered, this, &KNEditMenu::onStartEndSelect);
    connect(m_menuItems[CopyFilePath], &QAction::triggered, this, &KNEditMenu::onCopyFilePath);
    connect(m_menuItems[CopyFileName], &QAction::triggered, this, &KNEditMenu::onCopyFileName);
    connect(m_menuItems[CopyFileDir], &QAction::triggered, this, &KNEditMenu::onCopyFileDir);
    connect(m_menuItems[IndentInc], &QAction::triggered, this, &KNEditMenu::onIndentInc);
    connect(m_menuItems[IndentDec], &QAction::triggered, this, &KNEditMenu::onIndentDec);
    connect(m_menuItems[CaseUpper], &QAction::triggered, this, &KNEditMenu::onToUpper);
    connect(m_menuItems[CaseLower], &QAction::triggered, this, &KNEditMenu::onToLower);
    connect(m_menuItems[CaseProper], &QAction::triggered, this, &KNEditMenu::onToProper);
    connect(m_menuItems[CaseReverse], &QAction::triggered, this, &KNEditMenu::onToReverse);
    connect(m_menuItems[CaseRandom], &QAction::triggered, this, &KNEditMenu::onToRandom);
    connect(m_menuItems[LineDuplicate], &QAction::triggered, this, &KNEditMenu::onLineDuplicate);
    connect(m_menuItems[LineRemoveDuplicate], &QAction::triggered, this, &KNEditMenu::onLineRemoveDuplicate);
    connect(m_menuItems[LineJoin], &QAction::triggered, this, &KNEditMenu::onLineJoin);
    connect(m_menuItems[LineMoveUp], &QAction::triggered, this, &KNEditMenu::onLineMoveUp);
    connect(m_menuItems[LineMoveDown], &QAction::triggered, this, &KNEditMenu::onLineMoveDown);
    connect(m_menuItems[LineRemoveEmpty], &QAction::triggered, [=]{ removeEmptyLines(false); });
    connect(m_menuItems[LineRemoveBlank], &QAction::triggered, [=]{ removeEmptyLines(true); });
    connect(m_menuItems[LineSortLexAsc], &QAction::triggered, [=]{ sortLines(true, SortLexcial); });
    connect(m_menuItems[LineSortLexDec], &QAction::triggered, [=]{ sortLines(false, SortLexcial); });
    connect(m_menuItems[LineSortIntAsc], &QAction::triggered, [=]{ sortLines(true, SortInt); });
    connect(m_menuItems[LineSortIntDec], &QAction::triggered, [=]{ sortLines(false, SortInt); });
    connect(m_menuItems[LineSortDecCommaDec], &QAction::triggered, [=]{ sortLines(false, SortDoubleComma); });
    connect(m_menuItems[LineSortDecCommaAsc], &QAction::triggered, [=]{ sortLines(true, SortDoubleComma); });
    connect(m_menuItems[LineSortDecDotDec], &QAction::triggered, [=]{ sortLines(false, SortDoubleDot); });
    connect(m_menuItems[LineSortDecDotAsc], &QAction::triggered, [=]{ sortLines(true, SortDoubleDot); });
    connect(m_menuItems[TrimRight], &QAction::triggered, [=] { trimLines(TrimModeRight); });
    connect(m_menuItems[TrimLeft], &QAction::triggered, [=] { trimLines(TrimModeLeft); });
    connect(m_menuItems[TrimAll], &QAction::triggered, [=] { trimLines(TrimModeBoth); });
    connect(m_menuItems[TrimEolToSpace], &QAction::triggered, this, &KNEditMenu::onTrimEol);
    connect(m_menuItems[TrimAllAndEol], &QAction::triggered, this, &KNEditMenu::onTrimEolAndSpace);
    connect(m_menuItems[TrimTabToSpace], &QAction::triggered, this, &KNEditMenu::onTabToSpace);
    connect(m_menuItems[PasteHtml], &QAction::triggered, this, &KNEditMenu::onPasteHtml);
    connect(m_menuItems[PasteRtf], &QAction::triggered, this, &KNEditMenu::onPasteRtf);
    connect(m_menuItems[SelectOpen], &QAction::triggered, this, &KNEditMenu::onSelectOpen);
    connect(m_menuItems[SelectShowInExplorer], &QAction::triggered, this, &KNEditMenu::onSelectShowInExplorer);
    connect(m_menuItems[SelectSearchOnInternet], &QAction::triggered, this, &KNEditMenu::onSelectNetSearch);
    connect(m_menuItems[CharacterPanel], &QAction::triggered, m_charPanel, &QDockWidget::show);
    connect(m_menuItems[ClipboardHistory], &QAction::triggered, m_clipboardPanel, &KNClipboardHistory::show);
    connect(m_menuItems[SetReadOnly], &QAction::triggered, this, &KNEditMenu::onSetReadOnly);
    connect(m_menuItems[ClearReadOnly], &QAction::triggered, this, &KNEditMenu::onClearReadOnly);
    //Link translator.
    knUi->addTranslate(this, &KNEditMenu::retranslate);
}

void KNEditMenu::setEditor(KNTextEditor *editor)
{
    //Disconnect all the previous editor data.
    for(auto i : m_connects)
    {
        //Disconnect the connection.
        disconnect(i);
    }
    //Save the editor.
    m_editor = editor;
    if(m_editor)
    {
        //Update the widget status.
        m_connects.append(
                    connect(m_editor, &KNTextEditor::undoAvailable,
                            m_menuItems[Undo], &QAction::setEnabled));
        m_connects.append(
                    connect(m_editor, &KNTextEditor::redoAvailable,
                            m_menuItems[Redo], &QAction::setEnabled));
        m_connects.append(
                    connect(m_editor, &KNTextEditor::copyAvailable,
                            this, &KNEditMenu::onCopyAvailable));
        m_connects.append(
                    connect(m_editor, &KNTextEditor::readOnlyChange,
                            this, &KNEditMenu::onReadOnlyChanged));
    }
    //Update the status.
    auto doc = m_editor->document();
    m_menuItems[Undo]->setEnabled(doc->isUndoAvailable());
    m_menuItems[Redo]->setEnabled(doc->isRedoAvailable());
    onCopyAvailable(m_editor->canCopy());
    editorCanPasteCheck();
    onReadOnlyChanged(m_editor->isReadOnly());
    //Update select status.
    m_menuItems[BeginEndSelect]->setChecked(false);
}

QAction *KNEditMenu::menuItem(int index)
{
    return m_menuItems[index];
}

void KNEditMenu::retranslate()
{
    setTitle(tr("&Edit"));
    //Update action title.
    m_menuItems[Undo]->setText(tr("&Undo"));
    m_menuItems[Redo]->setText(tr("&Redo"));
    m_menuItems[Cut]->setText(tr("Cu&t"));
    m_menuItems[Copy]->setText(tr("&Copy"));
    m_menuItems[Paste]->setText(tr("&Paste"));
    m_menuItems[Delete]->setText(tr("&Delete"));
    m_menuItems[SelectAll]->setText(tr("Select &All"));
    m_menuItems[BeginEndSelect]->setText(tr("Begin/End Select"));
    m_menuItems[ColumnEditor]->setText(tr("Column Editor..."));
    m_menuItems[CharacterPanel]->setText(tr("Character Panel"));
    m_menuItems[ClipboardHistory]->setText(tr("Clipboard History"));
    m_menuItems[SetReadOnly]->setText(tr("Set Read-Only"));
    m_menuItems[ClearReadOnly]->setText(tr("Clear Read-Only Flag"));
    //Update sub menu title.
    m_subMenus[CopyToClipboard]->setTitle(tr("Copy to Clipboard"));
    m_subMenus[Indent]->setTitle(tr("Indent"));
    m_subMenus[ConvertCase]->setTitle(tr("Convert Case to"));
    m_subMenus[LineOps]->setTitle(tr("Line Operators"));
    m_subMenus[Comment]->setTitle(tr("Comment/Uncomment"));
    m_subMenus[AutoComplete]->setTitle(tr("Auto-Completion"));
    m_subMenus[EOLConvert]->setTitle(tr("EOL Conversion"));
    m_subMenus[BlankOperations]->setTitle(tr("Blank Operations"));
    m_subMenus[PasteSpecial]->setTitle(tr("Paste Special"));
    m_subMenus[OnSelection]->setTitle(tr("On Selection"));
    //Update sub menu items.
    m_menuItems[CopyFilePath]->setText(tr("Current Full File path to CLipboard"));
    m_menuItems[CopyFileName]->setText(tr("Current Filename to Clipboard"));
    m_menuItems[CopyFileDir]->setText(tr("Current Dir. Path to Clipboard"));
    m_menuItems[IndentInc]->setText(tr("Increase Line Indent"));
    m_menuItems[IndentDec]->setText(tr("Decrease Line Indent"));
    m_menuItems[CaseUpper]->setText(tr("&UPPER"));
    m_menuItems[CaseLower]->setText(tr("&lower"));
    m_menuItems[CaseProper]->setText(tr("&Proper Case"));
    m_menuItems[CaseReverse]->setText(tr("&iNVERT cASE"));
    m_menuItems[CaseRandom]->setText(tr("&ranDOM CasE"));
    m_menuItems[LineDuplicate]->setText(tr("Duplicate Current Line"));
    m_menuItems[LineRemoveDuplicate]->setText(tr("Remove Consecutive Duplicate Lines"));
    m_menuItems[LineJoin]->setText(tr("Join Lines"));
    m_menuItems[LineMoveUp]->setText(tr("Move Up Current Line"));
    m_menuItems[LineMoveDown]->setText(tr("Move Down Current Line"));
    m_menuItems[LineRemoveEmpty]->setText(tr("Remove Empty Lines"));
    m_menuItems[LineRemoveBlank]->setText(tr("Remove Empty Lines (Containing Blank characters)"));
    m_menuItems[LineSortLexAsc]->setText(tr("Sort Lines Lexicographically Ascending"));
    m_menuItems[LineSortIntAsc]->setText(tr("Sort Lines As Integers Ascending"));
    m_menuItems[LineSortDecCommaAsc]->setText(tr("Sort Lines As Decimals (Comma) Ascending"));
    m_menuItems[LineSortDecDotAsc]->setText(tr("Sort Lines As Decimals (Dot) Ascending"));
    m_menuItems[LineSortLexDec]->setText(tr("Sort Lines Lexicographically Descending"));
    m_menuItems[LineSortIntDec]->setText(tr("Sort Lines As Integers Descending"));
    m_menuItems[LineSortDecCommaDec]->setText(tr("Sort Lines As Decimals (Comma) Descending"));
    m_menuItems[LineSortDecDotDec]->setText(tr("Sort Lines As Decimals (Dot) Descending"));
    m_menuItems[TrimRight]->setText(tr("Trim Trailing Space"));
    m_menuItems[TrimLeft]->setText(tr("Trim Leading Space"));
    m_menuItems[TrimAll]->setText(tr("Trim Leading and Trailing Space"));
    m_menuItems[TrimEolToSpace]->setText(tr("EOL to Space"));
    m_menuItems[TrimAllAndEol]->setText(tr("Remove Unnecessary Blank and EOL"));
    m_menuItems[TrimTabToSpace]->setText(tr("TAB to Space"));
    m_menuItems[TrimSpaceToTab]->setText(tr("Space to TAB (All)"));
    m_menuItems[TrimSpaceToTabLead]->setText(tr("Space to TAB (Leading)"));
    m_menuItems[SelectOpen]->setText(tr("Open File"));
    m_menuItems[SelectShowInExplorer]->setText(tr("Open Containing Folder in Explorer"));
    m_menuItems[SelectSearchOnInternet]->setText(tr("Search on Internet"));
    m_menuItems[PasteHtml]->setText(tr("Paste HTML Content"));
    m_menuItems[PasteRtf]->setText(tr("Paste RTF Content"));
    //Update the title.
    m_charPanel->setWindowTitle(tr("ASCII Code Insertion Panel"));
}

void KNEditMenu::onCopyAvailable(bool yes)
{
    //Update both copy and cut.
    m_menuItems[Copy]->setEnabled(yes);
    m_menuItems[Cut]->setEnabled(yes);
}

void KNEditMenu::onReadOnlyChanged(bool isReadOnly)
{
    //Update the action enable state.
    m_menuItems[SetReadOnly]->setEnabled(!isReadOnly);
    m_menuItems[ClearReadOnly]->setEnabled(isReadOnly);
}

void KNEditMenu::editorCanPasteCheck()
{
    //Check the editor.
    if(m_editor)
    {
        //Check the paste validation.
        m_menuItems[Paste]->setEnabled(m_editor->canPaste());
    }
}

void KNEditMenu::onUndo()
{
    if(m_editor) { m_editor->undo(); }
}

void KNEditMenu::onRedo()
{
    if(m_editor) { m_editor->redo(); }
}

void KNEditMenu::onCut()
{
    if(m_editor) { m_editor->cut(); }
}

void KNEditMenu::onCopy()
{
    if(m_editor) { m_editor->copy(); }
}

void KNEditMenu::onPaste()
{
    if(m_editor) { m_editor->paste(); }
}

void KNEditMenu::onDelete()
{
    if(m_editor) { m_editor->cursorDelete(); }
}

void KNEditMenu::onSelectAll()
{
    if(m_editor) { m_editor->selectAll(); }
}

void KNEditMenu::onCopyFilePath()
{
    if(m_editor)
    {
        knGlobal->copyText(QDir::toNativeSeparators(
                               QFileInfo(m_editor->filePath()).absoluteFilePath()));
    }
}

void KNEditMenu::onCopyFileName()
{
    if(m_editor) { knGlobal->copyText(QFileInfo(m_editor->filePath()).fileName()); }
}

void KNEditMenu::onCopyFileDir()
{
    if(m_editor)
    {
        knGlobal->copyText(QDir::toNativeSeparators(
                               QFileInfo(m_editor->filePath()).absolutePath()));
    }
}

void KNEditMenu::onIndentInc()
{
    if(m_editor) { m_editor->indentCursor(); }
}

void KNEditMenu::onIndentDec()
{
    if(m_editor) { m_editor->unindentCursor(); }
}

void KNEditMenu::onToUpper()
{
    if(m_editor) { m_editor->convertSelectCase(KNTextEditor::Upper); }
}

void KNEditMenu::onToLower()
{
    if(m_editor) { m_editor->convertSelectCase(KNTextEditor::Lower); }
}

void KNEditMenu::onToProper()
{
    if(m_editor) { m_editor->convertSelectCase(KNTextEditor::Proper); }
}

void KNEditMenu::onToReverse()
{
    if(m_editor) { m_editor->convertSelectCase(KNTextEditor::Reverse); }
}

void KNEditMenu::onToRandom()
{
    if(m_editor) { m_editor->convertSelectCase(KNTextEditor::Random); }
}

void KNEditMenu::onLineDuplicate()
{
    if(m_editor)
    {
        auto tc = m_editor->textCursor();
        tc.beginEditBlock();
        //Copy the block after the current block.
        auto currentBlock = tc.block();
        //Move to the end of the block.
        tc.setPosition(currentBlock.position() + currentBlock.length() - 1);
        tc.insertBlock();
        tc.insertText(currentBlock.text());
        tc.endEditBlock();
    }
}

void KNEditMenu::onLineRemoveDuplicate()
{
    if(m_editor)
    {
        auto tc = m_editor->textCursor();
        //Check the duplication block after the current block.
        auto currentBlock = tc.block(), block = currentBlock.next();
        QString currentText = currentBlock.text();
        QTextBlock lastBlock;
        while(block.isValid())
        {
            //If the block does not matches the current line, then exit.
            if(block.text() != currentText)
            {
                break;
            }
            //Record the block.
            lastBlock = block;
            block = block.next();
        }
        //If the last block is valid, then start remove them.
        if(lastBlock.isValid())
        {
            tc.beginEditBlock();
            while(lastBlock != currentBlock)
            {
                auto prevBlock = lastBlock.previous();
                //Remove the block.
                tc = QTextCursor(lastBlock);
                tc.select(QTextCursor::BlockUnderCursor);
                tc.removeSelectedText();
                lastBlock = prevBlock;
            }
            tc.endEditBlock();
        }
    }
}

void KNEditMenu::onLineJoin()
{
    if(m_editor)
    {
        auto tc = m_editor->textCursor();
        //Only process when have selections.
        if(!tc.hasSelection())
        {
            return;
        }
        auto doc = m_editor->document();
        auto startBlock=doc->findBlock(tc.selectionStart()),
                endBlock=doc->findBlock(tc.selectionEnd());
        if(startBlock.blockNumber() == endBlock.blockNumber())
        {
            return;
        }
        //Combine the block number.
        tc.beginEditBlock();
        QTextBlock currentBlock = endBlock;
        do
        {
            //Move to the previous block.
            currentBlock = currentBlock.previous();
            //Move the tc to the end of the current block.
            tc.setPosition(currentBlock.position() + currentBlock.length() - 1);
            tc.deleteChar();
        }
        while(currentBlock != startBlock);
        tc.endEditBlock();
    }
}

void KNEditMenu::onLineMoveUp()
{
    if(m_editor)
    {
        auto tc = m_editor->textCursor();
        //Ignore when there is only one line.
        if(m_editor->blockCount() == 1 || tc.blockNumber() == 0)
        {
            return;
        }
        tc.beginEditBlock();
        auto doc = m_editor->document();
        //Change the selection of the line.
        if(tc.hasSelection())
        {
            //Extract the text block
            const QTextBlock &blockStart = doc->findBlock(tc.selectionStart()),
                    &blockEnd = doc->findBlock(tc.selectionEnd());
            //Change the tc selection.
            tc.setPosition(blockStart.position());
            tc.setPosition(blockEnd.position() + blockEnd.length() - 1,
                           QTextCursor::KeepAnchor);
        }
        else
        {
            const QTextBlock &block = tc.block();
            //Select the current block.
            tc.setPosition(block.position());
            tc.setPosition(block.position() + block.length() - 1,
                           QTextCursor::KeepAnchor);
        }
        //Move one block above.
        QString selectedLines = tc.selectedText();
        tc.removeSelectedText();
        tc.movePosition(QTextCursor::PreviousBlock);
        tc.movePosition(QTextCursor::EndOfBlock);
        tc.deleteChar();
        tc.movePosition(QTextCursor::StartOfBlock);
        int currentPos = tc.position();
        tc.insertText(selectedLines);
        tc.insertText("\n");
        tc.movePosition(QTextCursor::PreviousCharacter);
        tc.setPosition(currentPos, QTextCursor::KeepAnchor);
        tc.endEditBlock();
        //Set the text cursor.
        m_editor->setTextCursor(tc);
    }
}

void KNEditMenu::onLineMoveDown()
{
    if(m_editor)
    {
        auto tc = m_editor->textCursor();
        //Ignore when there is only one line.
        if(m_editor->blockCount() == 1)
        {
            return;
        }
        auto doc = m_editor->document();
        //Change the selection of the line.
        if(tc.hasSelection())
        {
            //Extract the text block
            QTextBlock blockStart = doc->findBlock(tc.selectionStart()),
                    blockEnd = doc->findBlock(tc.selectionEnd());
            //Check whether the block end already reaches the last line.
            if(blockEnd.blockNumber() == doc->blockCount() - 1)
            {
                //Check whether the block end is at the beginning of the next
                //block.
                if(tc.selectionEnd() != blockEnd.position())
                {
                    return;
                }
                //Treat the block end as its previous block.
                blockEnd = blockEnd.previous();
            }
                tc.beginEditBlock();
            //Change the tc selection.
            tc.setPosition(blockStart.position());
            tc.setPosition(blockEnd.position() + blockEnd.length() - 1,
                           QTextCursor::KeepAnchor);
        }
        else
        {
            //Check if it is already the last line.
            if(tc.blockNumber() == doc->blockCount() - 1)
            {
                return;
            }
            //Start move.
            tc.beginEditBlock();
            const QTextBlock &block = tc.block();
            //Select the current block.
            tc.setPosition(block.position());
            tc.setPosition(block.position() + block.length() - 1,
                           QTextCursor::KeepAnchor);
        }
        //Move one block above.
        QString selectedLines = tc.selectedText();
        tc.removeSelectedText();
        tc.deleteChar();
        tc.movePosition(QTextCursor::EndOfBlock);
        tc.insertText("\n");
        int currentPos = tc.position();
        tc.insertText(selectedLines);
        tc.setPosition(currentPos, QTextCursor::KeepAnchor);
        tc.endEditBlock();
        //Set the text cursor.
        m_editor->setTextCursor(tc);
    }
}

void KNEditMenu::onTrimEol()
{
    if(m_editor)
    {
        //Loop for all the blocks.
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        tc.beginEditBlock();
        while(doc->blockCount() > 1)
        {
            //For all the block, remove its end spacing.
            QTextBlock block = doc->firstBlock();
            tc.setPosition(block.position() + block.length());
            tc.deletePreviousChar();
            tc.insertText(" ");
        }
        tc.endEditBlock();
    }
}

static QString rstrip(const QString &source)
{
    for(int n=source.size()-1; n > -1; --n)
    {
        if(!source.at(n).isSpace())
        {
            return source.left(n + 1);
        }
    }
    return QString();
}

static QString lstrip(const QString &source)
{
    for(int n=0; n<source.size(); ++n)
    {
        if(!source.at(n).isSpace())
        {
            return source.mid(n);
        }
    }
    return QString();
}

static QString strip(const QString &source)
{
    return source.trimmed();
}

void KNEditMenu::onTrimEolAndSpace()
{
    if(m_editor)
    {
        //Loop for all the blocks.
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        tc.beginEditBlock();
        //Process the first block.
        QTextBlock block = doc->firstBlock();
        //For all the block, remove its end spacing.
        tc.setPosition(block.position());
        tc.setPosition(block.position() + block.length() - 1,
                       QTextCursor::KeepAnchor);
        tc.insertText(strip(block.text()));
        //Check whether the block count is greater than 1.
        while(doc->blockCount() > 1)
        {
            //Fetch the second block.
            block = doc->findBlockByNumber(1);
            //For all the block, remove its end spacing.
            tc.setPosition(block.position());
            tc.setPosition(block.position() + block.length() - 1,
                           QTextCursor::KeepAnchor);
            tc.insertText(strip(block.text()));
            //Move to the end of the first block.
            block = doc->firstBlock();
            tc.setPosition(block.position() + block.length() - 1);
            tc.deleteChar();
            tc.insertText(" ");
        }
        tc.endEditBlock();
    }
}

void KNEditMenu::onTabToSpace()
{
    if(m_editor)
    {
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        tc.beginEditBlock();
        //Find all the tab characters in the block
        QTextBlock block = doc->firstBlock();
        int tabSpacing = knGlobal->tabSpacing();
        while(block.isValid())
        {
            //Find all '\t' chars.
            QString blockText = block.text();
            //Prepare the start and end position.
            QVector<int> tabPos, tabSize;
            //Calculate the positions.
            int column = 0;
            for(int i=0; i<blockText.size(); ++i)
            {
                //Check whether the iteration is \t.
                if(blockText.at(i) == '\t')
                {
                    tabPos.append(i);
                    //Get the start pos of tab.
                    int tabStart = column;
                    //Find the nearest variable.
                    column = KNTextEditor::cellTabSpacing(column, tabSpacing);
                    tabSize.append(column - tabStart);
                    continue;
                }
                //Simply increase a char.
                column += 1;
            }
            //Loop from the last info to the first one.
            for(int i=tabPos.size() - 1; i>-1; --i)
            {
                //Replace the char as the spaces.
                blockText.replace(tabPos.at(i), 1, QString(tabSize.at(i), ' '));
            }
            //Replace the block context.
            tc.setPosition(block.position());
            tc.setPosition(block.position() + block.length() - 1,
                           QTextCursor::KeepAnchor);
            tc.insertText(blockText);
            //Move to next block.
            block = block.next();
        }
        tc.endEditBlock();
    }
}

void KNEditMenu::onPasteHtml()
{
    if(m_editor)
    {
        //Check the clipboard.
        const QMimeData *data = QApplication::clipboard()->mimeData();
        //Check the formats.
        if(data && data->hasHtml())
        {
            //Get the data information.
            m_editor->insertPlainText(data->html());
        }
    }
}

void KNEditMenu::onPasteRtf()
{
    if(m_editor)
    {
        //Check the clipboard.
        const QMimeData *data = QApplication::clipboard()->mimeData();
        //Check the formats.
        if(data && data->hasFormat("Rich Text Format"))
        {
            //Get the data information.
            m_editor->insertPlainText(QString::fromUtf8(
                                          data->data("Rich Text Format")));
        }
    }
}

void KNEditMenu::onSelectOpen()
{
    if(m_editor)
    {
        //Use the default editor to open the selection.
        QTextCursor tc = m_editor->textCursor();
        if(tc.hasSelection())
        {
            //Check the file existance first.
            if(QFile::exists(tc.selectedText()))
            {
                KNUtil::openLocalFile(tc.selectedText());
            }
        }
    }
}

void KNEditMenu::onSelectShowInExplorer()
{
    if(m_editor)
    {
        //Use the default editor to open the selection.
        QTextCursor tc = m_editor->textCursor();
        if(tc.hasSelection())
        {
            //Check the file existance first.
            if(QFile::exists(tc.selectedText()))
            {
                KNUtil::showInGraphicalShell(tc.selectedText());
            }
        }
    }
}

void KNEditMenu::onSelectNetSearch()
{
    if(m_editor)
    {
        //Use the default editor to open the selection.
        QTextCursor tc = m_editor->textCursor();
        if(tc.hasSelection())
        {
            //Call the URL.
            knGlobal->searchOnline(tc.selectedText());
        }
    }
}

void KNEditMenu::onSetReadOnly()
{
    if(m_editor)
    {
        //Enable the read only mode of the editor.
        m_editor->setReadOnly(true);
    }
}

void KNEditMenu::onClearReadOnly()
{
    if(m_editor)
    {
        //Enable the read only mode of the editor.
        m_editor->setReadOnly(false);
    }
}

void KNEditMenu::onInsertText(const QString &text)
{
    if(m_editor)
    {
        //Enable the read only mode of the editor.
        m_editor->textCursor().insertText(text);
        //Move the focus on the editor.
        m_editor->setFocus();
    }
}

void KNEditMenu::onStartEndSelect()
{
    if(m_editor)
    {
        QTextCursor tc = m_editor->textCursor();
        //Check the selection status.
        if(m_menuItems[BeginEndSelect]->isChecked())
        {
            //Save the status.
            m_selectStart = tc.position();
        }
        else
        {
            //Apply the selection.
            int finalPos = tc.position();
            //Select the area.
            tc.setPosition(m_selectStart);
            tc.setPosition(finalPos, QTextCursor::KeepAnchor);
            m_editor->setTextCursor(tc);
        }
    }
}

void KNEditMenu::trimLines(int trimMode)
{
    QString (*trimFunc)(const QString &);
    switch(trimMode)
    {
    case TrimModeRight:
        trimFunc = rstrip;
        break;
    case TrimModeLeft:
        trimFunc = lstrip;
        break;
    case TrimModeBoth:
        trimFunc = strip;
        break;
    }

    if(m_editor)
    {
        //Loop for all the blocks.
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        tc.beginEditBlock();
        QTextBlock block = doc->firstBlock();
        while(block.isValid())
        {
            //For all the block, remove its end spacing.
            tc.setPosition(block.position());
            tc.setPosition(block.position() + block.length() - 1,
                           QTextCursor::KeepAnchor);
            tc.insertText(trimFunc(block.text()));
            block = block.next();
        }
        tc.endEditBlock();
    }
}

int extractIntLength(const QString &buf)
{
    if(buf.length() < 1)
    {
        return 0;
    }
    int pos = 0;
    if(buf.at(0) == '-')
    {
        pos = 1;
    }
    for(; pos < buf.length(); ++pos)
    {
        auto c = buf.at(pos);
        if(!(c>='0' && c<='9'))
        {
            //If there is only minus, treat as no number.
            break;
        }
    }
    //Check the invalid position.
    if(pos == 1 && buf.at(0) == '-')
    {
        return 0;
    }
    return pos;
}

bool compareAsInt(const QString &a, const QString &b)
{
    //Find the first integer length.
    int numASize = extractIntLength(a), numBSize = extractIntLength(b);
    if(numASize == 0 || numBSize == 0)
    {
        //One of those is pure string, cannot be compared as int.
        return a < b;
    }
    //Both of them contains integers, check minus.
    bool aMinus = a.at(0) == '-', bMinus = b.at(0) == '-';
    if(aMinus && !bMinus)
    {
        return true;
    }
    if(!aMinus && bMinus)
    {
        return false;
    }
    //Now both of them is integer and both minus or plus.
    if(numASize != numBSize)
    {
        return aMinus ? (numASize > numBSize) : (numASize < numBSize);
    }
    if(aMinus)
    {
        for(int i=1; i<numASize; ++i)
        {
            if(a.at(i) < b.at(i)) { return false; }
            if(a.at(i) > b.at(i)) { return true; }
        }
        //Or else, just okay.
        return true;
    }
    for(int i=0; i<numASize; ++i)
    {
        if(a[i] < b[i]) { return true; }
        if(a[i] > b[i]) { return false; }
    }
    //When achieved here, then they are the same, return false.
    return false;
}


QString extractFloat(const QString &buf, const char splitter)
{
    //Initial the variables.
    if(buf.length() < 1)
    {
        return QString();
    }
    //Initial the position.
    int pos = 0, pointPos = -1;
    if(buf.at(0) == '-')
    {
        pos = 1;
    }
    for(; pos < buf.length(); ++pos)
    {
        auto c = buf.at(pos);
        if(c == splitter)
        {
            //Check the point pos is set.
            if(pointPos != -1)
            {
                //The second mark is invalid.
                return buf.mid(0, pos);
            }
            pointPos = pos;
            continue;
        }
        if(!(c>='0' && c<='9'))
        {
            return buf.mid(0, pos);
        }
    }
    return buf;
}

static bool compareAsFloat(const QString &a, const QString &b,
                           const char splitter)
{
    //Extract the float features.
    QString &&numABuf = extractFloat(a, splitter),
            &&numBBuf = extractFloat(b, splitter);
    if(numABuf.isEmpty() || numBBuf.isEmpty())
    {
        //One of those is pure string, cannot be compared as int.
        return a < b;
    }
    //Convert the splitter.
    numABuf.replace(splitter, '.');
    numBBuf.replace(splitter, '.');
    //Convert the double.
    bool resultA, resultB;
    double valueA = numABuf.toDouble(&resultA),
            valueB = numBBuf.toDouble(&resultB);
    //Check the result.
    if(!resultA || !resultB)
    {
        return a < b;
    }
    //Compare those two values.
    return valueA < valueB;
}

void KNEditMenu::sortLines(bool asc, int sortMode)
{
    if(m_editor)
    {
        //Split the entire document based on space.
        QStringList lines = m_editor->toPlainText().split('\n');
        //Sort the lines.
        switch(sortMode)
        {
        case SortLexcial:
            lines.sort();
            break;
        case SortInt:
            //Extract integers.
            std::sort(lines.begin(), lines.end(), compareAsInt);
            break;
        case SortDoubleComma:
            //Extract double.
            std::sort(lines.begin(), lines.end(),
                      [=](const QString &a, const QString &b)
            { return compareAsFloat(a, b, ','); });
            break;
        case SortDoubleDot:
            //Extract double.
            std::sort(lines.begin(), lines.end(),
                      [=](const QString &a, const QString &b)
            { return compareAsFloat(a, b, '.'); });
            break;
        }
        //Check sort order.
        if(!asc)
        {
            //Reverse the result.
            std::reverse(lines.begin(), lines.end());
        }
        //Select all and replace.
        QTextCursor tc = m_editor->textCursor();
        tc.movePosition(QTextCursor::Start);
        tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        //Insert the text.
        tc.insertText(lines.join("\n"));
    }
}

void KNEditMenu::removeEmptyLines(bool blankCheck)
{
    if(m_editor)
    {
        //Loop from the end of the line.
        bool edited = false;
        auto doc = m_editor->document();
        QTextCursor tc = m_editor->textCursor();
        QTextBlock block = doc->lastBlock();
        while(block.isValid())
        {
            //Check the block length.
            bool removed = block.length() == 1;
            if(!removed && blankCheck)
            {
                //Need to check whether this line is emtirely blank.
                removed = (KNTextEditor::firstNonSpacePos(block) ==
                           (block.length() - 1));
            }
            if(removed)
            {
                //Which means it only contains a new line char, remove it.
                if(!edited)
                {
                    edited = true;
                    tc.beginEditBlock();
                }
                //Clear the line first.
                tc.setPosition(block.position());
                if(block.length() > 1)
                {
                    tc.setPosition(block.position() + block.length() - 1,
                                   QTextCursor::KeepAnchor);
                    tc.removeSelectedText();
                }
                if(block.blockNumber() == 0)
                {
                    //Time to break.
                    tc.deleteChar();
                    break;
                }
                else if(block.blockNumber() == doc->lastBlock().blockNumber())
                {
                    //Then remove the current block.
                    tc.deletePreviousChar();
                    //Move to the next block.
                    block = tc.block();
                    continue;
                }
                //Or else, remove the current line.
                tc.deleteChar();
            }
            //Move to the next block.
            block = block.previous();
        }
        if(edited)
        {
            tc.endEditBlock();
        }
    }
}
