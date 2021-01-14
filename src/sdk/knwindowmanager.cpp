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
#include <QTreeView>
#include <QBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QItemSelectionModel>

#include "knfilemanager.h"
#include "knuimanager.h"
#include "kntabbar.h"
#include "kntexteditor.h"

#include "knwindowmanager.h"

KNWindowModel::KNWindowModel(KNTabBar *tabBar, KNFileManager *manager,
                             QObject *parent) :
    QAbstractTableModel(parent),
    m_tabBar(tabBar),
    m_manager(manager)
{
    knUi->addTranslate(this, &KNWindowModel::retranslate);
}

int KNWindowModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_windowInfo.size();
}

int KNWindowModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant KNWindowModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::TextAlignmentRole)
    {
        return (index.column() == 2) ? Qt::AlignCenter : Qt::AlignLeft;
    }
    const WindowInfo &info = m_windowInfo.at(index.row());
    //Ignore the other role.
    if(role == Qt::DisplayRole)
    {
        //Check the column.
        switch(index.column())
        {
        case 0:
            return info.name;
        case 1:
            return info.path;
        case 2:
            return info.size;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant KNWindowModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if(orientation == Qt::Horizontal)
    {
        switch(role)
        {
        case Qt::DisplayRole:
            //Check the section.
            return m_sectionName[section];
        case Qt::TextAlignmentRole:
            return (section == 2) ? Qt::AlignCenter : Qt::AlignLeft;
        }
    }
    return QVariant();
}

bool KNWindowModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index)
    //As the documentation said, called this function first.
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    while(rows--)
    {
        m_windowInfo.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void KNWindowModel::clearModel()
{
    if(m_windowInfo.isEmpty())
    {
        return;
    }
    beginResetModel();
    m_windowInfo.clear();
    endResetModel();
}

void KNWindowModel::synchronizeModel()
{
    //Clear the model data first.
    if(!m_windowInfo.isEmpty())
    {
        clearModel();
    }
    //Based on the tab bar, generate the window information.
    beginInsertRows(QModelIndex(), 0, m_tabBar->count());
    for(int i=0; i<m_tabBar->count(); ++i)
    {
        WindowInfo info;
        info.name = m_tabBar->tabText(i);
        KNTextEditor *editor = m_manager->editorAt(i);
        info.path = editor->filePath();
        info.size = editor->document()->characterCount();
        info.tabId = i;
        m_windowInfo.append(info);
    }
    endInsertRows();
}

int KNWindowModel::tabId(int row) const
{
    return m_windowInfo.at(row).tabId;
}

void KNWindowModel::sort(int column, Qt::SortOrder order)
{
    //Sort the data based on the column.
    switch(column)
    {
    case 0:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_windowInfo.begin(), m_windowInfo.end(),
                      [=](const WindowInfo &l, const WindowInfo &r)
            {
                return l.name < r.name;
            });
        }
        else
        {
            std::sort(m_windowInfo.begin(), m_windowInfo.end(),
                      [=](const WindowInfo &l, const WindowInfo &r)
            {
                return l.name > r.name;
            });
        }
        break;
    case 1:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_windowInfo.begin(), m_windowInfo.end(),
                      [=](const WindowInfo &l, const WindowInfo &r)
            {
                return l.path < r.path;
            });
        }
        else
        {
            std::sort(m_windowInfo.begin(), m_windowInfo.end(),
                      [=](const WindowInfo &l, const WindowInfo &r)
            {
                return l.path > r.path;
            });
        }
        break;
    case 2:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_windowInfo.begin(), m_windowInfo.end(),
                      [=](const WindowInfo &l, const WindowInfo &r)
            {
                return l.size < r.size;
            });
        }
        else
        {
            std::sort(m_windowInfo.begin(), m_windowInfo.end(),
                      [=](const WindowInfo &l, const WindowInfo &r)
            {
                return l.size > r.size;
            });
        }
        break;
    }
    //Update the model.
    dataChanged(index(0,0),index(m_windowInfo.count() - 1, 2));
}

QVector<int> KNWindowModel::tabIdList() const
{
    QVector<int> tabIds;
    tabIds.reserve(m_windowInfo.size());
    for(auto info : m_windowInfo)
    {
        tabIds.append(info.tabId);
    }
    return tabIds;
}

void KNWindowModel::retranslate()
{
    m_sectionName[0] = tr("Name");
    m_sectionName[1] = tr("Path");
    m_sectionName[2] = tr("Size");
}

KNWindowManager::KNWindowManager(KNTabBar *tabBar, QWidget *parent) :
    QDialog(parent),
    m_tabBar(tabBar),
    m_fileManager(static_cast<KNFileManager *>(parent)),
    m_tabView(new QTreeView(this)),
    m_tabModel(new KNWindowModel(tabBar, m_fileManager, this)),
    m_activate(new QPushButton(this)),
    m_save(new QPushButton(this)),
    m_closes(new QPushButton(this)),
    m_sort(new QPushButton(this)),
    m_okay(new QPushButton(this))
{
    //Set the initial size.
    resize(knUi->size(600, 400));
    //Construct the window.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    setLayout(mainLayout);
    //Add widget to layout.
    mainLayout->addWidget(m_tabView, 1);
    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->addLayout(buttonLayout);
    //Add widgets.
    buttonLayout->addWidget(m_activate);
    buttonLayout->addWidget(m_save);
    buttonLayout->addWidget(m_closes);
    buttonLayout->addWidget(m_sort);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okay);
    //Configure view.
    m_tabView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabView->setIndentation(0);
    m_tabView->setEditTriggers(QTreeView::NoEditTriggers);
    m_tabView->header()->setStretchLastSection(false);
    m_tabView->setSelectionMode(QTreeView::ExtendedSelection);
    m_tabView->setModel(m_tabModel);
    m_tabView->setSortingEnabled(true);
    m_tabView->setUniformRowHeights(true);
    m_tabView->setAllColumnsShowFocus(true);
    //Link selection changed signal.
    connect(m_tabView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &KNWindowManager::onSelectChange);
    //Configure buttons.
    m_activate->setDefault(true);
    //Link the buttons.
    connect(m_activate, &QPushButton::clicked, this, &KNWindowManager::onActivate);
    connect(m_closes, &QPushButton::clicked, this, &KNWindowManager::onCloses);
    connect(m_save, &QPushButton::clicked, this, &KNWindowManager::onSave);
    connect(m_sort, &QPushButton::clicked, this, &KNWindowManager::onSort);
    connect(m_okay, &QPushButton::clicked, this, &KNWindowManager::close);
    //Link translator.
    knUi->addTranslate(this, &KNWindowManager::retranslate);
}

void KNWindowManager::showEvent(QShowEvent *event)
{
    //Synchronize the widget.
    m_tabModel->synchronizeModel();
    //Show the widget.
    QDialog::showEvent(event);
}

void KNWindowManager::retranslate()
{
    setWindowTitle(tr("Windows"));
    //Update tab label.
    m_activate->setText(tr("&Activate"));
    m_save->setText(tr("&Save"));
    m_closes->setText(tr("&Close window(s)"));
    m_sort->setText(tr("Sort &tabs"));
    m_okay->setText(tr("&Ok"));
}

void KNWindowManager::onSelectChange()
{
    //Check whether each button should be changed.
    auto rows = m_tabView->selectionModel()->selectedRows(0);
    m_activate->setEnabled(rows.size() == 1);
    m_save->setEnabled(rows.size() > 0);
    m_closes->setEnabled(rows.size() > 0);
}

void KNWindowManager::onActivate()
{
    //Set current index to the current selected one.
    auto rows = m_tabView->selectionModel()->selectedRows(0);
    //Check the rows information.
    m_tabBar->setCurrentIndex(m_tabModel->tabId(rows.at(0).row()));
}

void KNWindowManager::onSave()
{
    //Extract the tab ids from the model.
    auto rows = m_tabView->selectionModel()->selectedRows(0);
    //Save the current rows.
    for(int i=0; i<rows.size(); ++i)
    {
        //Extract the tab id.
        int tabId = m_tabModel->tabId(rows.at(i).row());
        //Save the editor.
        if(!m_fileManager->saveEditor(m_fileManager->editorAt(tabId)))
        {
            return;
        }
    }
}

void KNWindowManager::onCloses()
{
    //Extract the tab ids from the model.
    auto rows = m_tabView->selectionModel()->selectedRows(0);
    //Gather the selected tab id from the model.
    QVector<int> tabIds;
    for(int i=0; i<rows.size(); ++i)
    {
        //Append tab id.
        tabIds.append(m_tabModel->tabId(rows.at(i).row()));
    }
    //Close the tab ids from the file manager.
    if(m_fileManager->closeTabs(tabIds))
    {
        //Sync the model.
        m_tabModel->synchronizeModel();
    }
}

void KNWindowManager::onSort()
{
    //Extract the new tab id list.
    auto sortedIds = m_tabModel->tabIdList();
    //Update the tab ids.
    QVector<int> currentIds;
    currentIds.reserve(sortedIds.size());
    for(int i=0; i<sortedIds.size(); ++i)
    {
        currentIds.append(i);
    }
    //Make current Ids to be the sorted Id.
    for(int i=sortedIds.size()-1; i>-1; --i)
    {
        //Check whether the tab matches.
        if(sortedIds.at(i) != currentIds.at(i))
        {
            m_tabBar->moveTab(sortedIds.at(i), currentIds.at(i));
            currentIds.move(sortedIds.at(i), currentIds.at(i));
        }
    }
    //Syncrhonize the model.
    m_tabModel->synchronizeModel();
}
