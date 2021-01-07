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
#include "knuimanager.h"
#include "kntexteditor.h"
#include "knsearchhighlighter.h"

#include "knsearchresult.h"

KNSearchResult::KNSearchResult(QWidget *parent) :
    QDockWidget(parent),
    m_editor(new KNTextEditor(QString(), QString(), QString(), this,
                              new KNSearchHighlighter(this)))
{
    //Configure the editor.
    m_editor->setReadOnly(true);
    m_editor->setCursorVisible(false);
    m_editor->setLineNumberVisible(false);
    m_editor->setBookmarkVisible(false);
    m_editor->setHighlightCursor(false);
    setWidget(m_editor);
    //Link the translator.
    knUi->addTranslate(this, &KNSearchResult::retranslate);
    //Add one result.
    addResult();
}

void KNSearchResult::addResult()
{
    //Append the title of the text.
    m_editor->appendPlainText(
                "Search 'bool' (5 hits in 1 file of 1 searched)\n"
                "  C:/Users/a.txt (5 hits)\n"
                "    Line 1: asdfasdf\n"
                "    Line 2: qowieuroqiewu\n");
}

void KNSearchResult::retranslate()
{
    setWindowTitle(tr("Find result"));
}
