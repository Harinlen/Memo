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
#ifndef KNSTATUSLABEL_H
#define KNSTATUSLABEL_H

#include <QLabel>

/*!
 * \brief The KNStatusLabel class provides the label button widget for the
 * status bar.
 */
class KNStatusLabel : public QLabel
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNStatusLabel widget.
     * \param parent The parent widget.
     */
    explicit KNStatusLabel(QWidget *parent = nullptr);

signals:
    /*!
     * \brief When the label is double clicked, this signal is emitted.
     */
    void dblClicked();

protected:
    /*!
     * \brief Reimplemented from QFrame::mouseDoubleClickEvent().
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // KNSTATUSLABEL_H
