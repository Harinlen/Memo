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
#include "kntabbar.h"

KNTabBar::KNTabBar(QWidget *parent) : QTabBar(parent),
    m_iconSaved(QIcon(":/icons/saved.ico")),
    m_iconUnsaved(QIcon(":/icons/unsaved.ico")),
    m_iconReadOnly(QIcon(":/icons/readonly.ico"))
{
    //Set properties.
    setTabsClosable(true);
    setMovable(true);
    setExpanding(false);
    setUsesScrollButtons(true);
}

int KNTabBar::createTab(const QString &name)
{
    //Create the tab widget.
    return addTab(QIcon(m_iconSaved), name);
}

void KNTabBar::setModifiedState(int index, bool modified)
{
    //Update the icon based on the modified.
    setTabIcon(index, modified ? m_iconUnsaved : m_iconSaved);
}

void KNTabBar::setReadOnlyState(int index)
{
    //Directly set the read-only icon.
    setTabIcon(index, m_iconReadOnly);
}

void KNTabBar::goToNextTab()
{
    if(count() < 2)
    {
        return;
    }
    //Increase the current index.
    int nextId = currentIndex() + 1;
    if(nextId >= count())
    {
        nextId = 0;
    }
    setCurrentIndex(nextId);
}

void KNTabBar::goToPrevTab()
{
    if(count() < 2)
    {
        return;
    }
    //Increase the current index.
    int nextId = currentIndex() - 1;
    if(nextId < 0)
    {
        nextId = count() - 1;
    }
    setCurrentIndex(nextId);
}

void KNTabBar::moveForward()
{
    if(count() < 2 || currentIndex() == count() - 1)
    {
        return;
    }
    //Move to the next position.
    moveTab(currentIndex(), currentIndex() + 1);
}

void KNTabBar::moveBackward()
{
    if(count() < 2 || currentIndex() == 0)
    {
        return;
    }
    //Move to the next position.
    moveTab(currentIndex(), currentIndex() - 1);
}
