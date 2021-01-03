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
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QPushButton>
#include <QTextBlock>

#include "knuimanager.h"
#include "kntexteditor.h"

#include "kngotowindow.h"

KNGotoWindow::KNGotoWindow(QWidget *parent) : QDialog(parent),
    m_line(new QRadioButton(this)),
    m_offset(new QRadioButton(this)),
    m_currentLabel(new QLabel(this)),
    m_maximumLabel(new QLabel(this)),
    m_targetLabel(new QLabel(this)),
    m_currentPos(new QLabel(this)),
    m_maximumPos(new QLabel(this)),
    m_position(new QLineEdit(this)),
    m_okay(new QPushButton(this)),
    m_cancel(new QPushButton(this))
{
    //Configure the layout.
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);
    //Set the auto exclusive
    m_line->setAutoExclusive(true);
    m_offset->setAutoExclusive(true);
    //Set default selection.
    m_line->setChecked(true);
    //Set default button.
    m_okay->setDefault(true);
    //Set the validator.
    m_position->setValidator(new QIntValidator(this));
    //Add widgets.
    mainLayout->addWidget(m_line, 0, 0, 1, 1);
    mainLayout->addWidget(m_offset, 0, 1, 1, 1);
    mainLayout->addWidget(m_currentLabel, 1, 0, 1, 1);
    mainLayout->addWidget(m_targetLabel, 2, 0, 1, 1);
    mainLayout->addWidget(m_maximumLabel, 3, 0, 1, 1);
    mainLayout->addWidget(m_currentPos, 1, 1, 1, 1);
    mainLayout->addWidget(m_position, 2, 1, 1, 1);
    mainLayout->addWidget(m_maximumPos, 3, 1, 1, 1);
    mainLayout->addWidget(m_okay, 2, 2, 1, 1);
    mainLayout->addWidget(m_cancel, 3, 2, 1, 1);
    //Link the translator.
    knUi->addTranslate(this, &KNGotoWindow::retranslate);
    //Link the buttons.
    connect(m_line, &QRadioButton::clicked, this, &KNGotoWindow::updateLabelInfo);
    connect(m_offset, &QRadioButton::clicked, this, &KNGotoWindow::updateLabelInfo);
    //Close result.
    connect(m_okay, &QPushButton::clicked, this, &KNGotoWindow::accept);
    connect(m_cancel, &QPushButton::clicked, this, &KNGotoWindow::reject);
}

bool KNGotoWindow::isLine() const
{
    return m_line->isChecked();
}

int KNGotoWindow::targetValue() const
{
    return m_position->text().toInt();
}

int KNGotoWindow::showGoto(KNTextEditor *editor)
{
    //Save the editor.
    m_editor = editor;
    //Update the current data.
    updateLabelInfo();
    //Clear the number box.
    m_position->clear();
    m_position->setFocus();
    //Show the dialog.
    return exec();
}

void KNGotoWindow::retranslate()
{
    setWindowTitle(tr("Go To"));
    //Update the label.
    m_line->setText(tr("&Line"));
    m_offset->setText(tr("&Offset"));
    //Update the label.
    m_currentLabel->setText(tr("You are here :"));
    m_targetLabel->setText(tr("You want to go to :"));
    m_maximumLabel->setText(tr("You can't go further than :"));
    //Update the okay.
    m_okay->setText(tr("Go"));
    m_cancel->setText(tr("I'm going nowhere"));
    //Update the window size.
    setFixedSize(layout()->sizeHint());
}

void KNGotoWindow::updateLabelInfo()
{
    //Check the editor information.
    if(!m_editor)
    {
        return;
    }
    //Extract variables.
    QTextCursor tc = m_editor->textCursor();
    auto doc = m_editor->document();
    //Check the current radio button.
    if(m_line->isChecked())
    {
        //Update the block information.
        m_currentPos->setText(QString::number(tc.block().blockNumber() + 1));
        m_maximumPos->setText(QString::number(doc->blockCount()));
    }
    else
    {
        //Update the position information.
        m_currentPos->setText(QString::number(tc.position()));
        //Update the maximum position information.
        m_maximumPos->setText(QString::number(doc->characterCount()));
    }
}
