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
#include <QLabel>
#include <QLineEdit>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QBoxLayout>

#include "knuimanager.h"

#include "knactionedit.h"

KNActionEdit::KNActionEdit(QWidget *parent) :
    QDialog(parent),
    m_mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
    m_shortcut(new QLabel(this)),
    m_nameEdit(new QLineEdit(this)),
    m_shortcutEdit(new QKeySequenceEdit(this)),
    m_okay(new QPushButton(this)),
    m_cancel(new QPushButton(this))
{
    //Construct the layout.
    setLayout(m_mainLayout);
    //Add widget.
    m_mainLayout->addWidget(m_nameEdit);
    QBoxLayout *editLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_mainLayout->addLayout(editLayout);
    editLayout->addWidget(m_shortcut);
    editLayout->addWidget(m_shortcutEdit);
    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_mainLayout->addLayout(buttonLayout);
    buttonLayout->addWidget(m_okay);
    buttonLayout->addWidget(m_cancel);
    //Configure the button.
    m_okay->setDefault(true);
    connect(m_okay, &QPushButton::clicked, this, &KNActionEdit::onOkay);
    connect(m_cancel, &QPushButton::clicked, this, &KNActionEdit::reject);
    //Link translator.
    knUi->addTranslate(this, &KNActionEdit::retranslate);
}

void KNActionEdit::clear()
{
    m_nameEdit->clear();
    m_shortcutEdit->clear();
}

QString KNActionEdit::actionName() const
{
    return m_nameEdit->text();
}

QKeySequence KNActionEdit::actionKeySequence() const
{
    return m_shortcutEdit->keySequence();
}

void KNActionEdit::retranslate()
{
    m_nameEdit->setPlaceholderText(tr("Name"));
    m_shortcut->setText(tr("Shortcut"));
    m_okay->setText(tr("OK"));
    m_cancel->setText(tr("Cancel"));
    //Update the size.
    setFixedSize(m_mainLayout->sizeHint());
}

void KNActionEdit::onOkay()
{
    //Check whether the name is empty or not.
    if(m_nameEdit->text().isEmpty())
    {
        //Set focus back to name edit.
        m_nameEdit->setFocus();
        return;
    }
    //Accept the action.
    accept();
}
