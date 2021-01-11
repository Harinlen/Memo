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
#ifndef KNSEARCHRESULT_H
#define KNSEARCHRESULT_H

#include <QTextEdit>
#include <QDockWidget>

class KNTextEditor;
/*!
 * \brief The KNSearchResult class provides a special editor for displaying the
 * search result.
 */
class KNSearchResult : public QDockWidget
{
    Q_OBJECT
public:
    struct ItemResult
    {
        QString slice;
        int row;
        int posInRow;
        int length;
    };

    struct FileResult
    {
        QString path;
        QVector<ItemResult> items;
    };

    struct SearchResult
    {
        QString keyword;
        QVector<FileResult> results;
    };

    /*!
     * \brief Construct a KNSearchResult widget.
     * \param parent The parent widget.
     */
    explicit KNSearchResult(QWidget *parent = nullptr);

signals:

public slots:
    void addResult(const SearchResult &result);

private slots:
    void retranslate();

private:
    KNTextEditor *m_editor;
    QList<QTextEdit::ExtraSelection> m_selections;
    QTextCharFormat m_resultFormat;
};

#endif // KNSEARCHRESULT_H
