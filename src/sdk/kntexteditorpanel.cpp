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

#define MARK_WIDTH (18)
#define FOLD_WIDTH (14)

KNTextEditorPanel::KNTextEditorPanel(KNTextEditor *parent) : QWidget(parent),
    m_lineNumberWidth(0)
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
    //Update the line number area.
    KNTextEditor *editor = static_cast<KNTextEditor *>(parentWidget());
    //Paint the current line.
    editor->paintCurrentLine(&painter, width());
    //Paint the line number part.
    editor->paintSidebar(&painter, m_lineNumberWidth, knUi->width(MARK_WIDTH));
}
