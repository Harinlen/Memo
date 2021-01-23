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
#include <QTextBlock>
#include <QTextCodec>
#include <QFileInfo>

#include "kntexteditor.h"

#include "knfindengine.h"

KNFindEngine::KNFindEngine(QObject *parent) : QObject(parent),
    m_fileDocumentBuf(nullptr),
    m_quit(false),
    m_useDocument(false)
{
}

QTextBlock matchLines(QTextBlock block, const QStringList &lines,
                      Qt::CaseSensitivity cs)
{
    int lineId = 0;
    while(lineId < lines.size())
    {
        //Check block validation and text matched.
        if(!block.isValid() || !block.text().compare(lines.at(lineId), cs))
        {
            return QTextBlock();
        }
        //Switch to next block and line.
        block = block.next();
        ++lineId;
    }
    return block;
}

int regMatchFirst(const QString &source, const QRegularExpression &exp)
{
    //Match the exp from the end of the block.
    QRegularExpressionMatch match;
    int result = source.indexOf(exp, 0, &match);
    if(result == -1)
    {
        return -1;
    }
    //Check the result.
    return (match.capturedStart() == 0) ?
              match.capturedEnd() : -1;
}

int regMatchLast(const QString &source, const QRegularExpression &exp)
{
    //Match the exp from the end of the block.
    QRegularExpressionMatch match;
    int result = source.lastIndexOf(exp, -1, &match);
    if(result == -1)
    {
        return -1;
    }
    //Check the result.
    return (match.capturedEnd() == source.length()) ?
              match.capturedStart() : -1;
}

QTextBlock regMatchLines(QTextBlock block, const QVector<QRegularExpression> &exps)
{
    int lineId = 0;
    while(lineId < exps.size())
    {
        //Check block validation and exp matched.
        if(!block.isValid())
        {
            return QTextBlock();
        }
        //Use the regular expression to match the string.
        const QString &blockText = block.text();
        auto match = exps.at(lineId).match(blockText);
        //Must match the entire string.
        if(!match.isValid() || match.capturedStart() != 0 ||
                match.capturedEnd() != blockText.length())
        {
            return QTextBlock();
        }
        //Keep moving.
        block = block.next();
        ++lineId;
    }
    return block;
}

QTextCursor KNFindEngine::cacheSearch(QTextDocument *doc,
                                      const QTextCursor &tc,
                                      const KNFindEngine::SearchCache &cache,
                                      QTextDocument::FindFlags flags)
{
    //Check the cache regular expression settings.
    if(cache.useReg)
    {
        if(cache.multiLine)
        {
            //Search for the first line.
            QTextBlock block = tc.block();
            while(block.isValid())
            {
                int firstMatch = regMatchLast(block.text(), cache.regExpFirst);
                if(firstMatch != -1)
                {
                    //Perform the mid-level search.
                    QTextBlock lastBlock = regMatchLines(block.next(), cache.regExpLines);
                    if(lastBlock.isValid())
                    {
                        // Match the last line.
                        int lastMatch = regMatchFirst(lastBlock.text(), cache.regExpLast);
                        if(lastMatch != -1)
                        {
                            //Now construct the position.
                            auto resultCursor = tc;
                            resultCursor.setPosition(block.position() + firstMatch);
                            resultCursor.setPosition(lastBlock.position() + lastMatch,
                                                     QTextCursor::KeepAnchor);
                            return resultCursor;
                        }
                    }
                }
                block = block.next();
            }
            return QTextCursor();
        }
        return doc->find(cache.regExp, tc, flags);
    }
    //Perform normal search.
    if(cache.multiLine)
    {
        auto cs = (flags & QTextDocument::FindCaseSensitively) ?
                    Qt::CaseSensitive : Qt::CaseInsensitive;
        //Search for the first line.
        QTextBlock block = tc.block();
        while(block.isValid())
        {
            QString text = block.text();
            //Search at the end of the block.
            if(text.endsWith(cache.keywordLineFirst, cs))
            {
                //Matches mid-level lines.
                QTextBlock lastBlock = matchLines(block.next(), cache.keywordLineMid, cs);
                //Check the mid block result and matches the last block.
                if(lastBlock.isValid() &&
                        lastBlock.text().startsWith(cache.keywordLineLast, cs))
                {
                    //Now construct the position.
                    auto resultCursor = tc;
                    resultCursor.setPosition(block.position() +
                                             text.length() -
                                             cache.keywordLineFirst.length());
                    //Set the selection.
                    resultCursor.movePosition(QTextCursor::NextCharacter,
                                              QTextCursor::KeepAnchor,
                                              cache.keywords.length());
                    return resultCursor;
                }
            }
            block = block.next();
        }
        //If goes here, then no need to find.
        return QTextCursor();
    }
    //Do normal single line search.
    return doc->find(cache.keywords, tc, flags);
}

void KNFindEngine::start()
{
    //Configure the quit variable.
    m_quitLock.lock();
    m_quit = false;
    m_quitLock.unlock();
    //Reset the result.
    m_result = KNSearchResult::SearchResult();
    m_counter = 0;
    //Set the result keyword.
    m_result.keyword = m_cache.keywords;
    //Start search in the files.
    for(int i=0; i<taskCounts(); ++i)
    {
        m_quitLock.lock();
        if(m_quit)
        {
            m_quitLock.unlock();
            //Clear the document.
            taskFree();
            //Break.
            emit searchBreak();
            return;
        }
        m_quitLock.unlock();
        //Emit the signal.
        emit searching(i, m_files.at(i));
        //Fetch the document.
        auto document = taskAt(i);
        if(!document)
        {
            continue;
        }
        //Check the file result.
        KNSearchResult::FileResult fileResult;
        //Construct a text cursor of the document.
        QTextCursor tc(document), posDetect(document);
        tc.movePosition(QTextCursor::Start);
        //Do the first search in the document.
        tc = cacheSearch(document, tc, m_cache, m_flags);
        //Change the text cursor.
        while(!tc.isNull())
        {
            //Construct the result item.
            KNSearchResult::ItemResult itemResult;
            //Append the result.
            itemResult.length = tc.selectionEnd() - tc.selectionStart();
            posDetect.setPosition(tc.selectionStart());
            itemResult.row = posDetect.blockNumber();
            itemResult.posStart = posDetect.positionInBlock();
            QString slice = tc.block().text();
            slice = slice.left(itemResult.posStart + 50);
            int expectEnd = qMin(itemResult.posStart + itemResult.length, slice.length()),
                    expectStart = itemResult.posStart;
            if(slice.length() > 100)
            {
                int offset = slice.length() - 100;
                expectStart -= offset;
                expectEnd -= offset;
                slice = slice.right(100);
            }
            itemResult.sliceStart = expectStart;
            itemResult.sliceEnd = expectEnd;
            itemResult.slice = slice;
            fileResult.items.append(itemResult);
            //Check quit request.
            m_quitLock.lock();
            if(m_quit)
            {
                m_quitLock.unlock();
                //Clear the document.
                taskFree();
                //Break.
                emit searchBreak();
                return;
            }
            m_quitLock.unlock();
            //Keep searching.
            tc = cacheSearch(document, tc, m_cache, m_flags);
        }
        //Append the item result to final result.
        fileResult.path = m_files.at(i);
        m_result.results.append(fileResult);
        //Increase the counter.
        ++m_counter;
    }
    taskFree();
    //Emit the searching complete.
    emit searchComplete();
}

inline int KNFindEngine::taskCounts() const
{
    return m_useDocument ? m_documents.size() : m_files.size();
}

QTextDocument *KNFindEngine::taskAt(int taskId)
{
    //If using document mode, directly return the document.
    if(m_useDocument)
    {
        return m_documents.at(taskId);
    }
    //Or else we have to construct the document.
    if(!m_fileDocumentBuf)
    {
        //Create a new text document buffer for document.
        m_fileDocumentBuf = new QTextDocument();
    }
    //Load the file to the buffer.
    QTextCodec *codec;
    if(KNTextEditor::loadToDocument(m_files.at(taskId), &codec,
                                    m_fileDocumentBuf))
    {
        return m_fileDocumentBuf;
    }
    //Or else, just ignore the buffer.
    return nullptr;
}

void KNFindEngine::taskFree()
{
    //Only have to free when using file mode.
    if(!m_useDocument)
    {
        //Free the document buf.
        delete m_fileDocumentBuf;
    }
}

void KNFindEngine::setSearchCache(const KNFindEngine::SearchCache &cache,
                                  QTextDocument::FindFlags flags)
{
    m_cache = cache;
    m_flags = flags;
}

KNSearchResult::SearchResult KNFindEngine::result() const
{
    return m_result;
}

void KNFindEngine::setSearchEditors(QVector<KNTextEditor *> editors)
{
    m_documents.clear();
    m_files.clear();
    //Save the documents.
    for(auto editor : editors)
    {
        m_documents.append(editor->document());
        //Check whether the editor is on disk.
        m_files.append(editor->isOnDisk() ? editor->filePath() :
                                            editor->documentTitle());
    }
    //Emit the signals.
    emit searchCountChange(m_files.size());
    //Configure the document file.
    m_useDocument = true;
}

void KNFindEngine::setSearchFilters(QString filter)
{
    //Construct the filter files.
    QFileInfo targetInfo(filter);
    //Set the target info to the file paths.
    if(targetInfo.isFile())
    {
        m_files.append(targetInfo.absoluteFilePath());
    }
    else
    {
        //Check whether it has filter.
        //!FIXME: Add codes here.
        m_files = QStringList();
    }
    //Emit the signals.
    emit searchCountChange(m_files.size());
    //Disable use document mode.
    m_useDocument = false;
}

void KNFindEngine::stopSearch()
{
    //Set the quit flag.
    m_quitLock.lock();
    m_quit = true;
    m_quitLock.unlock();
}
