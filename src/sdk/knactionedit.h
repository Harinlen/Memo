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
#ifndef KNACTIONEDIT_H
#define KNACTIONEDIT_H

#include <QDialog>

class QBoxLayout;
class QLabel;
class QLineEdit;
class QKeySequenceEdit;
/*!
 * \brief The KNActionEdit class provides a dialog to edit the title and
 * shortcut of the action.
 */
class KNActionEdit : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNActionEdit dialog.
     * \param parent The parent widget.
     */
    explicit KNActionEdit(QWidget *parent = nullptr);

    /*!
     * \brief Clear all the text of the edit panel.
     */
    void clear();

    QString actionName() const;

    QKeySequence actionKeySequence() const;

signals:

private slots:
    void retranslate();
    void onOkay();

private:
    QBoxLayout *m_mainLayout;
    QLabel *m_shortcut;
    QLineEdit *m_nameEdit;
    QKeySequenceEdit *m_shortcutEdit;
    QPushButton *m_okay, *m_cancel;
};

#endif // KNACTIONEDIT_H
