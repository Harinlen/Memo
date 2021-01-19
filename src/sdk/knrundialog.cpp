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
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QFileDialog>

#include "knutil.h"
#include "knuimanager.h"
#include "knactionedit.h"

#include "knrundialog.h"

KNRunDialog::KNRunDialog(QWidget *parent) :
    QDialog(parent),
    m_hint(new QLabel(this)),
    m_command(new QComboBox(this)),
    m_run(new QPushButton(this)),
    m_save(new QPushButton(this)),
    m_cancel(new QPushButton(this)),
    m_select(new QPushButton("...", this)),
    m_actionEdit(new KNActionEdit(this))
{
    //Construct the widget.
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(layout);
    //Add widgets.
    layout->addWidget(m_hint, 0, Qt::AlignLeft);
    QBoxLayout *commandLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addLayout(commandLayout);
    commandLayout->addWidget(m_command, 1);
    commandLayout->addWidget(m_select);
    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addLayout(buttonLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_run);
    buttonLayout->addWidget(m_save);
    buttonLayout->addWidget(m_cancel);
    //Update the layout height.
    setFixedHeight(layout->sizeHint().height());
    resize(layout->sizeHint().width() << 1, height());
    //Configure the widget.
    m_command->setEditable(true);
    m_command->setMaxCount(127);
    //Link buttons.
    connect(m_select, &QPushButton::clicked, this, &KNRunDialog::onSelect);
    connect(m_run, &QPushButton::clicked, this, &KNRunDialog::onRun);
    connect(m_save, &QPushButton::clicked, this, &KNRunDialog::onSaveAction);
    connect(m_cancel, &QPushButton::clicked, this, &KNRunDialog::close);
    //Add translator.
    knUi->addTranslate(this, &KNRunDialog::retranslate);
}

void KNRunDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    //Set focus on line edit.
    m_command->setFocus();
    m_command->lineEdit()->selectAll();
}

void KNRunDialog::retranslate()
{
    setWindowTitle(tr("Run..."));
    //Update the text.
    m_hint->setText(tr("The program to run"));
    //Update command text.
    m_run->setText(tr("Run"));
    m_save->setText(tr("Save..."));
    m_cancel->setText(tr("Cancel"));
}

void KNRunDialog::onSelect()
{
    QString commandPath =
            QFileDialog::getOpenFileName(this, tr("Open Executable File"));
    //Check the command path.
    if(!commandPath.isEmpty())
    {
        m_command->setCurrentText(QDir::toNativeSeparators(commandPath));
    }
    //Set focus to command.
    m_command->setFocus();
}

void KNRunDialog::onRun()
{
    //Close the window first.
    close();
    //Run the command.
    KNUtil::openLocalFile(m_command->currentText());
}

void KNRunDialog::onSaveAction()
{
    if(m_command->currentText().isEmpty())
    {
        return;
    }
    //Clear the editor.
    m_actionEdit->clear();
    //Run the dialog.
    m_actionEdit->exec();
    //Fetch the action status.
    if(m_actionEdit->result() == QDialog::Accepted)
    {
        QAction *action = new QAction(parent());
        action->setText(m_actionEdit->actionName());
        action->setShortcut(m_actionEdit->actionKeySequence());
        //Add action to menu.
        emit requireAddAction(action, m_command->currentText());
        //Close the window.
        close();
    }
}
