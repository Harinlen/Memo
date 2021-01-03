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
#include <QAction>
#include <QFileInfo>
#include <QSignalMapper>

#include "knglobal.h"
#include "knuimanager.h"
#include "knconfigure.h"
#include "knconfiguremanager.h"

#include "knrecentfilerecorder.h"

#define CONFIG_FLAG ("Recent")

KNRecentFileRecorder::KNRecentFileRecorder(QObject *parent) :
    QObject(parent),
    m_signalMapper(new QSignalMapper(this)),
    m_configure(knConf->configure(KNConfigureManager::System)),
    m_fileSeperator(new QAction()),
    m_opSeperator(new QAction())
{
    //Extract the array from the configure.
    m_records = m_configure->data("Recent").toStringList();
    //Construct the actions.
    m_fileSeperator->setSeparator(true);
    m_opSeperator->setSeparator(true);
    for(int i=0; i<MAX_RECORDS; ++i)
    {
        m_fileActions[i] = new QAction();
        //Link the connections to signal mapper.
        connect(m_fileActions[i], &QAction::triggered, m_signalMapper,
                static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        //Set the mapping.
        m_signalMapper->setMapping(m_fileActions[i], i);
    }
    for(int i=0; i<RecentActionCount; ++i)
    {
        m_opActions[i] = new QAction();
    }
    m_opActions[RestoreLast]->setShortcut(QKeySequence(KNG::CTRL | KNG::SHIFT | Qt::Key_T));
    //Link the mapper with the open slots.
    connect(m_signalMapper, &QSignalMapper::mappedInt,
            this, &KNRecentFileRecorder::onActionRequest);
    //Link operations.
    connect(m_opActions[RestoreLast], &QAction::triggered,
            this, &KNRecentFileRecorder::onActionOpenLast);
    connect(m_opActions[OpenAll], &QAction::triggered,
            this, &KNRecentFileRecorder::onActionOpenAll);
    connect(m_opActions[Clear], &QAction::triggered,
            this, &KNRecentFileRecorder::clear);
    //Synchronize with the array.
    syncActionsFromArray();
    //Connect the retranslation.
    knUi->addTranslate(this, &KNRecentFileRecorder::retranslate);
}

QAction *KNRecentFileRecorder::fileAction(int id)
{
    return m_fileActions[id];
}

QAction *KNRecentFileRecorder::opAction(int id)
{
    return m_opActions[id];
}

QAction *KNRecentFileRecorder::fileSeperator()
{
    return m_fileSeperator;
}

void KNRecentFileRecorder::pushRecord(const QString &filePath)
{
    //Check whether the file path already exist.
    int fileIndex = m_records.indexOf(filePath);
    if(fileIndex != -1)
    {
        m_records.append(m_records.takeAt(fileIndex));
    }
    else
    {
        //Append to the array.
        m_records.append(filePath);
    }
    //Ensure the record length.
    recordsLengthEnsure();
}

void KNRecentFileRecorder::pushRecords(const QStringList &filePaths)
{
    //Check whether the file path already exists.
    for(auto filePath : filePaths)
    {
        int fileIndex = m_records.indexOf(filePath);
        if(fileIndex != -1)
        {
            m_records.removeAt(fileIndex);
        }
    }
    //Append all the records.
    m_records.append(filePaths);
    //Ensure the record length.
    recordsLengthEnsure();
}

void KNRecentFileRecorder::clear()
{
    //Clear the records and hide all.
    m_records = QStringList();
    //Update the configure
    m_configure->setData(CONFIG_FLAG, QJsonArray());
    //Hide all the actions.
    m_fileSeperator->setVisible(false);
    //Hide the file actions.
    for(int i=0; i<MAX_RECORDS; ++i)
    {
        //Hide the file actions.
        m_fileActions[i]->setVisible(false);
    }
    m_opSeperator->setVisible(false);
    for(int i=0; i<RecentActionCount; ++i)
    {
        m_opActions[i]->setVisible(false);
    }
}

void KNRecentFileRecorder::retranslate()
{
    //Append the file paths.
    m_opActions[RestoreLast]->setText(tr("Restore Recent Closed File"));
    m_opActions[OpenAll]->setText(tr("Open All Recent Files"));
    m_opActions[Clear]->setText(tr("Empty Recent File List"));
}

void KNRecentFileRecorder::onActionOpenLast()
{
    //Pop the last file, restore the file.
    onActionRequest(m_records.length() - 1);
}

void KNRecentFileRecorder::onActionRequest(int id)
{
    QFileInfo fileInfo(m_records.takeAt(id));
    //Synchronize the file info.
    recordsLengthEnsure();
    //Directly emit the open request.
    emit requireOpen(fileInfo.absoluteFilePath());
}

void KNRecentFileRecorder::onActionOpenAll()
{
    //Construct the file list.
    QStringList pathList;
    pathList.reserve(m_records.size());
    //Load the file path.
    for(int i=0; i<m_records.size(); ++i)
    {
        //Append the filesize to path list.
        pathList.append(QFileInfo(m_records.at(i)).absoluteFilePath());
    }
    //Clear all the records.
    m_records = QStringList();
    //Ensure the size and synchronize.
    recordsLengthEnsure();
    //Emit the signal.
    emit requireOpenList(pathList);
}

void KNRecentFileRecorder::recordsLengthEnsure()
{
    //Manage the records.
    if(m_records.size() > MAX_RECORDS)
    {
        //Shrink the records.
        m_records = m_records.mid(m_records.size() - MAX_RECORDS);
    }
    //Save the records to configure.
    saveToConfig();
    //Sync the file actions.
    syncActionsFromArray();
}

void KNRecentFileRecorder::syncActionsFromArray()
{
    //Hide all actions when record is empty.
    m_fileSeperator->setVisible(!m_records.isEmpty());
    //Hide the file actions.
    for(int i=m_records.size(); i<MAX_RECORDS; ++i)
    {
        //Hide the file actions.
        m_fileActions[i]->setVisible(false);
    }
    //Update the action text.
    for(int i=0; i<m_records.size(); ++i)
    {
        //Update the text of the action.
        m_fileActions[i]->setText(QString("&%1: %2").arg(
                                      QString::number(i), m_records.at(i)));
        //Show the action.
        m_fileActions[i]->setVisible(true);
    }
    //Check the record size.
    bool opVisible = !m_records.isEmpty();
    //Change the operation action visiblility.
    m_opSeperator->setVisible(opVisible);
    for(int i=0; i<RecentActionCount; ++i)
    {
        m_opActions[i]->setVisible(opVisible);
    }
}

void KNRecentFileRecorder::loadFromConfig()
{
    //Load the data from the configure.
    QJsonArray &&configArray = m_configure->data(CONFIG_FLAG).toJsonArray();
    //Prepare the string list.
    m_records = QStringList();
    m_records.reserve(configArray.size());
    //Set the records.
    for(int i=0; i<configArray.size(); ++i)
    {
        m_records.append(configArray.at(i).toString());
    }
}

void KNRecentFileRecorder::saveToConfig()
{
    //Construct the json array.
    QJsonArray recordBuf;
    //Set the records.
    for(int i=0; i<m_records.size(); ++i)
    {
        recordBuf.append(m_records.at(i));
    }
    //Update the configure
    m_configure->setData(CONFIG_FLAG, recordBuf);
}

QAction *KNRecentFileRecorder::opSeperator()
{
    return m_opSeperator;
}
