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
#ifndef KNLINEEDIT_H
#define KNLINEEDIT_H

#include <QLineEdit>

/*!
 * \brief The KNLineEdit class is a normal line edit but could emit signal when
 * focus is reached.
 */
class KNLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNLineEdit widget.
     * \param parent The parent widget.
     */
    explicit KNLineEdit(QWidget *parent = nullptr);

signals:
    /*!
     * \brief When the line edit has the focus, this signal will be emitted.
     */
    void editHasFocus();

protected:
    /*!
     * \brief Reimplemented from QLineEdit::focusInEvent().
     * \param event
     */
    void focusInEvent(QFocusEvent *event) override;
};

#endif // KNLINEEDIT_H
