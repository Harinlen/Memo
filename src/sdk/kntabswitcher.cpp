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

#include "kntabswitcher.h"

KNTabSwitcher::KNTabSwitcher(QWidget *parent) :
    QListView(parent)
{
    setWindowFlag(Qt::ToolTip);
}

void KNTabSwitcher::focusOutEvent(QFocusEvent *event)
{
    //When the focus is out, hide the window as well.
    QListView::focusOutEvent(event);
    //Hide the widget.
    hide();
    parentWidget()->setFocus();
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
