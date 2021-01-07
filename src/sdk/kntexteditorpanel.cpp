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
#include <QFontDatabase>
#include <QFontMetrics>
#include <QPainter>

#include "knuimanager.h"
#include "kntexteditor.h"

#include "kntexteditorpanel.h"

KNTextEditorPanel::KNTextEditorPanel(KNTextEditor *parent) : QWidget(parent),
    m_lineNumberWidth(0),
    m_showMarks(true),
    m_showFold(true)
{
}

void KNTextEditorPanel::setLineNumberWidth(int width)
{
    m_lineNumberWidth = width;
}

void KNTextEditorPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    //Create the painter for the panel.
    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing);
    //Fill the panel.
    painter.fillRect(rect(), QColor(228, 228, 228));
    //Update the line number area.
    KNTextEditor *editor = static_cast<KNTextEditor *>(parentWidget());
    //Paint the line number part.
    editor->paintSidebar(&painter, m_lineNumberWidth,
                         m_showMarks ? knUi->width(MARK_WIDTH) : 0,
                         m_showFold ? knUi->width(FOLD_WIDTH) : 0);
}

bool KNTextEditorPanel::showFold() const
{
    return m_showFold;
}

void KNTextEditorPanel::setShowFold(bool showFold)
{
    m_showFold = showFold;
}

bool KNTextEditorPanel::showMarks() const
{
    return m_showMarks;
}

void KNTextEditorPanel::setShowMarks(bool showMarks)
{
    m_showMarks = showMarks;
}

int KNTextEditorPanel::panelBaseWidth() const
{
    //Check the show mark flag.
    int baseWidth = 0;
    if(m_showMarks)
    {
        baseWidth += knUi->width(MARK_WIDTH);
    }
    if(m_showFold)
    {
        baseWidth += knUi->width(FOLD_WIDTH);
    }
    return baseWidth;
}
