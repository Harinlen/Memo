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
    struct SyntaxExt
    {
        const QString name;
        const int priority;
        SyntaxExt(const QString &n, int p) :
            name(n), priority(p) {}
    };

    explicit KNCodeSyntaxHighlighter(const QString &syntaxName,
                                     QObject *parent = nullptr);

    /*!
     * \brief Get the highlighter based on the suffix.
     * \param filePath The file path for guessing the highlighter.
     * \return The syntax highlighter.
     */
    static KNSyntaxHighlighter *get(const QString &filePath);

    void loadRules(const QString &syntaxName);

    bool hasCodeLevel() const override;

signals:

private:
    static QHash<QString, QString> extMap;
    static QHash<QString, QString> namePathMap;
    static bool syntaxLoaded;
    static void loadDir(const QString &dirPath, QHash<QString, int> &priorityMap);
    static void loadXml(const QString &xmlPath, QHash<QString, int> &priorityMap);
};

#endif // KNCODESYNTAXHIGHLIGHTER_H
