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
#include <QKeyEvent>

#include "kntabmodel.h"
#include "knuimanager.h"

#include "kntabswitcher.h"

KNTabSwitcher::KNTabSwitcher(QWidget *parent) :
    QListView(parent),
    m_tabModel(new KNTabModel(this))
{
    setFixedWidth(knUi->width(870));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWindowFlag(Qt::ToolTip);
    setModel(m_tabModel);
    //When the item is clicked, also hide the list view.
    connect(this, &KNTabSwitcher::clicked,
            this, &KNTabSwitcher::hide);
}

void KNTabSwitcher::addEditor(KNTextEditor *editor)
{
    m_tabModel->addEditor(editor);
}

void KNTabSwitcher::removeEditor(KNTextEditor *editor)
{
    m_tabModel->removeEditor(editor);
}

void KNTabSwitcher::topEditor(KNTextEditor *editor)
{
    m_tabModel->moveEditorToTop(editor);
}

void KNTabSwitcher::hideEvent(QHideEvent *event)
{
    //When the switcher is hiding, show the current editor.
    QListView::hideEvent(event);
    //Extract the current editor.
    emit requireShowEditor(m_tabModel->editorAt(currentIndex().row()));
}

void KNTabSwitcher::focusOutEvent(QFocusEvent *event)
{
    //When the focus is out, hide the window as well.
    QListView::focusOutEvent(event);
    //Hide the widget.
    hide();
    parentWidget()->setFocus();
}

void KNTabSwitcher::keyPressEvent(QKeyEvent *event)
{
    //Check the key event.
    switch(event->key())
    {
    case Qt::Key_Tab:
    {
        auto currentRow = currentIndex().row();
        int nextRow = (currentRow == m_tabModel->rowCount() - 1) ?
                    0 : currentRow + 1;
        //Move to that row.
        setCurrentIndex(m_tabModel->index(nextRow));
        break;
    }
    }
    //Construct the key event.
    QListView::keyPressEvent(event);
}

void KNTabSwitcher::keyReleaseEvent(QKeyEvent *event)
{
    QListView::keyReleaseEvent(event);
    //Check the event.
    if(event->key() == Qt::Key_Control)
    {
        //Hide the current tab.
        hide();
    }
}
