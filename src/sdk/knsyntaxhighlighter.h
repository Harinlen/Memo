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
class KNTextBlockData;
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

    /*!
     * \brief Check whether the highlighter supports for code level.
     * \return The code level.
     */
    virtual bool hasCodeLevel() const;

protected:
    /*!
     * \brief Reimplemented from QSyntaxHighlighter::highlightBlock().
     */
    void highlightBlock(const QString &text) override;

    /*!
     * \brief Process the syntax related information of the text.
     * \param text The text of the block.
     * \param data The data pointer of the current block.
     */
    virtual void syntaxProcess(const QString &text, KNTextBlockData *data);

private:
};

#endif // KNSYNTAXHIGHLIGHTER_H
