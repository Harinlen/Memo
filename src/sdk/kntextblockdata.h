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
#ifndef KNTEXTBLOCKDATA_H
#define KNTEXTBLOCKDATA_H

#include <QMutex>
#include <QTextBlockUserData>

/*!
 * \brief The KNTextBlockData class provides all the data for the KNTextEdit to
 * use and support to do search, code folding and bookmarks.
 */
class KNTextBlockData : public QTextBlockUserData
{
public:
    KNTextBlockData() { }

    struct MarkBlock
    {
        int pos;
        int length;
        quint8 style;
        MarkBlock(int p, int l, quint8 s) :
            pos(p), length(l), style(s) { }
    };

    struct SearchMarks
    {
        int pos;
        int length;
        SearchMarks(int p, int l) :
            pos(p), length(l) { }
    };
    //Bookmarks.
    bool hasBookmark = false;
    //Mark flags.
    QVector<MarkBlock> marks;
    //Quick search data.
    QVector<SearchMarks> results;
    unsigned long long int searchCode;
    bool resultExpire;

    void lockQuickSearch() { lock.lock(); }
    void unlockQuickSearch() { lock.unlock(); }

    void onBlockChanged() { }

private:
    QMutex lock;
};

#endif // KNTEXTBLOCKDATA_H
