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

KNFindEngine::KNFindEngine(QObject *parent) : QObject(parent)
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

QStringList extractFilePaths(const QString &filter)
{
    return QStringList();
}

void KNFindEngine::start(const QString &filter)
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
    //Construct the codec and document.
    QTextCodec *codec;
    QTextDocument *document = new QTextDocument();
    //Construct the file paths.
    QStringList filePaths;
    QFileInfo targetInfo(filter);
    //Set the target info to the file paths.
    if(targetInfo.isFile())
    {
        filePaths.append(targetInfo.absoluteFilePath());
    }
    else
    {
        //Check whether it has filter.
        filePaths = extractFilePaths(filter);
    }
    //Start search in the file.
    for(int i=0; i<filePaths.size(); ++i)
    {
        m_quitLock.lock();
        if(m_quit)
        {
            m_quitLock.unlock();
            //Clear the document.
            delete document;
            //Break.
            emit searchBreak();
            return;
        }
        m_quitLock.unlock();
        //Emit the signal.
        const QString &filePath = filePaths.at(i);
        emit searching(i, filePath);
        //Clear the codec.
        codec = nullptr;
        //Load the file to the document.
        if(!KNTextEditor::loadToDocument(filePath, &codec, document))
        {
            continue;
        }
        //Check the file result.
        KNSearchResult::FileResult fileResult;
        //Construct a text cursor of the document.
        QTextCursor tc(document);
        tc.movePosition(QTextCursor::Start);
        //Do the first search in the document.
        tc = cacheSearch(document, tc, m_cache, m_flags);
        //Change the text cursor.
        while(!tc.isNull())
        {
            //Construct the result item.
            KNSearchResult::ItemResult itemResult;
            //Append the result.
            itemResult.pos = tc.selectionStart();
            itemResult.length = tc.selectionEnd() - tc.selectionStart();
            fileResult.items.append(itemResult);
            //Check quit request.
            m_quitLock.lock();
            if(m_quit)
            {
                m_quitLock.unlock();
                //Clear the document.
                delete document;
                //Break.
                emit searchBreak();
                return;
            }
            m_quitLock.unlock();
            //Keep searching.
            tc = cacheSearch(document, tc, m_cache, m_flags);
        }
        //Append the item result to final result.
        fileResult.path = filePath;
        m_result.results.append(fileResult);
        //Increase the counter.
        ++m_counter;
    }
    delete document;
    //Emit the searching complete.
    emit searchComplete();
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
