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
#include "kntexteditor.h"

#include "kntabmodel.h"

KNTabModel::KNTabModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int KNTabModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    //Show the content of the row.
    return m_tabs.size();
}

QVariant KNTabModel::data(const QModelIndex &index, int role) const
{
    auto editor = m_tabs.at(index.row());
    switch(role)
    {
    case Qt::DisplayRole:
        return editor->isOnDisk() ?
                    QString("%1 - %2").arg(editor->documentTitle(),
                                                          editor->filePath()) :
                    editor->documentTitle();
    }
    return QVariant();
}

bool KNTabModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index)
    //As the documentation said, called this function first.
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    while(rows--)
    {
        m_tabs.removeAt(position);
    }
    endRemoveRows();
    return true;
}

KNTextEditor *KNTabModel::editorAt(int row)
{
    return m_tabs.at(row);
}

void KNTabModel::addEditor(KNTextEditor *editor)
{
    //Insert the editor to the beginning of the tab bar.
    beginInsertRows(QModelIndex(), 0, 0);
    m_tabs.prepend(editor);
    endInsertRows();
}

void KNTabModel::removeEditor(KNTextEditor *editor)
{
    //Find the editor in the list.
    int editorId = editorIndex(editor);
    //Update the editor id.
    removeRow(editorId);
}

void KNTabModel::moveEditorToTop(KNTextEditor *editor)
{
    //Find the editor in the list.
    int editorId = editorIndex(editor);
    if(editorId == 0)
    {
        return;
    }
    if(editorId != -1)
    {
        beginMoveRows(QModelIndex(), editorId, editorId,
                      QModelIndex(), 0);
        m_tabs.move(editorId, 0);
        endMoveRows();
    }
}

int KNTabModel::editorIndex(KNTextEditor *editor)
{
    for(int i=0; i<m_tabs.size(); ++i)
    {
        if(m_tabs.at(i) == editor)
        {
            return i;
        }
    }
    return -1;
}
