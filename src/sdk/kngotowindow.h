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
#ifndef KNGOTOWINDOW_H
#define KNGOTOWINDOW_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QBoxLayout;
class QRadioButton;
class QPushButton;
class KNTextEditor;
/*!
 * \brief The KNGotoWindow class provides the goto dialog of a specific editor.
 */
class KNGotoWindow : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNGotoWindow dialog.
     * \param parent The parent widget.
     */
    explicit KNGotoWindow(QWidget *parent = nullptr);

    /*!
     * \brief Get whether the line radio button is selected.
     * \return If line is selected, return true.
     */
    bool isLine() const;

    /*!
     * \brief Get the user input target value.
     * \return The converted value of the input box.
     */
    int targetValue() const;

signals:

public slots:
    /*!
     * \brief Show the goto dialog.
     * \param editor The editor pointer.
     * \return Get the return value of the operation.
     */
    int showGoto(KNTextEditor *editor);

private slots:
    void retranslate();
    void updateLabelInfo();

private:
    QRadioButton *m_line, *m_offset;
    QLabel *m_currentLabel, *m_maximumLabel, *m_targetLabel, *m_currentPos,
           *m_maximumPos;
    QLineEdit *m_position;
    QPushButton *m_okay, *m_cancel;
    KNTextEditor *m_editor;
};

#endif // KNGOTOWINDOW_H
