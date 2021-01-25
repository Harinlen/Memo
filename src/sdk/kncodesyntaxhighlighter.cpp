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
#include <QFileInfo>

#include "kncodesyntaxhighlighter.h"

bool KNCodeSyntaxHighlighter::syntaxLoaded = false;
QHash<QString, QString> KNCodeSyntaxHighlighter::syntaxNames = QHash<QString, QString>();

KNCodeSyntaxHighlighter::KNCodeSyntaxHighlighter(QObject *parent) :
    KNSyntaxHighlighter(parent)
{

}

KNSyntaxHighlighter *KNCodeSyntaxHighlighter::get(const QString &filePath)
{
    //Check whether the global syntax is loaded.
    if(!syntaxLoaded)
    {
        //Load the information from the rows.
        ;
        //Load the syntax.
        syntaxLoaded = true;
    }
    //Extract the suffix of the file path.
    QFileInfo fileInfo(filePath);
    // Find the suffix in the global rules.
    ;
    //Or else, we have to use a default syntax highlighter, which does nothing.
    return new KNSyntaxHighlighter();
}

bool KNCodeSyntaxHighlighter::hasCodeLevel() const
{
    return true;
}
