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
#ifndef KNFOLDERPANEL_H
#define KNFOLDERPANEL_H

#include <QModelIndex>

#include <QWidget>

class QFileSystemModel;
class QTreeView;
class QPushButton;
class KNFileManager;
/*!
 * \brief The KNFolderPanel class provides a folder panel to open and trace the
 * opened file of the file manager.
 */
class KNFolderPanel : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNFolderPanel widget.
     * \param parent The parent widget.
     */
    explicit KNFolderPanel(QWidget *parent = nullptr);

    /*!
     * \brief Set the file manager instance.
     * \param fileManager The file instance pointer.
     */
    void setFileManager(KNFileManager *fileManager);

signals:
    /*!
     * \brief When a file is require to be opened in the file manager, this
     * signal is emitted.
     * \param filePath The file path string.
     */
    void requireOpen(const QString &filePath);

public slots:
    /*!
     * \brief Set the root folder path.
     * \param path The path of the display folder.
     */
    void setFolder(const QString &path);

    /*!
     * \brief When the file tracing is enabled, this function should be called.
     */
    void onTraceFile();

private slots:
    void onOpenFile(const QModelIndex &index);

private:
    QFileSystemModel *m_folderModel;
    QModelIndex m_folderIndex;
    QTreeView *m_folderView;
    KNFileManager *m_fileManager;
    QPushButton *m_expand, *m_collapse, *m_trace;
};

#endif // KNFOLDERPANEL_H
