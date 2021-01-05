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
#include "kntextblockdata.h"

#include "kntextsearcher.h"

KNTextSearcher::KNTextSearcher(QObject *parent) : QObject(parent),
    m_quit(false)
{
}

bool KNTextSearcher::isSearchLatest(KNTextBlockData *data,
                                    const unsigned long long &searchCode)
{
    return data->searchCode == searchCode && !data->resultExpire;
}

void KNTextSearcher::searchBlock(
        const QString &text, const QString &keywords,
        KNTextBlockData *data,
        const unsigned long long int &searchCode,
        Qt::CaseSensitivity cs, const int len)
{
    data->lockQuickSearch();
    //Check the search code and result expire.
    if(isSearchLatest(data, searchCode))
    {
        data->unlockQuickSearch();
        return;
    }
    //Set the search code and do the search.
    data->searchCode = searchCode;
    data->resultExpire = false;
    //Clear the search the result.
    data->results = QVector<KNTextBlockData::SearchMarks>();
    //Search the block.
    int pos = text.indexOf(keywords, 0, cs);
    while(pos != -1)
    {
        //Append the result.
        data->results.append(KNTextBlockData::SearchMarks(pos, len));
        //Search for the next.
        pos = text.indexOf(keywords, pos + len, cs);
    }
    data->unlockQuickSearch();
}



void KNTextSearcher::search(QTextBlock block, const QString &keywords,
                            int lineCount, bool untilEnd, bool forward,
                            const unsigned long long int &searchCode,
                            Qt::CaseSensitivity cs)
{
    const int len = keywords.length();
    //Loop and check every thing.
    for(; block.isValid() && (untilEnd || lineCount >= -1);
        block = forward ? block.next() : block.previous())
    {
        //Check the quit status.
        m_lock.lock();
        if(m_quit)
        {
            //Shut down the search.
            m_lock.unlock();
            return;
        }
        m_lock.unlock();
        //Update the line count.
        lineCount -= untilEnd ? 0 : block.lineCount();
        KNTextBlockData *data = static_cast<KNTextBlockData *>(block.userData());
        if(Q_UNLIKELY(data == nullptr))
        {
            continue;
        }
        //Does search for the string.
        searchBlock(block.text(), keywords, data, searchCode, cs, len);
    }
}

void KNTextSearcher::quit()
{
    //Set the quit flag.
    m_lock.lock();
    m_quit = true;
    m_lock.unlock();
}
