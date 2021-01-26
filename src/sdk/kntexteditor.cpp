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
#include <QFontDatabase>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QtConcurrent/QtConcurrent>

#include "kntextblockdata.h"
#include "knglobal.h"
#include "kntexteditorpanel.h"
#include "kncodesyntaxhighlighter.h"
#include "knuimanager.h"
#include "kndocumentlayout.h"

#include "kntexteditor.h"

QTextCodec *codecFromName(const char *name)
{
    return QTextCodec::codecForName(name);
}

QTextCodec *decodeString(const QByteArray &data, QTextCodec *codec,
                         QString *convert)
{
    if(convert != nullptr)
    {
        *convert = codec->toUnicode(data);
    }
    return codec;
}

QTextCodec *setDecodeString(const QString &localBuf, QTextCodec *codec,
                            QString *convert)
{
    if(convert != nullptr)
    {
        *convert = localBuf;
    }
    return codec;
}

QTextCodec *KNTextEditor::codecFromData(const QByteArray &data,
                                        QString *convert)
{
    enum EncodeType
    {
        ePureAscii = 0,
        eEscAscii  = 1,
        eHighbyte  = 2
    };
    int state = ePureAscii;
    char mLastChar = '\0';
    const char* aBuf = data.data();
    qsizetype aLen = data.length();
    //Check the BOM, it seems if the data starts with BOM, it is Unicode.
    if(aLen > 2)
    {
        //Check the BOM byte.
        switch(aBuf[0])
        {
        case '\xEF':
        {
            if (('\xBB' == aBuf[1]) && ('\xBF' == aBuf[2]))
            {
                // EF BB BF  UTF-8 encoded BOM.
                return decodeString(data, QTextCodec::codecForName("UTF-8"),
                                    convert);
            }
            break;
        }
        case '\xFE':
        {
            if ('\xFF' == aBuf[1])
            {
                // FE FF  UTF-16, big endian BOM
                return decodeString(data, QTextCodec::codecForName("UTF-16BE"),
                                    convert);
            }
            break;
        }
        case '\xFF':
        {
            if ('\xFE' == aBuf[1])
            {
                // FF FE  UTF-16, little endian BOM
                return decodeString(data, QTextCodec::codecForName("UTF-16LE"),
                                    convert);
            }
            break;
        }
        }
    }
    //Detect whether all the characters are ASCII chars.
    for(int i=0; i < aLen; ++i)
    {
        //other than 0xa0, if every othe character is ascii, the page is ascii
        //Since many Ascii only page contains NBSP
        if (aBuf[i] & '\x80' && aBuf[i] != '\xA0')
        {
            //Non-ASCII byte detected (high-byte in NPP).
            if(eHighbyte != state)
            {
                state = eHighbyte;
                break;
            }
        }
        else
        {
            //ok, just pure ascii so far.
            if(ePureAscii == state &&
                    (aBuf[i] == '\033' || (aBuf[i] == '{' && mLastChar == '~')) )
            {
                //found escape character or HZ "~{"
                state = eEscAscii;
                break;
            }
            //Update last char.
            mLastChar = aBuf[i];
        }
    }
    //Check the input state.
    QString localBuf;
    QByteArrayList trialList;
    switch(state)
    {
    case eEscAscii:
    {
        trialList << "JIS7";
        break;
    }
    case eHighbyte:
    {
        //So first we try the codecs.
        trialList << "UTF-8"
                  << "SJIS" << "EUC-JP"
                  << "EUC-KR"
                  << "Big5"
                  // Why Qt's GB18030 implementation does not raises error?
                  << "GB18030"
                  // MBCS (multibyte character set)
//                  << "windows-1251" << "KOI8-R" << "latin5" << "cyrillic"
//                  << "IBM866" << "windows-1253" << "TIS-620"
//                  // SBCS (single byte character set)
//                  << "latin1"
                     ;
        break;
    }
    default:
        //Well, it is pure ASCII, then we are going to use system codec.
        break;
    }
    //Loop in the trial list, try all the codec in the list.
    for(auto name : trialList)
    {
        //Try the codec.
        auto codec = codecFromName(name);
        QTextCodec::ConverterState cs;
        localBuf = codec->toUnicode(aBuf, aLen, &cs);
        if(cs.invalidChars == 0)
        {
            return setDecodeString(localBuf, codec, convert);
        }
    }
    return decodeString(data, QTextCodec::codecForLocale(), convert);
}

int charAsianWidth(const QChar &c)
{
    //Based on http://www.unicode.org/reports/tr11/.
    ushort unicode = c.unicode();
    return (unicode >= 0x4E00 && unicode <=0x9FFF) ||
            (unicode >= 0x3400 && unicode <= 0x4DBF) ||
            (unicode >= 0xF900 && unicode <= 0xFAFF) ? 2 : 1;
}

KNTextEditor::KNTextEditor(const QString &titleName,
                           const QString &filePath, const QString &codec,
                           QWidget *parent, KNSyntaxHighlighter *highlighter,
                           bool linkWithGlobal) :
    QPlainTextEdit(parent),
    m_vStartSpacePos(-1),
    m_vEndSpacePos(-1),
    m_verticalSelect(false),
    m_quickSearchSense(Qt::CaseInsensitive),
    m_quickSearchCode(0),
    m_showResults(false),
    m_filePath(QString()),
    m_codecName(codec.toLatin1()),
    m_panel(new KNTextEditorPanel(this)),
    m_highlighter(nullptr),
    m_editorOptions(HighlightCursor | CursorDisplay | LineNumberDisplay)
{
    //Set properties.
    setAcceptDrops(false);
    setFrameStyle(QFrame::NoFrame);
    setCursorWidth(0);
    //Configure the extra selections.
    m_currentLine.format.setBackground(QColor(232, 232, 255, 160));
    //Update the viewport margins.
    updateViewportMargins();
    if(linkWithGlobal)
    {
        //Configure the editor based on global settings.
        setSymbolDisplayMode(knGlobal->symbolDisplayMode());
        onWrapModeChange(knGlobal->isWrap());
        onEditorFontChanged();
        onResultDisplayChange(knGlobal->isSearchResultShown());
        onAlignLeftChange(knGlobal->isAlignLeft());
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
        addLink(connect(knGlobal, &KNGlobal::editorAlignLeft,
                        this, &KNTextEditor::onAlignLeftChange));
    }
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

void drawFoldMark(QPainter *painter, QRect buttonRect, KNTextBlockData *data)
{
    //Draw the rect.
    painter->fillRect(buttonRect, QColor(243, 243, 243));
    painter->drawRect(buttonRect);
    if(data->isFold)
    {
        ;
    }
    else
    {
        int foldY = (buttonRect.height() >> 1) + buttonRect.y();
        int foldX = buttonRect.x() + 2;
        //Draw a central line across the rect.
        painter->drawLine(foldX, foldY, foldX + buttonRect.width() - 4, foldY);
    }
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
    int markX = lineNumWidth, foldX = markX + markWidth + knUi->width(2),
            currentBlock = textCursor().blockNumber();
    //Draw the fold area background.
    painter->fillRect(foldX, 0, foldWidth, height(),
                      QColor(240, 240, 240));
    //Draw the content.
    bool drawLineNum = (m_editorOptions & LineNumberDisplay);
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
        QRect foldRect(foldX, area.y(), foldWidth, area.height());

        int foldSize = (area.height() < foldWidth) ?
                    (area.height() - knUi->height(4)) :
                    (foldWidth - knUi->width(4));
        foldSize = qMax(foldSize, 2);
        //Draw the center line.
        int hCenter = foldX + (foldSize >> 1);
        if(data->level)
        {
            painter->drawLine(hCenter, area.y(), hCenter, area.bottom());
        }
        if(data->levelMargin > 0)
        {
            //Draw the half of the bottom.
            painter->drawLine(hCenter, area.y() + (area.height() / 2),
                              hCenter, area.bottom());
            drawFoldMark(painter,
                         QRect(foldX,
                               area.y() + ((area.height() - foldSize) / 2),
                               foldSize, foldSize),
                         data);
        }
        //Move to next block.
        block = block.next();
    }
}

void KNTextEditor::loadFrom(const QString &filePath, const QByteArray &codecName)
{
    //Get the codec from the codec name.
    QTextCodec *codec = nullptr;
    //Based on the codec.
    if(!codecName.isEmpty())
    {
        //Use the default codec.
        codec = QTextCodec::codecForName(codecName);
    }
    //Reload the file.
    loadFrom(filePath, codec);
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
    //Shortcut key map:
    //Check whether the event is sent to us.
    switch(event->key())
    {
    case Qt::Key_Escape:
    {
        //Clear the extra text cursor.
        clearExtraCursor();
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
            //Check whether we are in extra selection mode.
            if(isExtraCursorEnabled())
            {
                int tabSpacing = knGlobal->tabSpacing();
                auto tc = textCursor();
                const int insBlock = tc.blockNumber();
                tc.beginEditBlock();
                QTextCursor refCursor;
                //Indent all the extra cursor.
                for(int i=0; i<m_extraCursors.size(); ++i)
                {
                    //Indent the cursor.
                    auto cc = m_extraCursors.at(i);
                    insertTabAt(cc, tabSpacing);
                    cc.setPosition(cc.selectionEnd());
                    if(insBlock == cc.blockNumber())
                    {
                        refCursor = cc;
                    }
                    m_extraCursors.replace(i, cc);
                }
                tc.endEditBlock();
                //If vertical mode is enabled, update the position.
                if(m_verticalSelect)
                {
                    //Update the vertical position.
                    m_vStartSpacePos = spacePosition(refCursor.block(), refCursor.positionInBlock(), tabSpacing);
                    m_vEndSpacePos = m_vStartSpacePos;
                }
            }
            else
            {
                //Do the indent.
                indentCursor();
            }
        }
        event->accept();
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
    /*
     * Default list:
     * Alt + Up / Down: Not defined.
     * Shift + Up / Down: Select. (Default)
     */
    case Qt::Key_Up:
    case Qt::Key_Down:
        //Alt + Shift + Up / Down: Block select to up / down.
        if(event->modifiers() == (KNG::ALT | KNG::SHIFT))
        {
            //Check whether the text editor in vertical mode or not.
            if(!isVerticalEnabled())
            {
                enterVerticalMode();
            }
            //Move the cursor one row down or up.
            event->setModifiers(Qt::NoModifier);
            //Do the event.
            QPlainTextEdit::keyPressEvent(event);
            //Append the new text cursor.
            appendVerticalCursor(textCursor());
            event->accept();
            return;
        }
        //Cancel the selection.
        clearExtraCursor();
        //Ctrl + Up / Down: Vertical Scroll Up / Down
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
        //Ctrl + Alt + Up / Down: Copy the current line to upper/lower line.
        if(event->modifiers() == (KNG::CTRL | KNG::ALT))
        {
            //Copy the line at specific position.
            QTextCursor tc = textCursor();
            QTextBlock block = tc.block();
            tc.beginEditBlock();
            if(event->key() == Qt::Key_Up)
            {
                //Record the current position.
                //Insert the text to the next line.
                tc.movePosition(QTextCursor::EndOfBlock);
                tc.insertBlock();
                tc.insertText(block.text());
                tc.setPosition(block.position());
                tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                setTextCursor(tc);
            }
            else
            {
                //Insert the text to the previous line.
                tc.movePosition(QTextCursor::StartOfBlock);
                tc.insertText(block.text());
                tc.insertBlock();
                tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                setTextCursor(tc);
            }
            tc.endEditBlock();
            event->accept();
            return;
        }
        //Ctrl + Shift + Up / Down: Move current line to upper/lower line.
        if(event->modifiers() == (KNG::CTRL | KNG::SHIFT))
        {
            //Disable multi-cursor mode.
            //!FIXME: Add codes here;
            //Call the slot here.
            if(event->key() == Qt::Key_Up)
            {
                moveCurrentBlockUp();
            }
            else
            {
                moveCurrentBlockDown();
            }
            event->accept();
            return;
        }
        break;
    /*
     * Key maps:
     * Alt + Left / Right           : Not defined.
     * Shift + Left / Right         : Select. (Default)
     * Ctrl + Left / Right         x: Word left / right move (Default)
     * Ctrl + Alt + Left / Right    : Not defined.
     * Ctrl + Shift + Left / Right x: Select to left / right word. (Default)
     *
     */
    case Qt::Key_Left:
    case Qt::Key_Right:
    {
        //Alt + Shift + Left / Right: Block select to left / right.
        if(event->modifiers() == (KNG::ALT | KNG::SHIFT))
        {
            if(!isVerticalEnabled())
            {
                enterVerticalMode();
            }
            int tabSpacing = knGlobal->tabSpacing();
            //Move all the extra cursors one character to its left or right.
            if(event->key() == Qt::Key_Left)
            {
                //Check the vertical end pos reaches limit.
                if(m_vEndSpacePos > 0)
                {
                    //Reduce 1 position.
                    --m_vEndSpacePos;
                    //Update all the cursors.
                    for(int i=0; i<m_extraCursors.size(); ++i)
                    {
                        auto cc = m_extraCursors.at(i);
                        auto block = cc.block();
                        //Move the cursor to the new position.
                        cc.setPosition(block.position() + textPosition(block, m_vEndSpacePos, tabSpacing),
                                       QTextCursor::KeepAnchor);
                        m_extraCursors.replace(i, cc);
                    }
                }
                //Check whether the move could move to next block.
                auto cc = textCursor();
                if(cc.positionInBlock() > 0)
                {
                    //Move the cursor one row down or up.
                    event->setModifiers(Qt::NoModifier);
                    //Do the event.
                    QPlainTextEdit::keyPressEvent(event);
                }
            }
            else
            {
                //Loop and increase the space position.
                int expectEndPos = m_vEndSpacePos + 1, actualEndPos = -1;
                for(int i=0; i<m_extraCursors.size(); ++i)
                {
                    auto cc = m_extraCursors.at(i);
                    //Check the real position.
                    auto block = cc.block();
                    //Calculate the text position.
                    int blockEndPos = textPosition(block, expectEndPos, tabSpacing);
                    //Update actual end position.
                    actualEndPos = qMax(actualEndPos, blockEndPos);
                    //Update the position.
                    cc.setPosition(block.position() + blockEndPos, QTextCursor::KeepAnchor);
                    m_extraCursors.replace(i, cc);
                }
                //Update the end position.
                m_vEndSpacePos = actualEndPos;
                //Check whether the move could move to next block.
                auto cc = textCursor();
                if(cc.positionInBlock() < cc.block().length() - 1)
                {
                    //Move the cursor one row down or up.
                    event->setModifiers(Qt::NoModifier);
                    //Do the event.
                    QPlainTextEdit::keyPressEvent(event);
                }
            }
            //Update extra selections.
            updateExtraSelections();
            event->accept();
            return;
        }
        //Or else, cacnel the selection.
        clearExtraCursor();
        break;
    }
    case Qt::Key_Backspace:
        if(isExtraCursorEnabled())
        {
            if(m_verticalSelect)
            {
                QTextCursor tc = textCursor();
                tc.beginEditBlock();
                const int insBlock = tc.blockNumber();
                //Loop for all the cursors, if they reach the left most
                //position, stop delete.
                for(int i=0; i<m_extraCursors.size(); ++i)
                {
                    //Check whether the extra cursors works.
                    auto cc = m_extraCursors.at(i);
                    if(cc.positionInBlock() > 0)
                    {
                        cc.deletePreviousChar();
                        m_extraCursors.replace(i, cc);
                        if(cc.blockNumber() == insBlock)
                        {
                            //Update the vertical selection value.
                            m_vStartSpacePos = spacePosition(cc.block(), cc.positionInBlock(), knGlobal->tabSpacing());
                            m_vEndSpacePos = m_vStartSpacePos;
                        }
                    }
                }
                tc.endEditBlock();
                //Update extra selections.
                updateExtraSelections();
                event->accept();
                return;
            }
        }
        break;
    case Qt::Key_Delete:
        if(isExtraCursorEnabled())
        {
            if(m_verticalSelect)
            {
                QTextCursor tc = textCursor();
                tc.beginEditBlock();
                const int insBlock = tc.blockNumber();
                //Loop for all the cursors, if they reach the left most
                //position, stop delete.
                for(int i=0; i<m_extraCursors.size(); ++i)
                {
                    //Check whether the extra cursors works.
                    auto cc = m_extraCursors.at(i);
                    if(cc.positionInBlock() != cc.block().length() - 1)
                    {
                        cc.deleteChar();
                        m_extraCursors.replace(i, cc);
                        if(cc.blockNumber() == insBlock)
                        {
                            //Update the vertical selection value.
                            m_vStartSpacePos = spacePosition(cc.block(), cc.positionInBlock(), knGlobal->tabSpacing());
                            m_vEndSpacePos = m_vStartSpacePos;
                        }
                    }
                }
                tc.endEditBlock();
                //Update extra selections.
                updateExtraSelections();
                event->accept();
                return;
            }
        }
        break;
    default:
    {
        //Check column status.
        if(isExtraCursorEnabled())
        {
            //Check whether the key press event is selection.
            QString text = event->text();
            if(!text.isEmpty() && (text.at(0).isPrint()))
            {
                QTextCursor tc = textCursor();
                tc.beginEditBlock();
                //Check veritical selection is enabled.
                if(m_verticalSelect)
                {
                    //Loop and find the cursor number.
                    const int insBlock = tc.blockNumber(),
                            tabSpacing = knGlobal->tabSpacing();
                    int insTextPos = qMin(m_vStartSpacePos, m_vEndSpacePos),
                            currentId = -1;
                    //Insert the for each line.
                    for(int i=0; i<m_extraCursors.size(); ++i)
                    {
                        auto cc = m_extraCursors.at(i);
                        QString patchedText = text;
                        int cursorTextPos = cc.positionInBlock();
                        if(cc.hasSelection())
                        {
                            cursorTextPos = cc.selectionStart() - cc.block().position();
                        }
                        //Convert the cursor text pos into space position.
                        cursorTextPos = spacePosition(cc.block(), cursorTextPos, tabSpacing);
                        //Check the row selection.
                        if(cursorTextPos < insTextPos)
                        {
                            //Fill the space to the block
                            patchedText.prepend(QString(insTextPos - cursorTextPos, ' '));
                            cc.insertText(patchedText);
                            cc.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
                        }
                        else
                        {
                            //Check the text cursor position.
                            cc.insertText(patchedText);
                            cc.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, patchedText.size());
                        }
                        m_extraCursors.replace(i, cc);
                        //Check the cc position.
                        if(cc.blockNumber() == insBlock)
                        {
                            currentId = i;
                        }
                    }
                    //Check current id.
                    if(currentId != -1)
                    {
                        auto cc = m_extraCursors.at(currentId);
                        //Update the current position.
                        tc.setPosition(cc.position());
                        //Also update the vertical select parameter.
                        m_vStartSpacePos = spacePosition(cc.block(), cc.positionInBlock(), knGlobal->tabSpacing());
                        m_vEndSpacePos = m_vStartSpacePos;
                    }
                }
                else
                {
                    //Just insert the text and update the cursor.
                    ;
                }
                tc.endEditBlock();
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
    if(isExtraCursorEnabled())
    {
        //Check whether the column mates the current offset.
        emit copyAvailable(true);
    }
}

void KNTextEditor::mousePressEvent(QMouseEvent *event)
{
    //Check the mouse event.
    if(event->button() == Qt::LeftButton)
    {
        //Check the event modifier.
        if(event->modifiers() == Qt::NoModifier)
        {
            //Clear the cursor.
            clearExtraCursor();
        }
    }
    //Do the original event.
    QPlainTextEdit::mousePressEvent(event);
}

static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, const QRectF &gradientRect = QRectF())
{
    p->save();
    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
            m.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(m);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        p->setBrushOrigin(rect.topLeft());
    }
    p->fillRect(rect, brush);
    p->restore();
}

void KNTextEditor::paintEvent(QPaintEvent *event)
{
    Q_ASSERT(qobject_cast<QPlainTextDocumentLayout*>(document()->documentLayout()));
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
        painter.fillRect(0, cursorLine.y(),
                         contentsRect().width(), height,
                         m_currentLine.format.background());
    }

    //Do original paint event. Most of the following code is copy from the
    //original paintEvent for giant modification.
    QPointF offset(contentOffset());
    QRect er = event->rect();
    QRect viewportRect = viewport()->rect();
    QTextBlock block = firstVisibleBlock();
    qreal maximumWidth = document()->documentLayout()->documentSize().width();
    // Set a brush origin so that the WaveUnderline knows where the wave started
    painter.setBrushOrigin(offset);
    // keep right margin clean from full-width selection
    int maxX = offset.x() + qMax((qreal)viewportRect.width(), maximumWidth)
               - document()->documentMargin();
    er.setRight(qMin(er.right(), maxX));
    painter.setClipRect(er);
    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();
    painter.setPen(context.palette.text().color());
    while (block.isValid())
    {
        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();
        if (!block.isVisible())
        {
            offset.ry() += r.height();
            block = block.next();
            continue;
        }
        if (r.bottom() >= er.top() && r.top() <= er.bottom())
        {
            QTextBlockFormat blockFormat = block.blockFormat();
            QBrush bg = blockFormat.background();
            if (bg != Qt::NoBrush)
            {
                QRectF contentsRect = r;
                contentsRect.setWidth(qMax(r.width(), maximumWidth));
                fillBackground(&painter, contentsRect, bg);
            }
            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); ++i)
            {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if(selStart < bllen && selEnd > 0 && selEnd > selStart)
                {
                    QTextLayout::FormatRange o;
                    o.start = selStart;
                    o.length = selEnd - selStart;
                    o.format = range.format;
                    selections.append(o);
                }
                //Full width selection.
                else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
                           && block.contains(range.cursor.position()))
                {
                    // for full width selections we don't require an actual selection, just
                    // a position to specify the line. that's more convenience in usage.
                    QTextLayout::FormatRange o;
                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
                    o.start = l.textStart();
                    o.length = l.textLength();
                    if (o.start + o.length == bllen - 1)
                        ++o.length; // include newline
                    o.format = range.format;
                    selections.append(o);
                }
            }
            //Draw the text using block layout.
            layout->draw(&painter, offset, selections, er);
        }
        offset.ry() += r.height();
        if (offset.y() > viewportRect.height())
        {
            break;
        }
        block = block.next();
    }
    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().window());
    }

    //Draw the text cursor.
    int filter = (CursorDisplay | CursorVisible);
    if((m_editorOptions & filter) == filter)
    {
        //Draw the extra cursors.
        QVector<QTextCursor> cursors;
        cursors.append(textCursor());
        const QVector<QTextCursor> &renderList = isExtraCursorEnabled() ? m_extraCursors : cursors;
        for(int i=0; i<renderList.size(); ++i)
        {
            //Draw the current text cursor.
            QRect cr = cursorRect(renderList.at(i));
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
//        if(isExtraCursorEnabled())
//        {
//            auto startBlock = document()->findBlockByNumber(m_columnBlockNumber);
//            startCursor.setPosition(startBlock.position() + m_columnOffset);
//            //Get the rect positions.
//            QRect startRect = cursorRect(startCursor),
//                    endRect = cursorRect();
//            //Get the selection rect.
//            int top = qMin(startRect.top(), endRect.top()),
//                    bottom = qMax(startRect.bottom(), endRect.bottom());
//            //Draw the cursor.
//            painter.fillRect(QRect(endRect.x(), top, knUi->width(1), bottom - top),
//                             Qt::black);
//        }
//        else
//        {
//        }
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
    if(!isExtraCursorEnabled())
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
    //Update the tab stop distance ASAP.
    setTabStopDistance(knGlobal->tabSpacing() *
                       fontMetrics().averageCharWidth());
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
    viewport()->update();
}

void KNTextEditor::onAlignLeftChange(bool showLeft)
{
    //Update the default left and right position.
    auto option = document()->defaultTextOption();
    option.setAlignment(showLeft ? Qt::AlignLeft : Qt::AlignRight);
    document()->setDefaultTextOption(option);
}

void KNTextEditor::quickSearchUi(const QTextBlock &block)
{
    //Do quick search for the display part.
    QScopedPointer<KNTextSearcher> searcher;
    searcher.reset(new KNTextSearcher);
    searcher->search(block, m_quickSearchKeyword,
                     KNTextSearcher::SearchOption(height()/fontMetrics().lineSpacing()+2, false, true, m_quickSearchCode, m_quickSearchSense));
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
            #if QT_VERSION_MAJOR > 5
                    &KNTextSearcher::search, m_quickSearchNext.data(),
            #else
                    m_quickSearchNext.data(), &KNTextSearcher::search,
            #endif
                    firstVisibleBlock(), m_quickSearchKeyword,
                    KNTextSearcher::SearchOption(-1, true, true, m_quickSearchCode, m_quickSearchSense));
        m_futurePrev = QtConcurrent::run(
            #if QT_VERSION_MAJOR > 5
                    &KNTextSearcher::search, m_quickSearchPrev.data(),
            #else
                    m_quickSearchPrev.data(), &KNTextSearcher::search,
            #endif
                    firstVisibleBlock(), m_quickSearchKeyword,
                    KNTextSearcher::SearchOption(-1, true, false, m_quickSearchCode, m_quickSearchSense));
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

void KNTextEditor::insertTabAt(QTextCursor &tc, int tabSpacing)
{
    int insertPos;
    if(tc.hasSelection())
    {
        //Mark the insert position as selection start.
        insertPos = tc.selectionStart();
        //Clear the selection.
        tc.removeSelectedText();
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
        int tabPos = spacePosition(currentBlock, tc.positionInBlock(),
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

    if(!m_connections.isEmpty())
    {
        //No need to maintain extra selection in this case.
        //Column selections.
        if(isExtraCursorEnabled())
        {
            //Construct the format.
            QTextCharFormat format;
            format.setBackground(palette().brush(QPalette::Highlight));
            format.setForeground(palette().brush(QPalette::HighlightedText));
            //Construct the block start.
            for(auto cc : m_extraCursors)
            {
                //Construct the selection.
                QTextEdit::ExtraSelection selection;
                selection.cursor = cc;
                selection.format = format;
                selections.append(selection);
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
    }

    setExtraSelections(selections);
}

void KNTextEditor::updateHighlighter(KNSyntaxHighlighter *highlighter)
{
    //Check the previous highlighter.
    if(m_highlighter)
    {
        m_highlighter->setDocument(nullptr);
        //Delete the highlighter.
        m_highlighter->deleteLater();
        //Reset the pointer.
        m_highlighter = nullptr;
    }
    //Check whether the highlighter is set.
    if(highlighter)
    {
        m_highlighter = highlighter;
    }
    else
    {
        //Set the new highlighter.
        m_highlighter = KNCodeSyntaxHighlighter::get(m_filePath);
    }
    //Check whether the highlighter is null.
    if(m_highlighter)
    {
        //Update the panel based on the highlighter.
        m_panel->setShowFold(m_highlighter->hasCodeLevel());
        onBlockCountChanged(document()->blockCount());
        //Configure the highlighter.
        m_highlighter->setDocument(document());
        m_highlighter->rehighlight();
    }
    else
    {
        //Hide the code level.
        m_panel->setShowFold(false);
    }
}

int KNTextEditor::spacePosition(const QTextBlock &block, int textPos,
                                int tabSpacing)
{
    //Based on the position and the characters, calculate the current characters.
    int column = 0;
    QString &&blockText = block.text();
    for (int i=0; i<textPos; ++i)
    {
        //Check whether the iteration is \t.
        if(blockText.at(i) == '\t')
        {
            //Find the nearest variable.
            column = cellTabSpacing(column, tabSpacing);
            continue;
        }
        //Simply increase a char.
        column += charAsianWidth(blockText.at(i));
    }
    return column;
}

int KNTextEditor::textPosition(const QTextBlock &block, int spacePos,
                               int tabSpacing)
{
    //For empty checking.
    if(spacePos == 0)
    {
        return 0;
    }
    //Loop and reduce the space pos, until it down to 0, then that is the space
    //pos.
    QString &&blockText = block.text();
    for (int i=0; i<blockText.size(); ++i)
    {
        int charWidth = blockText.at(i) == '\t' ? tabSpacing : charAsianWidth(blockText.at(i));
        spacePos -= charWidth;
        if(spacePos <= 0)
        {
            return i + 1;
        }
    }
    //Or else, it should be the end of the string.
    return blockText.size();
}

QString KNTextEditor::textLevelString(int spaceLevel, int tabSpacing)
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
    //If we have extra cursors, we have to generate the column text.
    if(isExtraCursorEnabled())
    {
        return extraCursorText();
    }
    //Or just extract from the current text cursor.
    return textCursor().selectedText();
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

void KNTextEditor::moveCurrentBlockUp()
{
    auto tc = textCursor();
    //Ignore when there is only one line.
    if(blockCount() == 1 || tc.blockNumber() == 0)
    {
        return;
    }
    tc.beginEditBlock();
    auto doc = document();
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
    setTextCursor(tc);
}

void KNTextEditor::moveCurrentBlockDown()
{
    auto tc = textCursor();
    //Ignore when there is only one line.
    if(blockCount() == 1)
    {
        return;
    }
    auto doc = document();
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
    setTextCursor(tc);
}

void KNTextEditor::undo()
{
    //Clear the column selection.
    clearExtraCursor();
    //Do undo.
    QPlainTextEdit::undo();
}

void KNTextEditor::redo()
{
    //Clear the column selection.
    clearExtraCursor();
    //Do redo.
    QPlainTextEdit::redo();
}

void KNTextEditor::cut()
{
    //Check column selection.
    if(isExtraCursorEnabled())
    {
        //Do extra cursor copy.
        extraCursorCopy();
        QTextCursor tc = textCursor();
        tc.beginEditBlock();
        //Remove all these column.
        for(int i=0; i<m_extraCursors.size(); ++i)
        {
            auto cc = m_extraCursors.at(i);
            //Remove the selection.
            tc.setPosition(m_extraCursors.at(i).selectionStart());
            tc.setPosition(m_extraCursors.at(i).selectionEnd(), QTextCursor::KeepAnchor);
            tc.removeSelectedText();
            cc.setPosition(tc.position());
            m_extraCursors.replace(i, cc);
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
    if(isExtraCursorEnabled())
    {
        //Do column copy.
        extraCursorCopy();
        return;
    }
    //Do original copy.
    QPlainTextEdit::copy();
}

void KNTextEditor::paste()
{
    //Check column selection.
    if(isExtraCursorEnabled())
    {
        //Do extra cursor paste here.
        auto textLines = knGlobal->clipboardText().split('\n');
        //Check the line length.
        if(textLines.size() > m_extraCursors.size())
        {
            textLines = textLines.mid(0, textLines.size());
        }
        //Check whether the text is in vertical mode.
        if(m_verticalSelect)
        {
            //Check whether the text lines are all the same.
            int initLength = textLines.at(0).length();
            bool keepVerticalMode = true;
            for(int i=1; i<textLines.size(); ++i)
            {
                if(textLines.at(i).size() != initLength)
                {
                    keepVerticalMode = false;
                    break;
                }
            }
            //Check the vertical mode.
            if(!keepVerticalMode)
            {
                m_verticalSelect = false;
            }
        }
        //Sort the cursor.
        std::sort(m_extraCursors.begin(), m_extraCursors.end(),
                  [=](const QTextCursor &a, const QTextCursor &b)
        {
            //Check the block number.
            return a.blockNumber() < b.blockNumber();
        });
        //Now insert the content.
        int textLineNum = 0;
        for(int i=0; i<m_extraCursors.size(); ++i)
        {
            auto cc = m_extraCursors.at(i);
            //Insert the text to the cursor.
            cc.insertText(textLines.at(textLineNum));
            cc.setPosition(cc.selectionEnd());
            m_extraCursors.replace(i, cc);
            //Move the text line number to the next one.
            textLineNum = ((textLineNum + 1) == textLines.size()) ? 0 : (textLineNum + 1);
        }
        //Update the extra cursors.
        updateExtraSelections();
        return;
    }
    //Do original paste.
    QPlainTextEdit::paste();
}

void KNTextEditor::loadUseCodec(const QByteArray &codecName)
{
    //Load the file.
    loadFrom(m_filePath, codecName);
}

void KNTextEditor::loadUseCodec(QTextCodec *codec)
{
    //Load the file.
    loadFrom(m_filePath, codec);
}

bool KNTextEditor::loadToDocument(const QString &filePath,
                                  QTextCodec **codec,
                                  QTextDocument *document)
{
    //Read the file.
    QFile targetFile(filePath);
    if(!targetFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    //Use codec to decode the file.
    QByteArray &&fileContent = targetFile.readAll();
    targetFile.close();
    //Decode the file from the codec.
    QString fileText;
    if(!*codec)
    {
        *codec = QTextCodec::codecForLocale();
    }
    //Use the specific codec to load the file.
    fileText = (*codec)->toUnicode(fileContent);
    //Set the file text to document.
    document->setPlainText(fileText);
    return true;
}

void KNTextEditor::loadFrom(const QString &filePath, QTextCodec *codec)
{
    //Read the file.
    if(!loadToDocument(filePath, &codec, document()))
    {
        QMessageBox::information(
                    this,
                    tr("Open failed"),
                    tr("Please check if this file is opened in another program."));
        return;
    }
    //Update the text cursor.
    QTextCursor tc = textCursor();
    tc.setPosition(0);
    setTextCursor(tc);
    //Set the codec name.
    setCodecName(codec->name());
    //Set the file path.
    setFilePath(filePath);
    //Update the syntax highlighter.
    updateHighlighter();
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

void KNTextEditor::extraCursorCopy()
{
    //Set the column text to clipboard.
    knGlobal->copyText(extraCursorText());
}

QString KNTextEditor::extraCursorText() const
{
    QStringList columnText;
    columnText.reserve(m_extraCursors.size());
    for(auto cc : m_extraCursors)
    {
        columnText.append(cc.selectedText());
    }
    //Set the selection text.
    return columnText.join("\n");
}


void KNTextEditor::clearExtraCursor()
{
    if(!isExtraCursorEnabled())
    {
        return;
    }
    //Reset the vertical selection mode.
    m_verticalSelect = false;
    //Update the current cursor position.
    auto tc = textCursor();
    for(auto cc : m_extraCursors)
    {
        if(cc.blockNumber() == tc.blockNumber())
        {
            tc.setPosition(cc.position());
            break;
        }
    }
    //Clear the extra cursors.
    m_extraCursors.clear();
    //Update the text cursor.
    setTextCursor(tc);
    //Update the extra selections, which also update the editor.
    updateExtraSelections();
}

void KNTextEditor::enterVerticalMode()
{
    //Enter the extra cursor mode.
    m_verticalSelect = true;
    m_extraCursors.clear();
    int tabSpacing = knGlobal->tabSpacing();
    //Check whether the text cursor is already contains selection.
    auto currentCursor = textCursor();
    if(currentCursor.hasSelection())
    {
        //Check whether the text cursor is at start or the end of the selection.
        bool atStart = currentCursor.position() == currentCursor.selectionStart();
        //Calculate the vertical start position.
        QTextCursor refCursor = textCursor();
        refCursor.setPosition(atStart ? currentCursor.selectionEnd() : currentCursor.selectionStart());
        m_vStartSpacePos = spacePosition(refCursor.block(), refCursor.positionInBlock(), tabSpacing);
        //Calculate the current end position.
        refCursor.setPosition(atStart ? currentCursor.selectionStart() : currentCursor.selectionEnd());
        m_vEndSpacePos = spacePosition(refCursor.block(), refCursor.positionInBlock(), tabSpacing);
        auto doc = document();
        //Update all the block during the selection.
        refCursor.setPosition(currentCursor.selectionStart());
        int blockStart = refCursor.blockNumber();
        refCursor.setPosition(currentCursor.selectionEnd());
        int blockEnd = refCursor.blockNumber();
        for(int i = qMin(blockStart, blockEnd),
             iMax = qMax(blockStart, blockEnd); i <= iMax; ++i)
        {
            auto block = doc->findBlockByNumber(i);
            refCursor.setPosition(block.position() + textPosition(block, m_vEndSpacePos, tabSpacing));
            appendVerticalCursor(refCursor);
        }
        return;
    }
    //Set the vertical space position.
    m_vStartSpacePos = spacePosition(
                textCursor().block(), textCursor().positionInBlock(),
                knGlobal->tabSpacing());
    m_vEndSpacePos = m_vStartSpacePos;
    //Append the vertical text cursor.
    appendVerticalCursor(textCursor());
}

bool KNTextEditor::isExtraCursorEnabled() const
{
    return !m_extraCursors.isEmpty();
}

bool KNTextEditor::isVerticalEnabled() const
{
    return m_verticalSelect && isExtraCursorEnabled();
}

void KNTextEditor::appendVerticalCursor(QTextCursor cursor)
{
    //Search the cursor in the list.
    int cursorBlock = cursor.blockNumber();
    for(auto tc : m_extraCursors)
    {
        if(tc.blockNumber() == cursorBlock)
        {
            return;
        }
    }
    //Extract the block data.
    const QTextBlock &block = cursor.block();
    auto data = blockData(block);
    data->verticalTextPos =
            textPosition(block, m_vStartSpacePos, knGlobal->tabSpacing());
    //Select from the vertical text pos to the current position.
    int endPosition = cursor.position();
    cursor.setPosition(block.position() + data->verticalTextPos);
    cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
    cursor.setKeepPositionOnInsert(true);
    //Append the cursor.
    m_extraCursors.append(cursor);
    //Update extra selections.
    updateExtraSelections();
}

QString KNTextEditor::filePath() const
{
    return m_filePath;
}

QJsonObject KNTextEditor::sessionObject() const
{
    QJsonObject sess;
    sess.insert("VScroll", verticalScrollBar()->value());
    sess.insert("HScroll", horizontalScrollBar()->value());
    sess.insert("CursorStart", textCursor().selectionStart());
    sess.insert("CursorEnd", textCursor().selectionEnd());
    sess.insert("ReadOnly", isReadOnly());
    sess.insert("Path", filePath());
    return sess;
}

void KNTextEditor::loadSessionStates(const QJsonObject &sess)
{
    //Synchronize the data.
    int iBuf = sess.value("VScroll").toInt(-1);
    if(iBuf > 0)
    {
        verticalScrollBar()->setValue(iBuf);
    }
    iBuf = sess.value("HScroll").toInt(-1);
    if(iBuf > 0)
    {
        horizontalScrollBar()->setValue(iBuf);
    }
    //Select the cursor.
    auto tc = textCursor();
    tc.setPosition(sess.value("CursorStart").toInt());
    tc.setPosition(sess.value("CursorEnd").toInt(), QTextCursor::KeepAnchor);
    setTextCursor(tc);
    //Update the read only state.
    setReadOnly(sess.value("ReadOnly").toBool());
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
    return isExtraCursorEnabled() ?
                true :
                textCursor().hasSelection();
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
    int tabSpacing = knGlobal->tabSpacing();
    //Check the text cursor selection contains multiple lines.
    auto tc = textCursor();
    if(tc.hasSelection())
    {
        //Check whether we have multiple blocks in the selection.
        tc.setKeepPositionOnInsert(true);
        auto startBlock = document()->findBlock(tc.selectionStart()),
                endBlock = document()->findBlock(tc.selectionEnd());
        if(startBlock.blockNumber() != endBlock.blockNumber())
        {
            tc.beginEditBlock();
            //Indent all lines.
            for(int i=startBlock.blockNumber(); i<=endBlock.blockNumber(); ++i)
            {
                //Fetch the current block.
                auto b = document()->findBlockByNumber(i);
                //Calculate the space pos.
                int textStart = firstNonSpacePos(b),
                    spaceLevel = spacePosition(b, textStart, tabSpacing);
                //Insert the level text.
                tc.setPosition(b.position());
                tc.setPosition(b.position() + textStart,
                               QTextCursor::KeepAnchor);
                tc.removeSelectedText();
                tc.insertText(textLevelString(spaceLevel + tabSpacing,
                                               tabSpacing));
            }
            tc.endEditBlock();
            return;
        }
    }
    tc.beginEditBlock();
    //We have to insert a tab at the cursor.
    insertTabAt(tc, tabSpacing);
    tc.endEditBlock();
    //Update the cursor.
    setTextCursor(tc);
}

void KNTextEditor::unindentCursor()
{
    //Check the text cursor selection contains multiple lines.
    auto tc = textCursor();
    //Fetch parameters from global.
    int tabSpacing = knGlobal->tabSpacing();
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
                    spaceLevel = spacePosition(b, textStart, tabSpacing) -
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
                tc.insertText(textLevelString(spaceLevel, tabSpacing));
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
        int spaceLevel = spacePosition(currentBlock, textStart, tabSpacing);
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
        tc.insertText(textLevelString(spaceLevel, tabSpacing));
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
