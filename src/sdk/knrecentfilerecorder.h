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
#ifndef KNRECENTFILERECORDER_H
#define KNRECENTFILERECORDER_H

#include <QJsonArray>

#include <QObject>

#define MAX_RECORDS (9)

class QAction;
class QSignalMapper;
class KNConfigure;
/*!
 * \brief The KNRecentFileRecorder class records the recent opened files.
 */
class KNRecentFileRecorder : public QObject
{
    Q_OBJECT
public:
    enum RecentActions
    {
        RestoreLast,
        OpenAll,
        Clear,
        RecentActionCount
    };

    /*!
     * \brief Construct a KNRecentFileRecorder object
     * \param parent The parent object.
     */
    explicit KNRecentFileRecorder(QObject *parent = nullptr);

    /*!
     * \brief Get the file access action.
     * \param id The action id.
     * \return The action pointer.
     */
    QAction *fileAction(int id);

    /*!
     * \brief Get the recent recorder operation action.
     * \param id The action id.
     * \return The action pointer.
     */
    QAction *opAction(int id);

    /*!
     * \brief Get the file list seperator action.
     * \return The action pointer.
     */
    QAction *fileSeperator();

    /*!
     * \brief Get the operations seperator action.
     * \return The action pointer.
     */
    QAction *opSeperator();

signals:
    /*!
     * \brief Require to open the file path.
     * \param filePath The target file path.
     */
    void requireOpen(const QString &filePath);

    /*!
     * \brief Require to open a sort of files.
     * \param filePaths The file path list.
     */
    void requireOpenList(const QStringList &filePaths);

public slots:
    /*!
     * \brief Append a file open record.
     * \param filePath The file path.
     */
    void pushRecord(const QString &filePath);

    /*!
     * \brief Push multiple file path records to recent manager.
     * \param filePaths The file path list.
     */
    void pushRecords(const QStringList &filePaths);

    /*!
     * \brief Clear the recent file list.
     */
    void clear();

private slots:
    void retranslate();
    void onActionOpenLast();
    void onActionRequest(int id);
    void onActionOpenAll();

private:
    void recordsLengthEnsure();
    void syncActionsFromArray();
    void loadFromConfig();
    void saveToConfig();
    QSignalMapper *m_signalMapper;
    KNConfigure *m_configure;
    QStringList m_records;
    QAction *m_fileSeperator, *m_opSeperator;
    QAction *m_fileActions[MAX_RECORDS], *m_opActions[RecentActionCount];
};

#endif // KNRECENTFILERECORDER_H
