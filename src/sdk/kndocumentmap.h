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
#ifndef KNDOCUMENTMAP_H
#define KNDOCUMENTMAP_H

#include <QWidget>

class QTextDocument;
class KNDocumentMap : public QWidget
{
    Q_OBJECT
public:
    explicit KNDocumentMap(QWidget *widget = nullptr);

    void setDocument(QTextDocument *document);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTextDocument *m_document;
};

#endif // KNDOCUMENTMAP_H
