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
#ifndef KNTABSWITCHER_H
#define KNTABSWITCHER_H

#include <QListView>

/*!
 * \brief The KNTabSwitcher class provides the listview of the switch board
 * widget.
 */
class KNTabSwitcher : public QListView
{
    Q_OBJECT
public:
    explicit KNTabSwitcher(QWidget *parent = nullptr);

signals:

protected:
    /*!
     * \brief focusOutEvent
     * \param event
     */
    void focusOutEvent(QFocusEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // KNTABSWITCHER_H
