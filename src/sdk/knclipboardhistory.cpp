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
#include <QListView>
#include <QStringListModel>
#include <QMap>
#include <QApplication>
#include <QClipboard>

#include "knuimanager.h"

#include "knclipboardhistory.h"

#define MAX_RECORD  (64)

KNClipboardHistory::KNClipboardHistory(QWidget *parent) :
    QDockWidget(parent),
    m_historyView(new QListView(this)),
    m_historyModel(new QStringListModel(this))
{
    //Set the list view.
    setWidget(m_historyView);
    //Set the model.
    m_historyView->setModel(m_historyModel);
    m_historyView->setEditTriggers(QListView::NoEditTriggers);
    //Link the clipboard.
    connect(qApp->clipboard(), &QClipboard::dataChanged,
            this, &KNClipboardHistory::onDataChanged);
    //Link the view widget.
    connect(m_historyView, &QListView::activated,
            this, &KNClipboardHistory::onLinkPaste);
    //Link the retranslate.
    knUi->addTranslate(this, &KNClipboardHistory::retranslate);
    //Check the clipboard is valid or not.
    onDataChanged();
}

void KNClipboardHistory::retranslate()
{
    //Update the title.
    setWindowTitle(tr("Clipboard History"));
}

QString displayText(const QString &source)
{
    QString simp = source.simplified();
    if(simp.length() > 22)
    {
        return simp.left(22) + "...";
    }
    return simp;
}

void KNClipboardHistory::onDataChanged()
{
    //Check the clipboard data.
    QString &&buf = qApp->clipboard()->text();
    //Check whether the buf existed in the string list.
    if(buf.isEmpty())
    {
        return;
    }
    int bufIndex = m_history.indexOf(buf);
    if(bufIndex == -1)
    {
        //Prepend the shorten the history.
        m_history.prepend(buf);
        m_display.prepend(displayText(buf));
        if(m_history.length() >= MAX_RECORD)
        {
            m_history = m_history.mid(0, MAX_RECORD);
            m_display = m_display.mid(0, MAX_RECORD);
        }
    }
    else
    {
        //Move the history to the first.
        m_display.move(bufIndex, 0);
        //Move the record to the first.
        m_history.move(bufIndex, 0);
    }
    //Update the model.
    m_historyModel->setStringList(m_display);
}

void KNClipboardHistory::onLinkPaste(const QModelIndex &index)
{
    //Set the paste data.
    emit requireInsert(m_history.at(index.row()));
}
