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
#ifndef KNFINDENGINE_H
#define KNFINDENGINE_H

#include <QRegularExpression>
#include <QTextCursor>
#include <QTextDocument>
#include <QMutex>

#include "knsearchresult.h"

#include <QObject>

class KNFindEngine : public QObject
{
    Q_OBJECT
public:
    struct SearchCache
    {
        bool useReg;
        bool multiLine;
        QString keywords;
        QString rawKeyword;
        QString keywordLineFirst, keywordLineLast;
        QStringList keywordLineMid;
        QRegularExpression regExp;
        QRegularExpression regExpFirst, regExpLast;
        QVector<QRegularExpression> regExpLines;
    };

    explicit KNFindEngine(QObject *parent = nullptr);

    static QTextCursor cacheSearch(QTextDocument *doc, const QTextCursor &tc,
            const SearchCache &cache,
            QTextDocument::FindFlags flags);


    void setSearchCache(const SearchCache &cache,
                        QTextDocument::FindFlags flags);

    KNSearchResult::SearchResult result() const;

signals:
    void searchBreak();
    void searchComplete();
    void searching(int i, QString filePath);

public slots:
    void start(const QString &filter);

private:
    QMutex m_quitLock;
    bool m_quit;
    SearchCache m_cache;
    KNSearchResult::SearchResult m_result;
    quint64 m_counter;
    QTextDocument::FindFlags m_flags;
};

#endif // KNFINDENGINE_H
