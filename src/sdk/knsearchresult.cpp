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
#include <QMenu>

#include "knuimanager.h"
#include "knsearchresulteditor.h"
#include "knfilemanager.h"

#include "knsearchresult.h"

KNSearchResult::KNSearchResult(QWidget *parent) :
    QDockWidget(parent),
    m_resultMenu(new QMenu(this)),
    m_manager(static_cast<KNFileManager *>(parentWidget())),
    m_editor(new KNSearchResultEditor(this))
{
    //Configure the editor.
    setWidget(m_editor);
    //Link the editor.
    connect(m_editor, &KNSearchResultEditor::requireLocate,
            this, &KNSearchResult::onShowResult);

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
                                  QString::number(item.row + 1),
                                  item.slice));
        }
        resultBuffer.append(fileBuffer.join("\n"));
    }
    //Check the cursor.
    QTextCursor tc = m_editor->textCursor();
    tc.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(tc);
    //Now insert the editor.
    m_editor->insertPlainText(resultBuffer.join("\n") + "\n");
    //Move to the result.
    tc.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(tc);
    //Save ther result.
    m_results.prepend(result);
}

void KNSearchResult::clearResult()
{
    //Clear the search result.
    m_results.clear();
    m_editor->clear();
}

void KNSearchResult::retranslate()
{
    setWindowTitle(tr("Find result"));
}

void KNSearchResult::onShowResult(int resultId, int fileId, int itemId)
{
    //Extract the file info from the result id.
    const FileResult &fileResult = m_results.at(resultId).results.at(fileId);
    //Locate the editor in the manager.
    auto editor = m_manager->locateEditor(fileResult.path);
    //Show the editor.
    if(editor)
    {
        //Extract the result.
        const ItemResult &item = fileResult.items.at(itemId);
        //Extract the editor.
        auto tc = editor->textCursor();
        QTextBlock block = editor->document()->findBlockByNumber(item.row);
        //Check the block information.
        tc.setPosition(block.position() + item.posStart);
        tc.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, item.length);
        editor->setTextCursor(tc);
        //Set focus on the editor.
        editor->setFocus();
    }
}
