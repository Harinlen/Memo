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
#ifndef KNTOOLHASHINPUT_H
#define KNTOOLHASHINPUT_H

#include "knutil.h"

#include <QDialog>

class QCheckBox;
class QPlainTextEdit;
/*!
 * \brief The KNToolHashInput class provides the dialog for generating hash data
 * for the input dialog data.
 */
class KNToolHashInput : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNToolHashInput widget.
     * \param hashFunc The hash function.
     * \param parent The parent widget.
     */
    explicit KNToolHashInput(HashFunc hashFunc, QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();
    void onBufferChanged();
    void onCopyResult();

private:
    HashFunc m_hashFunc;
    QPushButton *m_copy, *m_close;
    QCheckBox *m_lineCases;
    QPlainTextEdit *m_buffer, *m_result;

};

#endif // KNTOOLHASHINPUT_H
