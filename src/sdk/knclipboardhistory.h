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
#ifndef KNCLIPBOARDHISTORY_H
#define KNCLIPBOARDHISTORY_H

#include <QDockWidget>

class QListView;
class QStringListModel;
/*!
 * \brief The KNClipboardHistory class provides a clipboard history recording
 * widget.
 */
class KNClipboardHistory : public QDockWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNClipboardHistory widget.
     * \param parent The parent widget.
     */
    explicit KNClipboardHistory(QWidget *parent = nullptr);

signals:
    /*!
     * \brief Request to insert the specific text to text editor.
     * \param data The data string to insert.
     */
    void requireInsert(QString data);

private slots:
    void retranslate();
    void onDataChanged();
    void onLinkPaste(const QModelIndex &index);

private:
    QStringList m_history, m_display;
    QListView *m_historyView;
    QStringListModel *m_historyModel;
};

#endif // KNCLIPBOARDHISTORY_H
