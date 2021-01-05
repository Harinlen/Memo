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
#ifndef KNTEXTSEARCHER_H
#define KNTEXTSEARCHER_H

#include <QMutex>
#include <QTextBlock>

#include <QObject>

class KNTextBlockData;
/*!
 * \brief The KNTextSearcher class provides the searcher for a text editor.
 */
class KNTextSearcher : public QObject
{
    Q_OBJECT
public:
    explicit KNTextSearcher(QObject *parent = nullptr);

    /*!
     * \brief Check whether the search is the latest.
     * \param data The text search block data.
     * \param searchCode The latest search code.
     * \return If the search result is latest, then return true.
     */
    static bool isSearchLatest(KNTextBlockData *data,
                               const unsigned long long int &searchCode);

    /*!
     * \brief Search a specific block and updated the block data.
     * \param text The block text.
     * \param keywords The target keywords.
     * \param data The block data.
     * \param searchCode The search code.
     * \param cs Whether the search is case sensitive.
     * \param len The length of the keyword for quick search.
     */
    static void searchBlock(const QString &text, const QString &keywords,
                            KNTextBlockData *data,
                            const unsigned long long int &searchCode,
                            Qt::CaseSensitivity cs, const int len);

signals:

public slots:
    /*!
     * \brief Search the entire document for the text.
     * \param block The text start block.
     * \param keywords The keywords to search.
     * \param lineCount The line count for search.
     * \param untilEnd Whether the search should be checked to the end.
     * \param forward Whether the searcher is search in backward direction.
     * \param searchCode The search code mark.
     * \param cs The search flag for case sensitive.
     */
    void search(QTextBlock block, const QString &keywords,
                int lineCount, bool untilEnd, bool forward,
                const unsigned long long int &searchCode,
                Qt::CaseSensitivity cs);

    /*!
     * \brief Quit the search progress.
     */
    void quit();

private:
    QMutex m_lock;
    bool m_quit;
};

#endif // KNTEXTSEARCHER_H
