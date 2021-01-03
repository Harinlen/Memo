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
#include "knstatuslabel.h"

KNStatusLabel::KNStatusLabel(QWidget *parent) : QLabel(parent)
{

}

void KNStatusLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    //Do the original event.
    QLabel::mouseDoubleClickEvent(event);
    //Emit the double click signal.
    emit dblClicked();
}
