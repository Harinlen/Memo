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
#include <QTextBlock>

#include "kndocumentlayout.h"

KNDocumentLayout::KNDocumentLayout(QTextDocument *document) :
    QPlainTextDocumentLayout(document)
{
}

void KNDocumentLayout::documentChanged(int from, int charsRemoved, int charsAdded)
{
    //Broadcast this layout.
    //Do original document changed
    QPlainTextDocumentLayout::documentChanged(from, charsRemoved, charsAdded);
}
