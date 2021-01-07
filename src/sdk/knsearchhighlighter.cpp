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

#include "knsearchhighlighter.h"

KNSearchHighlighter::KNSearchHighlighter(QObject *parent) :
    KNSyntaxHighlighter(parent)
{
    m_resultFormat.setBackground(QColor(232, 232, 255));
    m_resultFormat.setForeground(QColor(39, 39, 149));
    m_resultFormat.setFontWeight(QFont::Bold);
    m_resultFormat.setProperty(QTextFormat::FullWidthSelection, true);
}

bool KNSearchHighlighter::hasCodeLevel() const
{
    return true;
}

void KNSearchHighlighter::syntaxProcess(const QString &text,
                                        KNTextBlockData *data)
{
    //Set the block format.
    if(text.startsWith("    "))
    {
        data->level = 2;
        data->levelMargin = 0;
    }
    else if(text.startsWith("  "))
    {
        data->level = 1;
        data->levelMargin = 1;
    }
    else
    {
        data->level = 0;
        data->levelMargin = 1;
        //Change the text format.
        setFormat(0, text.length(), m_resultFormat);
    }
}
