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
#include <QBoxLayout>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextBlock>

#include "knglobal.h"
#include "knuimanager.h"

#include "kntoolhashinput.h"

KNToolHashInput::KNToolHashInput(HashFunc hashFunc, QWidget *parent) :
    QDialog(parent),
    m_hashFunc(hashFunc),
    m_copy(new QPushButton(this)),
    m_close(new QPushButton(this)),
    m_lineCases(new QCheckBox(this)),
    m_buffer(new QPlainTextEdit(this)),
    m_result(new QPlainTextEdit(this))
{
    //Set properties.
    setFixedSize(knUi->size(540, 370));
    //Update the editor.
    m_copy->setFixedSize(knUi->size(131, 21));
    m_close->setFixedSize(knUi->size(88, 21));
    //Configure the editor.
    m_buffer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_buffer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_result->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_result->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QPalette pal = m_result->palette();
    pal.setColor(QPalette::Base, palette().color(QPalette::Window));
    m_result->setPalette(pal);
    //Update the layout.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(mainLayout);
    //Show the widget.
    mainLayout->addWidget(m_lineCases);
    mainLayout->addWidget(m_buffer);
    mainLayout->addWidget(m_result);
    mainLayout->addWidget(m_copy, 0, Qt::AlignRight);
    mainLayout->addWidget(m_close, 0, Qt::AlignCenter);
    //Configure the text.
    QFont targetFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_buffer->setFont(targetFont);
    m_result->setFont(targetFont);
    m_result->setReadOnly(true);
    //Link the button.
    connect(m_copy, &QPushButton::clicked, this, &KNToolHashInput::onCopyResult);
    connect(m_close, &QPushButton::clicked, this, &KNToolHashInput::close);
    connect(m_lineCases, &QCheckBox::stateChanged, this, &KNToolHashInput::onBufferChanged);
    //Link the UI.
    knUi->addTranslate(this, &KNToolHashInput::retranslate);
    connect(m_buffer->document(), &QTextDocument::contentsChanged,
            this, &KNToolHashInput::onBufferChanged);
}

void KNToolHashInput::retranslate()
{
    setWindowTitle(tr("Generate digest"));
    m_lineCases->setText(tr("Treat each line as a seperate string"));
    m_copy->setText(tr("Copy to Clipboard"));
    m_close->setText(tr("Close"));
}

void KNToolHashInput::onBufferChanged()
{
    if(m_lineCases->isChecked())
    {
        auto doc = m_buffer->document();
        QStringList result;
        result.reserve(doc->blockCount());
        for(auto i=0; i<doc->blockCount(); ++i)
        {
            result.append(m_hashFunc(doc->findBlockByNumber(i).text().toUtf8()));
        }
        m_result->setPlainText(result.join("\n"));
    }
    else
    {
        //Send all the content to hash function.
        m_result->setPlainText(m_hashFunc(
                                   m_buffer->toPlainText().toUtf8()));
    }
}

void KNToolHashInput::onCopyResult()
{
    //Copy the result.
    knGlobal->copyText(m_result->toPlainText());
}
