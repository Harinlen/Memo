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
#include "knstatuslabel.h"
#include "kntexteditor.h"

#include "knstatusbar.h"

KNStatusBar::KNStatusBar(QWidget *parent) : QStatusBar(parent),
    m_fileInfo(new KNStatusLabel(this)),
    m_cursorPos(new KNStatusLabel(this)),
    m_codecName(new KNStatusLabel(this)),
    m_overwrite(new KNStatusLabel(this))
{
    //Add widgets to status bar.
    addPermanentWidget(m_fileInfo);
    addPermanentWidget(m_cursorPos);
    addPermanentWidget(m_codecName);
    addPermanentWidget(m_overwrite);
    //Link the signal.
    connect(m_fileInfo, &KNStatusLabel::dblClicked, this, &KNStatusBar::requireSummary);
    connect(m_cursorPos, &KNStatusLabel::dblClicked, this, &KNStatusBar::requireGoto);
    connect(m_overwrite, &KNStatusLabel::dblClicked, this, &KNStatusBar::requireOverwrite);
}

void KNStatusBar::linkEditor(KNTextEditor *editor)
{
    //Clear the existed conenctions.
    removeConnections();
    //Check the editor.
    if(!editor)
    {
        return;
    }
    //Link with the new editor.
    m_connects += connect(editor, &KNTextEditor::cursorPosUpdate,
                          this, &KNStatusBar::onCursorPosUpdate);
    m_connects += connect(editor, &KNTextEditor::lengthChange,
                          this, &KNStatusBar::onLengthChange);
    m_connects += connect(editor, &KNTextEditor::overwriteModeChange,
                          this, &KNStatusBar::onOverwriteChanged);
    m_connects += connect(editor, &KNTextEditor::fileCodecChange,
                          this, &KNStatusBar::onCodecChanged);
    //Emit the signal.
    editor->syncWithStatusBar();
}

void KNStatusBar::removeConnections()
{
    //Disconnect the existed links.
    for(auto link : m_connects)
    {
        disconnect(link);
    }
    m_connects.clear();
}

void KNStatusBar::onCursorPosUpdate(int row, int column, int selected,
                                    int selectedRows)
{
    m_cursorPos->setText(QString("Ln : %1   Col : %2   Sel : %3").arg(
                             QString::number(row + 1),
                             QString::number(column + 1),
                             selected < 0 ?
                                 "N/A" :
                                 QString("%1 | %2").arg(
                                     QString::number(selected),
                                     QString::number(selectedRows))));
}

void KNStatusBar::onLengthChange(int length, int lines)
{
    m_fileInfo->setText(QString("length : %1   lines : %2").arg(
                            QString::number(length),
                            QString::number(lines)));
}

void KNStatusBar::onOverwriteChanged(bool overwrite)
{
    m_overwrite->setText(overwrite ? "OVR" : "INS");
}

void KNStatusBar::onCodecChanged(const QString &codecName)
{
    m_codecName->setText(codecName);
}
