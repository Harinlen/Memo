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
#include "knglobal.h"
#include "knsearchhighlighter.h"

#include "knsearchresulteditor.h"

KNSearchResultEditor::KNSearchResultEditor(QWidget *widget) :
    KNTextEditor(QString(), QString(), QString(), widget,
                 new KNSearchHighlighter(widget), false)
{
    //Set properties.
    setReadOnly(true);
    setCursorVisible(false);
    setLineNumberVisible(false);
    setBookmarkVisible(false);
    setWordWrapMode(QTextOption::NoWrap);
    setFont(knGlobal->editorPresetFont());
}

void KNSearchResultEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    //Convert the position into block number.
    QTextCursor tc = cursorForPosition(event->pos());
    //Request to jump to the cursor position.
    auto block = tc.block();
    auto data = blockData(block);
    switch(data->level)
    {
    case 0:
    case 1:
        //Fold this level.
        break;
    case 2:
        //Jump to the target position.
        //Find the item index.
        int itemIndex = -1;
        while(block.isValid() && data->level == 2)
        {
            //Increase the item position
            ++itemIndex;
            //Switch to next block
            block = block.previous();
            data = blockData(block);
        }
        //Now Find the file index.
        int fileIndex = -1;
        while(block.isValid() && data->level > 0)
        {
            //Increase the item position.
            if(data->level == 1)
            {
                ++fileIndex;
            }
            //Switch to next block.
            block = block.previous();
            data = blockData(block);
        }
        //Finally, find the result index.
        int resultIndex = -1;
        while(block.isValid())
        {
            //Increase the item position.
            if(data->level == 0)
            {
                ++resultIndex;
            }
            //Switch to next block.
            block = block.previous();
            data = blockData(block);
        }
        //Locate the file result.
        emit requireLocate(resultIndex, fileIndex, itemIndex);
        break;
    }
    event->ignore();
    return;
}
