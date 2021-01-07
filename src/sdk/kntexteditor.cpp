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
#include <ctime>

#include <QTimer>
#include <QTextBlock>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextStream>
#include <QFileInfo>
#include <QStringDecoder>
#include <QStringEncoder>
#include <QFontDatabase>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QtConcurrent/QtConcurrent>

#include "kntextblockdata.h"
#include "knglobal.h"
#include "kntexteditorpanel.h"
#include "knsyntaxhighlighter.h"
#include "knuimanager.h"
#include "kndocumentlayout.h"

#include "kntexteditor.h"

KNTextEditor::KNTextEditor(const QString &titleName,
                           const QString &filePath, const QString &codec,
                           QWidget *parent, KNSyntaxHighlighter *highlighter) :
    QPlainTextEdit(parent),
    m_quickSearchSense(Qt::CaseInsensitive),
    m_quickSearchCode(0),
    m_showResults(false),
    m_filePath(QString()),
    m_codecName(codec.toLatin1()),
    m_panel(new KNTextEditorPanel(this)),
    m_columnBlockNumber(-1), m_columnOffset(-1),
    m_editorOptions(HighlightCursor | CursorDisplay | LineNumberDisplay)
{
    //Set properties.
    setFrameStyle(QFrame::NoFrame);
    setCursorWidth(0);
    //Configure the extra selections.
    m_currentLine.format.setBackground(QColor(232, 232, 255, 160));
    //Update the viewport margins.
    updateViewportMargins();
    //Configure the editor based on global settings.
    setSymbolDisplayMode(knGlobal->symbolDisplayMode());
    onWrapModeChange(knGlobal->isWrap());
    setTabStopDistance(knGlobal->tabSpacing() *
                       fontMetrics().averageCharWidth());
    onEditorFontChanged();
    onResultDisplayChange(knGlobal->isSearchResultShown());
    //Reset the text layout.
//    document()->setDocumentLayout(new KNDocumentLayout(document()));

    //Link the cursor painting signals.
    addLink(connect(knGlobal->cursorTimer(), &QTimer::timeout,
                    this, &KNTextEditor::onCursorUpdate));
    addLink(connect(knGlobal, &KNGlobal::editorFontChange,
                    this, &KNTextEditor::onEditorFontChanged));
    addLink(connect(knGlobal, &KNGlobal::editorDisplayModeChange,
                    this, &KNTextEditor::setSymbolDisplayMode));
    addLink(connect(knGlobal, &KNGlobal::editorWrapModeChange,
                    this, &KNTextEditor::onWrapModeChange));
    addLink(connect(knGlobal, &KNGlobal::editorResultDisplayChange,
                    this, &KNTextEditor::onResultDisplayChange));
    //Link the signals.
    connect(this, &KNTextEditor::updateRequest,
            this, &KNTextEditor::updatePanelArea);
    connect(this, &KNTextEditor::blockCountChanged,
            this, &KNTextEditor::onBlockCountChanged);
    connect(this, &KNTextEditor::cursorPositionChanged,
            this, &KNTextEditor::onCursorPositionChanged);
    connect(this, &KNTextEditor::textChanged,
            this, &KNTextEditor::onTextChanged);
    //Based on the parameter set information.
    if(filePath.isEmpty())
    {
        //Only set the title name.
        setDocumentTitle(titleName);
        //Configure and set the highlighter.
        updateHighlighter(highlighter);
    }
    else
    {
        //Load the file from the file path.
        loadFrom(filePath, m_codecName);
    }
    //Initial the position change signal.
    emit cursorPositionChanged();
    //Update the document information.
    document()->setModified(false);
    //Move to the first position.
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::Start);
    setTextCursor(tc);
}

void KNTextEditor::paintSidebar(QPainter *painter, int lineNumWidth,
                                int markWidth, int foldWidth)
{
    //Update the painter.
    painter->setFont(font());
    //Paint from the first visible block.
    QTextBlock block = firstVisibleBlock();
    QRectF area;
    const QPixmap &bookmark = knGlobal->bookmark();
    int markX = lineNumWidth, foldX = markX + markWidth,
            currentBlock = textCursor().blockNumber();
    //Draw the fold area background.
    painter->fillRect(foldX, 0, foldWidth, height(),
                      QColor(240, 240, 240));
    //Draw the content.
    bool drawLineNum = (m_editorOptions & LineNumberDisplay);
    KNTextBlockData *prevData = nullptr;
    if(block.previous().isValid())
    {
        prevData = blockData(block.previous());
    }
    while(block.isValid() && area.y() < height())
    {
        //Fetch the block area.
        area = blockBoundingGeometry(block).translated(contentOffset());
        //Check the block number.
        if((m_editorOptions & HighlightCursor) &&
                currentBlock == block.blockNumber())
        {
            int height = (currentBlock == document()->blockCount() - 1) ?
                        area.height() - 3:
                        area.height();
            //Fill the background.
            painter->fillRect(0, area.y(), m_panel->width(), height,
                              m_currentLine.format.background());
        }
        //Extract the data from the block.
        //Draw the line number.
        if(drawLineNum)
        {
            painter->drawText(0, area.y(), lineNumWidth, area.height(),
                              Qt::AlignRight,
                              QString::number(block.blockNumber() + 1));
        }
        auto data = blockData(block);
        if(data && data->hasBookmark)
        {
            if(area.height() < bookmark.height())
            {
                //Draw at center.
                painter->drawPixmap(lineNumWidth,
                                    area.center().y() - (bookmark.height() >> 1),
                                    bookmark);
            }
            //Or else draw at the first line.
            else
            {
                //Draw a bookmark at the bookmark position.
                painter->drawPixmap(markX, area.y(), bookmark);
            }
        }
        //Based on code level, draw the code folding area.
        if(data->levelMargin > 0)
        {
            //Need to draw the expand icon.
            painter->drawRect(foldX, area.y(), foldWidth, foldWidth);
        }
        //Move to next block.
        block = block.next();
    }
}

void KNTextEditor::loadFrom(const QString &filePath, const QByteArray &codecName)
{
    //Read the file.
    QFile targetFile(filePath);
    if(!targetFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(
                    this,
                    tr("Open failed"),
                    tr("Please check if this file is opened in another program."));
        return;
    }
    QTextCodec *codec = QTextCodec::codecForName(codecName);
    //Based on the codec.
    if(!codec)
    {
        //Use the default codec.
        codec = QTextCodec::codecForLocale();
    }
    //Decode the file.
    document()->setPlainText(codec->toUnicode(targetFile.readAll()));
    targetFile.close();
    //Set the codec name.
    setCodecName(codec->name());
    //Set the file path.
    setFilePath(filePath);
    //Update the syntax highlighter.
    updateHighlighter();
}

bool KNTextEditor::save()
{
    //Use the file path to save.
    return saveToFile(m_filePath);
}

void KNTextEditor::resizeEvent(QResizeEvent *event)
{
    //Resize the widget first.
    QPlainTextEdit::resizeEvent(event);
    //Update the panel.
    m_panel->resize(m_panel->width(), height());
}

void KNTextEditor::keyPressEvent(QKeyEvent *event)
{
    //Shortcut overrides.
    if(event == QKeySequence::Undo) { undo(); event->accept(); return; }
    else if(event == QKeySequence::Redo) { redo(); event->accept(); return; }
    else if(event == QKeySequence::Cut) { cut(); event->accept(); return; }
    else if(event == QKeySequence::Copy) { copy(); event->accept(); return; }
    else if(event == QKeySequence::Paste) { paste(); event->accept(); return; }
    //Check whether the event is sent to us.
    switch(event->key())
    {
    case Qt::Key_Escape:
    {
        //Clear the extra text cursor.
        clearColumnCursor();
        //Update viewport.
        viewport()->update();
        //Update the cursor.
        onCursorPositionChanged();
        break;
    }
    case Qt::Key_Tab:
    {
        //Check whether it contains the shift modifier.
        if(event->modifiers() == KNG::SHIFT)
        {
            //Back tab, remove indent.
            unindentCursor();
        }
        else if(event->modifiers() == Qt::NoModifier)
        {
            //Do the indent.
            indentCursor();
        }
        //Event complete.
        return;
    }
    case Qt::Key_Return:
    {
        //Disable the soft return.
        //Check whether the modifiers contains Shift.
        if(event->modifiers() & KNG::SHIFT)
        {
            //Remove the modifiers.
            event->setModifiers(event->modifiers() & ~KNG::SHIFT);
        }
        break;
    }
    case Qt::Key_Insert:
    {
        //Change the overwrite mode.
        setOverwriteMode(!overwriteMode());
        return;
    }
    case Qt::Key_Up:
    case Qt::Key_Down:
        if(event->modifiers() == KNG::CTRL)
        {
            //Just change the vertical scroll bar.
            if(event->key() == Qt::Key_Up)
            {
                verticalScrollBar()->setValue(verticalScrollBar()->value() - 1);
            }
            else
            {
                verticalScrollBar()->setValue(verticalScrollBar()->value() + 1);
            }
            event->accept();
            return;
        }
        //If alt modifiters is applied, enter vertical mode.
        if(event->modifiers() & KNG::ALT)
        {
            if(m_columnBlockNumber == -1)
            {
                QTextCursor tc = textCursor();
                //Check the text cursor selection pos.
                if(tc.hasSelection())
                {
                    QTextCursor startCursor = textCursor();
                    //Check whether the cursor is at the start or the end.
                    startCursor.setPosition(tc.position() == tc.selectionStart() ?
                                                tc.selectionEnd():
                                                tc.selectionStart());
                    setColumnCursor(startCursor);
                }
                else
                {
                    //Set the column cursor.
                    setColumnCursor(textCursor());
                }
            }
            //Remove the modifiers.
            event->setModifiers(Qt::NoModifier);
        }
        else
        {
            //We have to clear column selection.
            clearColumnCursor();
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
    {
        //Check modifiers.
        if(event->modifiers() == Qt::NoModifier)
        {
            //Backup the previous pos.
            const int currentBlockNum = textCursor().blockNumber();
            //Do movement.
            QPlainTextEdit::keyPressEvent(event);
            //Check whether the block num changed.
            if(currentBlockNum != textCursor().blockNumber())
            {
                //Clear the column selection.
                clearColumnCursor();
            }
            else
            {
                //Update the column selection position.
                m_columnOffset = textCursor().positionInBlock();
                updateExtraSelections();
            }
            return;
        }
        //If alt modifiers is applied, enter vertical mode.
        if(event->modifiers() & KNG::ALT)
        {
            //Remove ALT modifier first.
            event->setModifiers(event->modifiers() & ~KNG::ALT);
            //Check whether we have the start selection or not.
            if(m_columnBlockNumber != -1)
            {
                //Remove the alt modifiers.
                event->setModifiers(event->modifiers() &~KNG::SHIFT);
                //Update the viewport.
                viewport()->update();
            }
        }
        break;
    }
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        if(m_columnBlockNumber != -1)
        {
            //In block edit, remove all.
            QTextCursor tc = textCursor();
            tc.beginEditBlock();
            const int insBlock = tc.blockNumber();
            int insOffset = tc.positionInBlock();
            int textMove = 0;
            //Generate all the input cursor.
            auto ccs = columnCursors();
            bool hasSelection = false;
            //Keep the position.
            for(auto cc : ccs)
            {
                if(cc.hasSelection())
                {
                    hasSelection = true;
                }
            }
            //Check the final selections.
            if(hasSelection)
            {
                insOffset = qMin(m_columnOffset, textCursor().positionInBlock());
            }
            //Check the key operation.
            bool removePrev = event->key() == Qt::Key_Backspace;
            //Insert the text.
            for(auto cc : ccs)
            {
                //Check the selection.
                if(cc.hasSelection())
                {
                    cc.removeSelectedText();
                }
                else
                {
                    //Check validation.
                    if(removePrev)
                    {
                        if(cc.positionInBlock() > 0)
                        {
                            //Execute remove.
                            cc.deletePreviousChar();
                            textMove = 1;
                        }
                    }
                    else
                    {
                        if(cc.positionInBlock() != cc.block().length() - 1)
                        {
                            //Execute the delete.
                            cc.deleteChar();
                        }
                    }
                }
            }
            insOffset -= textMove;
            tc.setPosition(document()->findBlockByNumber(insBlock).position() + insOffset);
            tc.endEditBlock();
            //Update the column position.
            m_columnOffset = insOffset;
            //Update the extra selection.
            updateExtraSelections();
            event->accept();
            return;
        }
        break;
    default:
    {
        //Check column status.
        if(m_columnBlockNumber != -1)
        {
            //Check whether the key press event is selection.
            QString text = event->text();
            if(!text.isEmpty() && (text.at(0).isPrint()))
            {
                QTextCursor tc = textCursor();
                tc.beginEditBlock();
                const int insBlock = tc.blockNumber();
                int insOffset = tc.positionInBlock();
                //Generate all the input cursor.
                auto ccs = columnCursors();
                for(auto cc : ccs)
                {
                    if(cc.blockNumber() == insBlock)
                    {
                        //Check whether the cursor has selection.
                        if(cc.hasSelection())
                        {
                            //Update the insert offset.
                            insOffset = cc.selectionStart() - cc.block().position();
                        }
                        break;
                    }
                }
                //Insert the for each line.
                for(auto cc : ccs)
                {
                    QString patchedText = text;
                    int currentInsOffset = cc.positionInBlock();
                    if(cc.hasSelection())
                    {
                        currentInsOffset = cc.selectionStart() - cc.block().position();
                    }
                    //Check the row selection.
                    if(currentInsOffset < insOffset)
                    {
                        //Fill the space to the block
                        patchedText.prepend(QString(insOffset - currentInsOffset, ' '));
                    }
                    //Check the text cursor position.
                    cc.insertText(patchedText);
                }
                tc.setPosition(document()->findBlockByNumber(insBlock).position() +
                               insOffset + text.length());
                tc.endEditBlock();
                //Update the column position.
                m_columnOffset = tc.positionInBlock();
                //Update the extra selection.
                updateExtraSelections();
                event->accept();
                return;
            }
        }
        break;
    }
    }
    //Do original key press event.
    QPlainTextEdit::keyPressEvent(event);
    //Check the copy validation.
    if(m_columnBlockNumber != -1)
    {
        //Check whether the column mates the current offset.
        emit copyAvailable(m_columnOffset != textCursor().positionInBlock());
    }
}

void KNTextEditor::mousePressEvent(QMouseEvent *event)
{
    //Check the mouse event.
    if(event->button() == Qt::LeftButton)
    {
        if(m_columnBlockNumber != -1)
        {
            //Clear the result.
            clearColumnCursor();
            //Update viewport.
            viewport()->update();
        }
    }
    //Do the original event.
    QPlainTextEdit::mousePressEvent(event);
}

void KNTextEditor::paintEvent(QPaintEvent *event)
{
    //Update the cursor position.
    QPainter painter(viewport());
    auto tc = textCursor();
    if(m_editorOptions & HighlightCursor)
    {
        QRectF cursorLine = blockBoundingGeometry(textCursor().block()).translated(
                    contentOffset());
        int height = (tc.blockNumber() == document()->blockCount() - 1) ?
                    cursorLine.height() - 3:
                    cursorLine.height();
        painter.fillRect(cursorLine.x(), cursorLine.y(), contentsRect().width(), height,
                         m_currentLine.format.background());
    }
    painter.end();
    //Do original paint event.
    QPlainTextEdit::paintEvent(event);
    //Start paint again.
    painter.begin(viewport());
    int filter = (CursorDisplay | CursorVisible);
    if((m_editorOptions & filter) == filter)
    {
        if(m_columnBlockNumber == -1)
        {
            //Draw the current text cursor.
            QRect cr = cursorRect();
            if(overwriteMode())
            {
                cr = QRect(cr.x(), cr.bottom() - 1, cr.width(), knUi->height(1));
            }
            else
            {
                cr.setWidth(knUi->width(1));
            }
            //Draw the cursor.
            painter.fillRect(cr, Qt::black);
        }
        else
        {
            //Draw the block content selection.
            QTextCursor startCursor = textCursor();
            auto startBlock = document()->findBlockByNumber(m_columnBlockNumber);
            startCursor.setPosition(startBlock.position() + m_columnOffset);
            //Get the rect positions.
            QRect startRect = cursorRect(startCursor),
                    endRect = cursorRect();
            //Get the selection rect.
            int top = qMin(startRect.top(), endRect.top()),
                    bottom = qMax(startRect.bottom(), endRect.bottom());
            //Draw the cursor.
            painter.fillRect(QRect(endRect.x(), top, knUi->width(1), bottom - top),
                             Qt::black);
        }
    }
}

void KNTextEditor::scrollContentsBy(int dx, int dy)
{
    //Directly update the scroll result.
    QPlainTextEdit::scrollContentsBy(dx, dy);
    //Update the current selection.
    updateExtraSelections();
}

void KNTextEditor::onBlockCountChanged(int newBlockCount)
{
    //Update the block count.
    //Calculate the line count width.
    int lineNumWidth = 0;
    if(m_editorOptions & LineNumberDisplay)
    {
        QFontMetrics fontMetrics(font());
        int charWidth = fontMetrics.boundingRect("9").width();
        lineNumWidth = charWidth * (QString::number(newBlockCount).length() + 3);
    }
    //Add the fixed other width.
    m_panel->setFixedWidth(m_panel->panelBaseWidth() + lineNumWidth);
    m_panel->setLineNumberWidth(lineNumWidth);
    //Update the margin.
    updateViewportMargins();
}

void KNTextEditor::updatePanelArea(const QRect &rect, int dy)
{
    //When the panel is scrolled, update the entire panel.
    if(dy)
    {
        m_panel->scroll(0, dy);
    }
    else
    {
        //Or we only have to update the specific part.
        m_panel->update(0, rect.y(), m_panel->width(), rect.height());
    }
    //When there is no scroll bar, just update all.
    if(rect.contains(viewport()->rect()))
    {
        onBlockCountChanged(blockCount());
    }
}

void KNTextEditor::onCursorPositionChanged()
{
    //Highlight the current line.
    updateExtraSelections();
    //Emit the signal.
    auto tc = textCursor();
    int posBegin=tc.blockNumber(), posEnd=tc.positionInBlock(),
            selLength = -1, selLines = -1;
    if(m_columnBlockNumber == -1)
    {
        int selEnd = tc.selectionEnd(), selStart = tc.selectionStart();
        tc.setPosition(selEnd);
        int endRow = tc.blockNumber();
        tc.setPosition(selStart);
        int startRow = tc.blockNumber();
        selLength = selEnd - selStart;
        selLines = selLength == 0 ? 0 : (endRow - startRow + 1);
    }
    emit cursorPosUpdate(posBegin, posEnd, selLength, selLines);
}

void KNTextEditor::onTextChanged()
{
    emit lengthChange(document()->characterCount(),
                      document()->lineCount());
}

void KNTextEditor::onCursorUpdate()
{
    //Check cursor display state.
    if(m_editorOptions & CursorDisplay)
    {
        //Change the cursor state.
        m_editorOptions ^= CursorVisible;
        //Update the viewport.
        viewport()->update();
    }
    //No matter how, we need to update the cursor area.
    QRectF currentRect = blockBoundingGeometry(textCursor().block()).translated(
                contentOffset());
    emit updateRequest(QRect(currentRect.x(),
                             currentRect.y(), width(),
                             currentRect.height()), 0);
}

void KNTextEditor::onEditorFontChanged()
{
    //Reset the font from the global.
    setFont(knGlobal->editorDefaultFont());
}

void KNTextEditor::onWrapModeChange(bool wrap)
{
    //Set the wrap mode.
    setWordWrapMode(wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere :
                           QTextOption::NoWrap);
}

void KNTextEditor::onResultDisplayChange(bool showResult)
{
    //Update the result display switch.
    m_showResults = showResult;
    //Update the selection.
    updateExtraSelections();
}

void KNTextEditor::quickSearchUi(const QTextBlock &block)
{
    //Do quick search for the display part.
    QScopedPointer<KNTextSearcher> searcher;
    searcher.reset(new KNTextSearcher);
    searcher->search(block, m_quickSearchKeyword,
                     height()/fontMetrics().lineSpacing()+2,
                     false, true, m_quickSearchCode, m_quickSearchSense);
    //Update the selection.
    updateExtraSelections();
}

void KNTextEditor::quickSearchCheck(const QTextBlock &block)
{
    //Check the block.
    auto data = blockData(block);
    if(Q_UNLIKELY(data == nullptr))
    {
        return;
    }
    //Check whether the block has been searched.
    data->lockQuickSearch();
    if(!KNTextSearcher::isSearchLatest(data, m_quickSearchCode))
    {
        data->unlockQuickSearch();
        //Start update the entire UI.
        quickSearchUi(block);
        //Lock the data.
        data->lockQuickSearch();
    }
    //Unlock the data.
    data->unlockQuickSearch();
}

void KNTextEditor::setCodecName(const QString &codecName)
{
    //Save the codec name.
    m_codecName = codecName.toLatin1();
    //Emit the signal.
    emit fileCodecChange(codecName);
}

void KNTextEditor::quickSearch(const QString &keywords, Qt::CaseSensitivity cs,
                               int position)
{
    //Save the keywords and settings.
    m_quickSearchKeyword = keywords;
    m_quickSearchSense = cs;
    //Increase the search code.
    ++m_quickSearchCode;
    //Update the length.
    const int len = keywords.length();
    //Clear the previous threads.
    if(!m_quickSearchNext.isNull())
    {
        m_quickSearchNext->quit();
        m_futureNext.waitForFinished();
    }
    if(!m_quickSearchPrev.isNull())
    {
        m_quickSearchPrev->quit();
        m_futurePrev.waitForFinished();
    }
    //Do the search through the entire editor.
    QTextCursor tc = textCursor();
    tc.clearSelection();
    tc.setPosition(position);
    setTextCursor(tc);
    //Check the keywords.
    if(len == 0)
    {
        QTextBlock block = document()->firstBlock();
        //Clear all the data result.
        while(block.isValid())
        {
            //Does search for the string.
            auto data = blockData(block);
            //Clear the search the result.
            data->results.clear();
            //Loop to the end of the block.
            block = block.next();
        }
        //Update the extra search.
        updateExtraSelections();
    }
    else
    {
        //Do instant search.
        quickSearchUi(firstVisibleBlock());
        //Search the visible part.
        m_quickSearchNext.reset(new KNTextSearcher);
        m_quickSearchPrev.reset(new KNTextSearcher);
        //Start searching.
        m_futureNext = QtConcurrent::run(
                    &KNTextSearcher::search, m_quickSearchNext.data(),
                    firstVisibleBlock(), m_quickSearchKeyword, -1, true, true,
                    m_quickSearchCode, m_quickSearchSense);
        m_futurePrev = QtConcurrent::run(
                    &KNTextSearcher::search, m_quickSearchPrev.data(),
                    firstVisibleBlock(), m_quickSearchKeyword, -1, true, false,
                    m_quickSearchCode, m_quickSearchSense);
        //Move to next.
        quickSearchNext(position);
    }
}

void KNTextEditor::quickSearchNext(int position)
{
    //Construct a cursor at the beginning of the document.
    QTextCursor tc = textCursor();
    if(position != -1)
    {
        tc.setPosition(position);
    }
    //Check the search forward.
    if(!quickSearchForward(tc))
    {
        tc.movePosition(QTextCursor::Start);
        quickSearchForward(tc);
    }
}

void KNTextEditor::quickSearchPrev(int position)
{
    //Check the search backward.
    QTextCursor tc = textCursor();
    if(position != -1)
    {
        tc.setPosition(position);
    }
    if(!quickSearchBackward(tc))
    {
        //Construct a cursor at the end of the document.
        tc.movePosition(QTextCursor::End);
        quickSearchBackward(tc);
    }
}

void KNTextEditor::clearAllMarks()
{
    //Loop and clear all the marks.
    QTextBlock block = document()->firstBlock();
    while(block.isValid())
    {
        //Append the mark to the block data.
        blockData(block)->marks = QVector<KNTextBlockData::MarkBlock>();
        //Switch to next block.
        block = block.next();
    }
    //Update the selection.
    updateExtraSelections();
}

bool KNTextEditor::quickSearchForward(const QTextCursor &cursor)
{
    //Search from the current cursor.
    auto tc = textCursor();
    if(cursor.hasSelection())
    {
        tc.setPosition(cursor.selectionEnd());
    }
    else
    {
        tc.setPosition(cursor.position());
    }
    for(auto i = tc.block(); i.isValid(); i=i.next())
    {
        //Extract the user data.
        quickSearchCheck(i);
        //Check the search result.
        auto data = blockData(i);
        data->lockQuickSearch();
        if(!data->results.isEmpty())
        {
            //Now compare the position.
            int cursorPos = (i.blockNumber() == tc.blockNumber()) ?
                        tc.positionInBlock() : -1;
            for(int j=0; j<data->results.size(); ++j)
            {
                //Check the position.
                if(data->results.at(j).pos > cursorPos)
                {
                    //We find the cursor, move the cursor.
                    tc.setPosition(i.position() + data->results.at(j).pos);
                    tc.movePosition(QTextCursor::NextCharacter,
                                    QTextCursor::KeepAnchor,
                                    data->results.at(j).length);
                    data->unlockQuickSearch();
                    //Set the cursor.
                    setTextCursor(tc);
                    return true;
                }
            }
        }
        data->unlockQuickSearch();
    }
    return false;
}

bool KNTextEditor::quickSearchBackward(const QTextCursor &cursor)
{
    //Search from the current cursor.
    auto tc = cursor;
    if(tc.hasSelection())
    {
        tc.setPosition(tc.selectionStart());
    }
    for(auto i = tc.block(); i.isValid(); i=i.previous())
    {
        //Extract the user data.
        quickSearchCheck(i);
        //Check the search result.
        auto data = blockData(i);
        data->lockQuickSearch();
        if(!data->results.isEmpty())
        {
            //Now compare the position.
            bool isCursorLine = i.blockNumber() == tc.blockNumber();
            int cursorPos = tc.positionInBlock();
            for(int j=data->results.size() - 1; j > -1; --j)
            {
                //Check the position.
                if(!isCursorLine || (data->results.at(j).pos < cursorPos))
                {
                    //We find the cursor, move the cursor.
                    tc.setPosition(i.position() + data->results.at(j).pos);
                    tc.movePosition(QTextCursor::NextCharacter,
                                    QTextCursor::KeepAnchor,
                                    data->results.at(j).length);
                    data->unlockQuickSearch();
                    setTextCursor(tc);
                    return true;
                }
            }
        }
        data->unlockQuickSearch();
    }
    return false;
}

QList<QTextCursor> KNTextEditor::columnCopy()
{
    //Extract the block information.
    QString buffer;
    auto ccs = columnSelectionText(buffer);
    //Set the column text to clipboard.
    knGlobal->copyText(buffer);
    return ccs;
}

QList<QTextCursor> KNTextEditor::columnSelectionText(QString &selectionText) const
{
    auto ccs = columnCursors();
    QStringList columnText;
    columnText.reserve(ccs.size());
    for(auto cc : ccs)
    {
        columnText.append(cc.selectedText());
    }
    //Set the selection text.
    selectionText = columnText.join("\n");
    //Set the column text to clipboard.
    return ccs;
}

void KNTextEditor::updateExtraSelections()
{
    //Construct the selection list.
    QList<QTextEdit::ExtraSelection> selections;
    //Current lines.
    if(m_editorOptions & HighlightCursor)
    {
        m_currentLine.cursor = textCursor();
        m_currentLine.cursor.clearSelection();
        selections.append(m_currentLine);
    }

    //Column selections.
    if(m_columnBlockNumber != -1)
    {
        //Check whether we have to create the selections.
        if(m_columnOffset != textCursor().positionInBlock())
        {
            //Construct the format.
            QTextCharFormat format;
            format.setBackground(palette().brush(QPalette::Highlight));
            format.setForeground(palette().brush(QPalette::HighlightedText));
            //Construct the block start.
            auto ccs = columnCursors();
            for(auto cc : ccs)
            {
                //Construct the selection.
                QTextEdit::ExtraSelection selection;
                selection.cursor = cc;
                selection.format = format;
                selections.append(selection);
            }
        }
    }

    // Quick search result.
    auto block = firstVisibleBlock();
    QRectF blockRect = blockBoundingGeometry(block).translated(contentOffset());
    auto searchFormat = knGlobal->quickSearchFormat();
    while(block.isValid() && blockRect.bottom() < height())
    {
        //Load for all the items to the selections.
        auto data = blockData(block);
        if(!data)
        {
            break;
        }
        if(!m_quickSearchKeyword.isEmpty() && m_showResults)
        {
            //Add all the selection to the extra selections.
            QTextEdit::ExtraSelection selection;
            selection.format = searchFormat;
            //Ensure block information.
            quickSearchCheck(block);
            for(auto result : data->results)
            {
                QTextCursor tc = textCursor();
                tc.setPosition(block.position() + result.pos);
                tc.movePosition(QTextCursor::NextCharacter,
                                QTextCursor::KeepAnchor,
                                result.length);
                QTextEdit::ExtraSelection selection;
                selection.format = searchFormat;
                selection.cursor = tc;
                selections.append(selection);
            }
        }
        //Move to the next block.
        block = block.next();
        blockRect = blockBoundingGeometry(block);
    }

    setExtraSelections(selections);
}

void KNTextEditor::updateHighlighter(KNSyntaxHighlighter *highlighter)
{
    //Check whether the highlighter is set.
    if(highlighter)
    {
        m_highlighter = highlighter;
    }
    else
    {
        //Set the new highlighter.
        m_highlighter = KNSyntaxHighlighter::get(m_filePath);
    }
    //Update the panel based on the highlighter.
    m_panel->setShowFold(m_highlighter->hasCodeLevel());
    onBlockCountChanged(document()->blockCount());
    //Configure the highlighter.
    m_highlighter->setDocument(document());
    m_highlighter->rehighlight();
}

int KNTextEditor::tabSpacePosition(const QTextBlock &block, int pos,
                                   int tabSpacing)
{
    //Based on the position and the characters, calculate the current characters.
    int column = 0;
    QString &&blockText = block.text();
    for (int i=0; i<pos; ++i)
    {
        //Check whether the iteration is \t.
        if(blockText.at(i) == '\t')
        {
            //Find the nearest variable.
            column = cellTabSpacing(column, tabSpacing);
            continue;
        }
        //Simply increase a char.
        column += 1;
    }
    return column;
}

QString KNTextEditor::levelLevelString(int spaceLevel, int tabSpacing)
{
    if(knGlobal->replaceTab())
    {
        //Add one more tab spacings to space level.
        return QString(spaceLevel, ' ');
    }
    //Calculate the tab level.
    int tabLevel = spaceLevel / tabSpacing,
        remainSpaces = spaceLevel - tabLevel * tabSpacing;
    //Construct the level text.
    return QString(tabLevel, '\t') + QString(remainSpaces, ' ');

}

int KNTextEditor::cellTabSpacing(int spacePos, int tabSpacing)
{
    int nc = (spacePos / tabSpacing) * tabSpacing;
    if(nc < spacePos)
    {
        return nc + tabSpacing;
    }
    return spacePos + tabSpacing;
}

QString KNTextEditor::codecName() const
{
    return m_codecName;
}

QString KNTextEditor::selectedText() const
{
    if(m_columnBlockNumber == -1)
    {
        return textCursor().selectedText();
    }
    //Or else we have to generate the column text.
    QString buffer;
    columnSelectionText(buffer);
    return buffer;
}

void KNTextEditor::setCursorVisible(bool yes)
{
    //Save as the flash switch.
    m_editorOptions = yes ? (m_editorOptions | CursorDisplay) :
                            (m_editorOptions & (~CursorDisplay));
    //Update the widget.
    viewport()->update();
}

void KNTextEditor::setLineNumberVisible(bool yes)
{
    //Update the editor options.
    m_editorOptions = yes ? (m_editorOptions | LineNumberDisplay) :
                            (m_editorOptions & (~LineNumberDisplay));
    //Update the viewport margin.
    onBlockCountChanged(document()->blockCount());
}

void KNTextEditor::setBookmarkVisible(bool yes)
{
    //Update the flag at panel.
    m_panel->setShowMarks(yes);
    //Update the viewport margin.
    onBlockCountChanged(document()->blockCount());
}

void KNTextEditor::setHighlightCursor(bool yes)
{
    //Update the editor options.
    m_editorOptions = yes ? (m_editorOptions | HighlightCursor) :
                            (m_editorOptions & (~HighlightCursor));
    //Update the viewport and current line.
    updateExtraSelections();
}

void KNTextEditor::undo()
{
    //Clear the column selection.
    clearColumnCursor();
    //Do undo.
    QPlainTextEdit::undo();
}

void KNTextEditor::redo()
{
    //Clear the column selection.
    clearColumnCursor();
    //Do redo.
    QPlainTextEdit::redo();
}

void KNTextEditor::cut()
{
    //Check column selection.
    if(m_columnBlockNumber != -1)
    {
        //Do column copy.
        auto ccs = columnCopy();
        QTextCursor tc = textCursor();
        tc.beginEditBlock();
        //Remove all these column.
        for(int i=ccs.size()-1; i>-1; --i)
        {
            //Remove the selection.
            tc.setPosition(ccs.at(i).selectionStart());
            tc.setPosition(ccs.at(i).selectionEnd(), QTextCursor::KeepAnchor);
            tc.removeSelectedText();
        }
        tc.endEditBlock();
        return;
    }
    //Do original cut.
    QPlainTextEdit::cut();
}

void KNTextEditor::copy()
{
    //Check column selection.
    if(m_columnBlockNumber != -1)
    {
        //Do column copy.
        columnCopy();
        return;
    }
    //Do original copy.
    QPlainTextEdit::copy();
}

void KNTextEditor::loadUseCodec(const QByteArray &codecName)
{
    //Load the file.
    loadFrom(m_filePath, codecName);
}

int KNTextEditor::firstNonSpacePos(const QTextBlock &block)
{
    QString &&blockText = block.text();
    for(int i=0; i<blockText.size(); ++i)
    {
        const QChar &&c = blockText.at(i);
        if(c != '\t' && c != ' ')
        {
            return i;
        }
    }
    return blockText.size();
}

bool KNTextEditor::saveToFile(const QString &filePath)
{
    //Open the file to write.
    QFile targetFile(filePath);
    if(!targetFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(
                    this,
                    tr("Save failed"),
                    tr("Please check if this file is opened in another program."));
        return false;
    }
    //Write the file content.
    QTextCodec *codec = QTextCodec::codecForName(m_codecName);
    targetFile.write(codec->fromUnicode(toPlainText()));
    //Close the file.
    targetFile.close();
    //Change the saving state.
    document()->setModified(false);
    return true;
}

void KNTextEditor::setFilePath(const QString &filePath)
{
    //Save the file path.
    m_filePath = filePath;
    //Update the document title.
    QFileInfo info(m_filePath);
    setDocumentTitle(info.fileName());
    //Emit the title change signal.
    emit documentTitleChange();
    //Change the modify state.
    document()->setModified(false);
}

void KNTextEditor::updateViewportMargins()
{
    //Update the viewport margins.
    setViewportMargins(m_panel->width(), 0, 0, 0);
}

void KNTextEditor::setColumnCursor(const QTextCursor &cursor)
{
    //Append the cursor.
    setColumnCursor(cursor.blockNumber(), cursor.positionInBlock());
}

void KNTextEditor::setColumnCursor(int blockNum, int offset)
{
    m_columnBlockNumber = blockNum;
    m_columnOffset = offset;
}

void KNTextEditor::clearColumnCursor()
{
    //Clear the cursor.
    m_columnBlockNumber = -1;
    m_columnOffset = -1;
    //Clear the extra selection.
    updateExtraSelections();
}

QList<QTextCursor> KNTextEditor::columnCursors() const
{
    //Create the cursor list.
    QList<QTextCursor> cursors;
    //Construct the block start.
    QTextCursor tc = textCursor();
    int blockStart = qMin(tc.blockNumber(), m_columnBlockNumber),
            blockEnd = qMax(tc.blockNumber(), m_columnBlockNumber),
            offsetStart = qMin(tc.positionInBlock(), m_columnOffset),
            offsetEnd = qMax(tc.positionInBlock(), m_columnOffset);
    for(int i=blockStart; i<=blockEnd; ++i)
    {
        //Extract the block.
        auto block = document()->findBlockByNumber(i);
        //Construct the block selection cursor.
        QTextCursor sc = tc;
        if(offsetStart < block.length())
        {
            sc.setPosition(block.position() + offsetStart);
        }
        else
        {
            sc.setPosition(block.position() + block.length() - 1);
        }
        if(offsetEnd < block.length())
        {
            sc.setPosition(block.position() + offsetEnd, QTextCursor::KeepAnchor);
        }
        else
        {
            sc.setPosition(block.position() + block.length() - 1,
                           QTextCursor::KeepAnchor);
        }
        sc.setKeepPositionOnInsert(true);
        //Construct the selection.
        cursors.append(sc);
    }
    return cursors;
}

QString KNTextEditor::filePath() const
{
    return m_filePath;
}

void KNTextEditor::setOverwriteMode(bool overwrite)
{
    //Update the overwrite mode.
    QPlainTextEdit::setOverwriteMode(overwrite);
    //Emit the signal.
    emit overwriteModeChange(overwriteMode());
}

bool KNTextEditor::isOnDisk() const
{
    return !m_filePath.isEmpty();
}

void KNTextEditor::setReadOnly(bool ro)
{
    //Do the original set.
    QPlainTextEdit::setReadOnly(ro);
    //Update the signal.
    emit readOnlyChange(isReadOnly());
}

KNTextBlockData *KNTextEditor::blockData(const QTextBlock &block)
{
    //Get the block data.
    return static_cast<KNTextBlockData *>(block.userData());
}

KNTextBlockData *KNTextEditor::currentBlockData()
{
    //Get the cursort block data.
    return blockData(textCursor().block());
}

bool KNTextEditor::canCopy() const
{
    return m_columnBlockNumber == -1 ?
                textCursor().hasSelection() :
                m_columnOffset != textCursor().positionInBlock();
}

void KNTextEditor::addLink(const QMetaObject::Connection &connection)
{
    m_connections.append(connection);
}

void KNTextEditor::removeAllLinks()
{
    //Loop and disconnect all the links.
    for(auto link : m_connections)
    {
        disconnect(link);
    }
    //Clear the connections.
    m_connections.clear();
}

bool KNTextEditor::saveAs(const QString &filePath)
{
    //Try to save to file path.
    if(saveToFile(filePath))
    {
        //Set the file path.
        setFilePath(filePath);
        return true;
    }
    return false;
}

void KNTextEditor::setSymbolDisplayMode(int mode)
{
    //Fetch the options.
    QTextOption option = document()->defaultTextOption();
    //Based on the mode, append different flags.
    auto showFlags = QTextOption::ShowTabsAndSpaces |
                     QTextOption::ShowLineAndParagraphSeparators |
                     QTextOption::ShowDocumentTerminator;
    //Clear all the flags first.
    option.setFlags(option.flags() & ~showFlags);
    switch(mode)
    {
    case SymbolNormal:
        break;
    case SymbolSpaces:
        option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
        break;
    case SymbolEndOfBlock:
        option.setFlags(option.flags() |
                        QTextOption::ShowLineAndParagraphSeparators |
                        QTextOption::ShowDocumentTerminator);
        break;
    case SymbolAll:
        option.setFlags(option.flags() | showFlags);
        break;
    }
    //Update the default text options.
    document()->setDefaultTextOption(option);
}

void KNTextEditor::syncWithStatusBar()
{
    //Update the text cursor and text length.
    onCursorPositionChanged();
    onTextChanged();
    //Update the overwrite signal.
    emit overwriteModeChange(overwriteMode());
    //Update the codec.
    emit fileCodecChange(m_codecName);
}

void KNTextEditor::cursorDelete()
{
    //Get the cursor.
    auto tc = textCursor();
    //If the text cursor has selection, then delete the data.
    if(tc.hasSelection())
    {
        //Remove all the data.
        tc.removeSelectedText();
        return;
    }
    //Or else just delete the char.
    tc.deleteChar();
}

void KNTextEditor::indentCursor()
{
    //Fetch parameters from global.
    int tabSpacing = knGlobal->tabSpacing(), insertPos;
    //Check the text cursor selection contains multiple lines.
    auto tc = textCursor();
    tc.beginEditBlock();
    if(tc.hasSelection())
    {
        //Check whether we have multiple blocks in the selection.
        tc.setKeepPositionOnInsert(true);
        auto startBlock = document()->findBlock(tc.selectionStart()),
                endBlock = document()->findBlock(tc.selectionEnd());
        if(startBlock.blockNumber() != endBlock.blockNumber())
        {
            //Indent all lines.
            for(int i=startBlock.blockNumber(); i<=endBlock.blockNumber(); ++i)
            {
                //Fetch the current block.
                auto b = document()->findBlockByNumber(i);
                //Calculate the space pos.
                int textStart = firstNonSpacePos(b),
                    spaceLevel = tabSpacePosition(b, textStart, tabSpacing);
                //Insert the level text.
                tc.setPosition(b.position());
                tc.setPosition(b.position() + textStart,
                               QTextCursor::KeepAnchor);
                tc.removeSelectedText();
                tc.insertText(levelLevelString(spaceLevel + tabSpacing,
                                               tabSpacing));
            }
            tc.endEditBlock();
            return;
        }
        else
        {
            //Mark the insert position as selection start.
            insertPos = tc.selectionStart();
            //Clear the selection.
            tc.removeSelectedText();
        }
    }
    else
    {
        //Use the text cursor position as the position.
        insertPos = tc.position();
    }
    //Find the current block index.
    auto currentBlock = document()->findBlock(insertPos);
    //Just do normal tab insert.
    if(knGlobal->replaceTab())
    {
        //Calculate and insert spacing.
        int tabPos = tabSpacePosition(currentBlock, tc.positionInBlock(),
                                      tabSpacing);
        //Check whether the tab position is a tab stop.
        if(tabPos % tabSpacing)
        {
            //Find the cell position, insert enough data.
            int cellTabPos = cellTabSpacing(tabPos, tabSpacing);
            tc.insertText(QString(cellTabPos - tabPos, ' '));
        }
        else
        {
            //Insert a set of tab spacing.
            tc.insertText(QString(tabSpacing, ' '));
        }
    }
    else
    {
        //Just insert a tab.
        tc.insertText("\t");
    }
    //Set the text cursor.
    tc.endEditBlock();
    setTextCursor(tc);
}

void KNTextEditor::unindentCursor()
{
    //Fetch parameters from global.
    int tabSpacing = knGlobal->tabSpacing();
    //Check the text cursor selection contains multiple lines.
    auto tc = textCursor();
    if(tc.hasSelection())
    {
        //Start tab inserting.
        tc.beginEditBlock();
        tc.setKeepPositionOnInsert(true);
        auto startBlock = document()->findBlock(tc.selectionStart()),
                endBlock = document()->findBlock(tc.selectionEnd());
        if(startBlock.blockNumber() != endBlock.blockNumber())
        {
            //Indent all lines.
            for(int i=startBlock.blockNumber(); i<=endBlock.blockNumber(); ++i)
            {
                //Fetch the current block.
                auto b = document()->findBlockByNumber(i);
                //Calculate the space pos.
                int textStart = firstNonSpacePos(b),
                    spaceLevel = tabSpacePosition(b, textStart, tabSpacing) -
                        tabSpacing;
                if(spaceLevel < 0)
                {
                    spaceLevel = 0;
                }
                //Insert the level text.
                tc.setPosition(b.position());
                tc.setPosition(b.position() + textStart,
                                      QTextCursor::KeepAnchor);
                tc.removeSelectedText();
                tc.insertText(levelLevelString(spaceLevel, tabSpacing));
            }
        }
        tc.endEditBlock();
        return;
    }
    //If we don't have selection, check whether the position is at content.
    auto currentBlock = document()->findBlock(tc.position());
    //Find the content start.
    int textStart = firstNonSpacePos(currentBlock);
    if(tc.positionInBlock() <= textStart)
    {
        int spaceLevel = tabSpacePosition(currentBlock, textStart, tabSpacing);
        //Reduce one spaceing level.
        spaceLevel -= tabSpacing;
        if(spaceLevel < 0)
        {
            spaceLevel = 0;
        }
        //Convert the space level into expressions.
        //Insert the level text.
        tc.beginEditBlock();
        tc.setPosition(currentBlock.position());
        tc.setPosition(currentBlock.position() + textStart,
                        QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.insertText(levelLevelString(spaceLevel, tabSpacing));
        tc.endEditBlock();
    }
}

void KNTextEditor::convertSelectCase(int letterCase)
{
    //Extract the selection text.
    auto tc = textCursor();
    if(!tc.hasSelection())
    {
        return;
    }
    tc.beginEditBlock();
    int startPos = tc.selectionStart(), endPos = tc.selectionEnd();
    //Convert the string into target cases.
    QString selectedText = tc.selectedText();
    switch(letterCase)
    {
    case Upper:
        selectedText = selectedText.toUpper();
        break;
    case Lower:
        selectedText = selectedText.toLower();
        break;
    case Proper:
    {
        QStringList parts = selectedText.split(' ');
        for(int i=0; i<parts.size(); ++i)
        {
            if(parts.at(i).length() > 0)
            {
                //Prepare the buffer.
                QString properPart;
                properPart.reserve(parts[i].size());
                //Reaplce the parts.
                properPart.append(parts[i][0].toUpper());
                properPart.append(parts[i].mid(1).toLower());
                //Replace the original string.
                parts[i] = properPart;
            }
        }
        selectedText = parts.join(" ");
        break;
    }
    case Reverse:
    {
        //Check letter by letter, reverse it.
        for(int i=0; i<selectedText.size(); ++i)
        {
            if(selectedText.at(i).isUpper())
            {
                selectedText[i] = selectedText[i].toLower();
                continue;
            }
            if(selectedText.at(i).isLower())
            {
                selectedText[i] = selectedText[i].toUpper();
            }
        }
        break;
    }
    case Random:
    {
        //WTF is this?
        std::srand(std::time(0));
        for(int i=0; i<selectedText.size(); ++i)
        {
            selectedText[i] = (std::rand() & 1) ?
                        selectedText[i].toLower() :
                        selectedText[i].toUpper();
        }
        break;
    }
    }
    //Remove the previous text.
    tc.insertText(selectedText);
    tc.setPosition(startPos);
    tc.setPosition(endPos, QTextCursor::KeepAnchor);
    tc.endEditBlock();
    setTextCursor(tc);
}

void KNTextEditor::showSummary()
{
    //Check whether the file is saved or not.
    QStringList rows;
    if(!m_filePath.isEmpty())
    {
        QFileInfo fileInfo(m_filePath);
        //Construct the file info.
        rows.append(tr("Full file path:\n%1").arg(QDir::toNativeSeparators(m_filePath)));
        rows.append(tr("Created: %1").arg(fileInfo.lastModified().toString()));
        rows.append(tr("Modified: %1").arg(fileInfo.birthTime().toString()));
    }
    rows.append(tr("Lines: %1").arg(QString::number(document()->characterCount())));
    rows.append(tr("Document Length: %3").arg(QString::number(document()->blockCount())));
    //Show the row information.
    QMessageBox::information(this, tr("Summary"), rows.join("\n"));
}
