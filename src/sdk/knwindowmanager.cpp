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

#include "knuimanager.h"

#include "knwindowmanager.h"



KNWindowManager::KNWindowManager(KNTabBar *tabBar, QWidget *parent) :
    QDialog(parent),
    m_tabBar(tabBar),
    m_tabView(new QTreeView(this)),
    m_activate(new QPushButton(this)),
    m_save(new QPushButton(this)),
    m_closes(new QPushButton(this)),
    m_sort(new QPushButton(this)),
    m_okay(new QPushButton(this))
{
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
    //Configure buttons.
    m_activate->setDefault(true);
    connect(m_okay, &QPushButton::clicked, this, &KNWindowManager::close);
    //Link translator.
    knUi->addTranslate(this, &KNWindowManager::retranslate);
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
