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
#ifndef KNDOCUMENTLAYOUT_H
#define KNDOCUMENTLAYOUT_H

#include <QPlainTextDocumentLayout>

class KNDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
public:
    explicit KNDocumentLayout(QTextDocument *document);

signals:

protected:
    void documentChanged(int from, int charsRemoved, int charsAdded) override;

};

#endif // KNDOCUMENTLAYOUT_H
