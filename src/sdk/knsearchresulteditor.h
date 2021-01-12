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
#ifndef KNSEARCHRESULTEDITOR_H
#define KNSEARCHRESULTEDITOR_H

#include "kntexteditor.h"

class KNSearchResultEditor : public KNTextEditor
{
    Q_OBJECT
public:
    explicit KNSearchResultEditor(QWidget *widget = nullptr);

signals:
    void requireLocate(int resultId, int fileId, int itemId);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // KNSEARCHRESULTEDITOR_H
