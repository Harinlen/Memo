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
#ifndef KNRUNDIALOG_H
#define KNRUNDIALOG_H

#include <QDialog>

class QLabel;
class QComboBox;
class QPushButton;
/*!
 * \brief The KNRunDialog class provides the run dialog of the Run menu.
 */
class KNRunDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNRunDialog widget.
     * \param parent The parent widget.
     */
    explicit KNRunDialog(QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();
    void onSelect();
    void onRun();

private:
    QLabel *m_hint;
    QComboBox *m_command;
    QPushButton *m_run, *m_save, *m_cancel, *m_select;
};

#endif // KNRUNDIALOG_H
