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
#ifndef KNCODESYNTAXHIGHLIGHTER_H
#define KNCODESYNTAXHIGHLIGHTER_H

#include "knsyntaxhighlighter.h"

class KNCodeSyntaxHighlighter : public KNSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit KNCodeSyntaxHighlighter(QObject *parent = nullptr);

    /*!
     * \brief Get the highlighter based on the suffix.
     * \param filePath The file path for guessing the highlighter.
     * \return The syntax highlighter.
     */
    static KNSyntaxHighlighter *get(const QString &filePath);

    bool hasCodeLevel() const override;

signals:

private:
    static QHash<QString, QString> syntaxNames;
    static bool syntaxLoaded;

};

#endif // KNCODESYNTAXHIGHLIGHTER_H
