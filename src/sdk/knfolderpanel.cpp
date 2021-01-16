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
#include <QFileSystemModel>
#include <QTreeView>
#include <QPushButton>
#include <QBoxLayout>
#include <QScrollBar>

#include "kniconprovider.h"
#include "knuimanager.h"
#include "knfilemanager.h"
#include "kntexteditor.h"

#include "knfolderpanel.h"

KNFolderPanel::KNFolderPanel(QWidget *parent) : QWidget(parent),
    m_folderModel(new QFileSystemModel(this)),
    m_folderView(new QTreeView(this)),
    m_fileManager(nullptr),
    m_expand(new QPushButton(QIcon(":/icons/expand_all.png"), QString(), this)),
    m_collapse(new QPushButton(QIcon(":/icons/fold_all.png"), QString(), this)),
    m_trace(new QPushButton(QIcon(":/icons/select_current_file.png"), QString(), this))
{
    //Add widgets.
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setContentsMargins(knUi->margins(0, 0, 0, 0));
    layout->setSpacing(0);
    setLayout(layout);
    //Add buttons to layout.
    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addLayout(buttonLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_expand);
    buttonLayout->addWidget(m_collapse);
    buttonLayout->addWidget(m_trace);
    layout->addWidget(m_folderView, 1);
    m_folderView->setAllColumnsShowFocus(true);
    //Set the button states.
    m_trace->setCheckable(true);
    //Configure the file view.
    m_folderView->setModel(m_folderModel);
    m_folderView->hideColumn(1);
    m_folderView->hideColumn(2);
    m_folderView->hideColumn(3);
    m_folderView->setHeaderHidden(true);
    m_folderView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_folderView->setRootIsDecorated(true);
    //Customized the icon provider.
    m_folderModel->setIconProvider(new KNIconProvider());
    //Link the view signals.
    connect(m_folderView, &QTreeView::activated, this, &KNFolderPanel::onOpenFile);
    //Link buttons.
    connect(m_expand, &QPushButton::clicked, m_folderView, &QTreeView::expandAll);
    connect(m_collapse, &QPushButton::clicked, m_folderView, &QTreeView::collapseAll);
    connect(m_trace, &QPushButton::clicked, this, &KNFolderPanel::onTraceFile);
    connect(m_folderView->verticalScrollBar(), &QScrollBar::rangeChanged, this, &KNFolderPanel::onTraceFile);
}

void KNFolderPanel::setFolder(const QString &path)
{
    //Change the view root item.
    m_folderIndex = m_folderModel->setRootPath(path);
    m_folderView->setRootIndex(m_folderIndex);
}

void KNFolderPanel::onTraceFile()
{
    if(!m_fileManager || !m_trace->isChecked() || !m_folderIndex.isValid())
    {
        return;
    }
    //Check the current editor.
    auto editor = m_fileManager->currentEditor();
    if(Q_LIKELY(editor) && editor->isOnDisk())
    {
        //Try to find the index inside the model.
        auto index = m_folderModel->index(editor->filePath());
        //Check whether the index parent is my parent.
        QModelIndexList parents;
        auto folder = index.parent();
        bool meetModelRoot = false;
        while(folder.isValid())
        {
            parents.append(folder);
            if(folder == m_folderIndex)
            {
                meetModelRoot = true;
                break;
            }
            folder = folder.parent();
        }
        //Check whether it meet the folder.
        if(!meetModelRoot)
        {
            return;
        }
        //Expanded all the parents.
        m_folderView->expandRecursively(index);
        //Select the index at the model.
        m_folderView->setCurrentIndex(index);
        m_folderView->scrollTo(index, QTreeView::PositionAtCenter);
    }
}

void KNFolderPanel::onOpenFile(const QModelIndex &index)
{
    //Emit the open file signal.
    const QFileInfo &fileInfo = m_folderModel->fileInfo(index);
    //Check the file info.
    if(fileInfo.isFile())
    {
        emit requireOpen(fileInfo.absoluteFilePath());
    }
}

void KNFolderPanel::setFileManager(KNFileManager *fileManager)
{
    m_fileManager = fileManager;
}
