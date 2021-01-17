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
#include <QJsonArray>

#include "knuimanager.h"
#include "knrundialog.h"
#include "knconfigure.h"
#include "knconfiguremanager.h"

#include "knrunmenu.h"

#define RUN_FIELD   "RunCommands"

#define CMD_TITLE   "Title"
#define CMD_CODE    "Command"
#define CMD_KEYS    "Shortcut"

KNRunMenu::KNRunMenu(QWidget *parent) :
    QMenu(parent),
    m_runDialog(new KNRunDialog(parent)),
    m_runConfig(knConf->userConfigure())
{
    //Construct actions.
    for(int i=0; i<RunMenuItemCount; ++i)
    {
        m_actions[i] = new QAction(this);
    }
    //Add actions.
    addAction(m_actions[Execute]);
    m_commandSeperator = addSeparator();
    //Construct the action based on the configure.
    auto commandArray = m_runConfig->data(RUN_FIELD).toJsonArray();
    for(int i=0; i<commandArray.size(); ++i)
    {
        //Construct the action from the action.
        ;
    }
    //Link actions.
    connect(m_actions[Execute], &QAction::triggered,
            m_runDialog, &KNRunDialog::show);
    //Link translator.
    knUi->addTranslate(this, &KNRunMenu::retranslate);
}

void KNRunMenu::retranslate()
{
    setTitle(tr("&Run"));
    //Execute a program.
    m_actions[Execute]->setText(tr("&Run..."));
}

QAction *KNRunMenu::actionFromObject(const QJsonObject &obj)
{
    //Construct the action information.
    QAction *action = new QAction(obj.value(CMD_TITLE).toString(), this);
    //Add action information.
    return action;
}
