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

class KNTextEditor;
/*!
 * \brief The KNFindEngine class provides the core search function at the
 * document level. It should works in an independent thread to complete the
 * function.
 */
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

    /*!
     * \brief Construct a KNFindEngine object.
     * \param parent The parent object.
     */
    explicit KNFindEngine(QObject *parent = nullptr);

    /*!
     * \brief Execute search in a document once with a prepared search cache.
     * \param doc The document to search.
     * \param tc The text cursor to start searching.
     * \param cache The prepared search cache.
     * \param flags The search flag.
     * \return The target text cursor with the text selected.
     */
    static QTextCursor cacheSearch(QTextDocument *doc, const QTextCursor &tc,
            const SearchCache &cache,
            QTextDocument::FindFlags flags);

    /*!
     * \brief Set the document search cache.
     * \param cache The cache of the document.
     * \param flags The search flag.
     */
    void setSearchCache(const SearchCache &cache,
                        QTextDocument::FindFlags flags);

    /*!
     * \brief The search result of the mission.
     * \return The search result instance.
     */
    KNSearchResult::SearchResult result() const;

    /*!
     * \brief Set the search area to be a set of the document of text editors.
     * \param editors The editor pointer list.
     */
    void setSearchEditors(QVector<KNTextEditor *> editors);

    /*!
     * \brief Set the search area to be a specific file or directory.
     * \param filter The local file search filter.
     */
    void setSearchFilters(QString filter);

    /*!
     * \brief Stop the current search progress.
     */
    void stopSearch();

signals:
    /*!
     * \brief When the search progress is break in the middle, this signal is
     * emitted.
     */
    void searchBreak();

    /*!
     * \brief When the search is completed, this signal is emitted.
     */
    void searchComplete();

    /*!
     * \brief When the search area is set, this signal is emitted.
     * \param count The total file or document to be searched.
     */
    void searchCountChange(int count);

    /*!
     * \brief During the searching, this signal is emitted to display the search
     * progress.
     * \param i The current search id.
     * \param filePath The current searching file path.
     */
    void searching(int i, QString filePath);

public slots:
    /*!
     * \brief Start the search progress.
     */
    void start();

private:
    inline int taskCounts() const;
    inline QTextDocument *taskAt(int taskId);
    inline void taskFree();
    QMutex m_quitLock;
    QVector<QTextDocument *> m_documents;
    QStringList m_files;
    QTextDocument *m_fileDocumentBuf;
    bool m_quit, m_useDocument;
    SearchCache m_cache;
    KNSearchResult::SearchResult m_result;
    quint64 m_counter;
    QTextDocument::FindFlags m_flags;
};

#endif // KNFINDENGINE_H
