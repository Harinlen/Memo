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
#include <QSignalMapper>

#include "kntabbar.h"
#include "knuimanager.h"
#include "knwindowmanager.h"

#include "knwindowsmenu.h"

KNWindowsMenu::KNWindowsMenu(KNTabBar *tabBar, QWidget *parent) : QMenu(parent),
    m_tabBar(tabBar),
    m_manager(new KNWindowManager(tabBar, parent))
{
    //Create signal mapper.
    QSignalMapper *mapper = new QSignalMapper(this);
    for(int i=0; i<WindowMenuItemCount; ++i)
    {
        m_menuItems[i] = new QAction(this);
        //Add action to menu.
        addAction(m_menuItems[i]);
        //Link the actions.
        connect(m_menuItems[i], &QAction::triggered,
                mapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        mapper->setMapping(m_menuItems[i], i);
    }
    connect(mapper, &QSignalMapper::mappedInt, this, &KNWindowsMenu::onShowTab);
    connect(m_menuItems[WindowManager], &QAction::triggered,
            m_manager, &KNWindowManager::exec);

    //Link translator.
    knUi->addTranslate(this, &KNWindowsMenu::retranslate);
}

void KNWindowsMenu::showEvent(QShowEvent *event)
{
    //Update the item layout.
    int shownTabs = qMin(m_tabBar->count(), 10);
    for(int i=0; i<shownTabs; ++i)
    {
        //Change the item label and visible.
        m_menuItems[i]->setText(QString("%1: %2").arg(
                                    QString::number(i+1),
                                    m_tabBar->tabText(i)));
        m_menuItems[i]->setVisible(true);
    }
    //Hide all the other tabs.
    for(int i=shownTabs; i<10; ++i)
    {
        m_menuItems[i]->setVisible(false);
    }
    //Show the menu.
    QMenu::showEvent(event);
}

void KNWindowsMenu::retranslate()
{
    setTitle(tr("&Window"));
    //Add the menu items.
    m_menuItems[WindowManager]->setText(tr("&Windows..."));
}

void KNWindowsMenu::onShowTab(int tabId)
{
    //Set the current tab id to tab bar.
    m_tabBar->setCurrentIndex(tabId);
}
