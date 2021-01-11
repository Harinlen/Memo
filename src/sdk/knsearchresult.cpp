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
#include "knglobal.h"

#include "knsearchresult.h"

KNSearchResult::KNSearchResult(QWidget *parent) :
    QDockWidget(parent),
    m_editor(new KNTextEditor(QString(), QString(), QString(), this,
                              new KNSearchHighlighter(this), false))
{
    //Configure the editor.
    m_editor->setReadOnly(true);
    m_editor->setCursorVisible(false);
    m_editor->setLineNumberVisible(false);
    m_editor->setBookmarkVisible(false);
    m_editor->setHighlightCursor(false);
    m_editor->setWordWrapMode(QTextOption::NoWrap);
    m_editor->setFont(knGlobal->editorPresetFont());
    setWidget(m_editor);

    //Link the translator.
    knUi->addTranslate(this, &KNSearchResult::retranslate);
}

void KNSearchResult::addResult(const SearchResult &result)
{
    //Construct the search result.
    QStringList resultBuffer;
    resultBuffer.append(result.keyword);
    for(int i=0; i<result.results.size(); ++i)
    {
        QStringList fileBuffer;
        auto fileItems = result.results.at(i).items;
        fileBuffer.reserve(fileItems.size() + 1);
        //Append the title.
        fileBuffer.append(QString("  %1 (%2)").arg(
                              result.results.at(i).path,
                              QString::number(fileItems.size())));
        //Construct the file result.
        for(int j=0; j<fileItems.size(); ++j)
        {
            auto item = fileItems.at(j);
            //Insert to file buffer.
            fileBuffer.append(QString("    %1: %2").arg(
                                  QString::number(item.row),
                                  QString::number(item.posInRow)));
        }
        resultBuffer.append(fileBuffer.join("\n"));
    }
    m_editor->appendPlainText(resultBuffer.join("\n"));
}

void KNSearchResult::retranslate()
{
    setWindowTitle(tr("Find result"));
}
