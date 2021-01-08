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
#ifndef KNCODECDIALOG_H
#define KNCODECDIALOG_H

#include <QDialog>

class QLabel;
class QListView;
class QPushButton;
class KNTextEditor;
class KNCodecDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KNCodecDialog(QWidget *parent = nullptr);

signals:

public slots:
    void showDialog(KNTextEditor *editor);

private slots:
    void retranslate();
    void onLoadAsCodec();
    void onSaveAsCodec();

private:
    QLabel *m_label;
    QListView *m_codecList;
    QPushButton *m_loadAs, *m_saveAs, *m_close;
    KNTextEditor *m_editor;
    QList<int> m_sortedMibs;
};

#endif // KNCODECDIALOG_H
