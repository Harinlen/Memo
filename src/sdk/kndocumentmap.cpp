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
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "kndocumentmap.h"

KNDocumentMap::KNDocumentMap(QWidget *parent) :
    QWidget(parent),
    m_document(nullptr)
{
}

void KNDocumentMap::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    if(m_document)
    {
//        QAbstractTextDocumentLayout::PaintContext ctx;
//        ctx.clip = rect();
//        ctx.palette.setColor(QPalette::Text, QColor(0, 0, 0));
//        m_document->documentLayout()->draw(&painter, ctx);
        painter.setPen(QColor(255, 0, 0));
        m_document->drawContents(&painter);
    }
}

void KNDocumentMap::setDocument(QTextDocument *document)
{
    m_document = document;
    update();
}
