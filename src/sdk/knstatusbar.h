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
#ifndef KNSTATUSBAR_H
#define KNSTATUSBAR_H

#include <QStatusBar>

class KNStatusLabel;
class KNTextEditor;
/*!
 * \brief The KNStatusBar class provides the standard status widget.
 */
class KNStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNStatusBar widget.
     * \param parent The parent widget.
     */
    explicit KNStatusBar(QWidget *parent = nullptr);

    /*!
     * \brief Link the status bar to an editor.
     * \param editor The text editor.
     */
    void linkEditor(KNTextEditor *editor);

    /*!
     * \brief Remove the connections with the existed editor.
     */
    void removeConnections();

signals:
    void requireGoto();
    void requireSummary();
    void requireOverwrite();
    void requireCodec();

private slots:
    void onCursorPosUpdate(int row, int column, int selected, int selectedRows);
    void onLengthChange(int length, int lines);
    void onOverwriteChanged(bool overwrite);
    void onCodecChanged(const QString &codecName);

private:
    QVector<QMetaObject::Connection> m_connects;
    KNStatusLabel *m_fileInfo, *m_cursorPos, *m_codecName, *m_overwrite;
};

#endif // KNSTATUSBAR_H
