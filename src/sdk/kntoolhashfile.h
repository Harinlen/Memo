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
#ifndef KNTOOLHASHFILE_H
#define KNTOOLHASHFILE_H

#include "knutil.h"

#include <QDialog>

class QPlainTextEdit;
/*!
 * \brief The KNToolHashFile class provides the dialog for generating hash data
 * from specific files.
 */
class KNToolHashFile : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNToolHashFile widget.
     * \param length The length of the hash.
     * \param hashFunc The hash function.
     * \param parent The parent widget.
     */
    explicit KNToolHashFile(int length, HashFunc hashFunc,
                            QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();
    void onSelectFiles();
    void onCopyResult();

private:
    HashFunc m_hashFunc;
    QPushButton *m_copy, *m_close, *m_selectFile;
    QPlainTextEdit *m_buffer, *m_result;
    int m_length;

};

#endif // KNTOOLHASHFILE_H
