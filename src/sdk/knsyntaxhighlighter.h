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
#ifndef KNSYNTAXHIGHLIGHTER_H
#define KNSYNTAXHIGHLIGHTER_H

#include <QHash>

#include <QSyntaxHighlighter>

class KNTextEdit;
/*!
 * \brief The KNSyntaxHighlighter class is the default and base syntax
 * highlighter implementation, which manages the format of the file.
 */
class KNSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNSyntaxHighlighter object.
     * \param parent The parent object.
     */
    explicit KNSyntaxHighlighter(QObject *parent = nullptr);

    /*!
     * \brief Get the highlighter based on the suffix.
     * \param filePath The file path for guessing the highlighter.
     * \return The syntax highlighter.
     */
    static KNSyntaxHighlighter *get(const QString &filePath);

protected:
    /*!
     * \brief Reimplemented from QSyntaxHighlighter::highlightBlock().
     */
    void highlightBlock(const QString &text) override;

private:
};

#endif // KNSYNTAXHIGHLIGHTER_H
