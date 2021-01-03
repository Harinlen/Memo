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
#ifndef KNTOOLMENU_H
#define KNTOOLMENU_H

#include <QMenu>

/*!
 * \brief The KNToolMenu class provides the tool menu.
 */
class KNToolMenu : public QMenu
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNToolMenu widget.
     * \param parent The parent widget.
     */
    explicit KNToolMenu(QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();

private:
};

#endif // KNTOOLMENU_H
