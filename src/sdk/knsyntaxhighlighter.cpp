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
#include "kntextblockdata.h"

#include "knsyntaxhighlighter.h"

KNSyntaxHighlighter::KNSyntaxHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

KNSyntaxHighlighter *KNSyntaxHighlighter::get(const QString &filePath)
{
    //Based on the language suffix.
    return new KNSyntaxHighlighter();
}

void KNSyntaxHighlighter::highlightBlock(const QString &text)
{
    //Create the user data for the text block.
    auto blockData = static_cast<KNTextBlockData *>(currentBlockUserData());
    if(!blockData)
    {
        //Create and set the block data.
        blockData = new KNTextBlockData;
        setCurrentBlockUserData(blockData);
    }
}
