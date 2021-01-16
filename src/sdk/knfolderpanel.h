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
class KNFolderPanel : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNFolderPanel widget.
     * \param parent The parent widget.
     */
    explicit KNFolderPanel(QWidget *parent = nullptr);

    void setFileManager(KNFileManager *fileManager);

signals:
    void requireOpen(const QString &filePath);

public slots:
    void setFolder(const QString &path);
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
