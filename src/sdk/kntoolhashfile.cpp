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
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextBlock>
#include <QDir>
#include <QFile>
#include <QFileDialog>

#include "knglobal.h"
#include "knuimanager.h"

#include "kntoolhashfile.h"

KNToolHashFile::KNToolHashFile(int length, HashFunc hashFunc, QWidget *parent) :
    QDialog(parent),
    m_hashFunc(hashFunc),
    m_copy(new QPushButton(this)),
    m_close(new QPushButton(this)),
    m_selectFile(new QPushButton(this)),
    m_buffer(new QPlainTextEdit(this)),
    m_result(new QPlainTextEdit(this)),
    m_length(length)
{
    //Set properties.
    setFixedSize(knUi->size(540, 370));
    //Update the button.
    m_selectFile->setFixedSize(knUi->size(196, 21));
    m_copy->setFixedSize(knUi->size(131, 21));
    m_close->setFixedSize(knUi->size(88, 21));
    //Configure the editor.
    m_buffer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_buffer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_result->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_result->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QPalette pal = m_result->palette();
    pal.setColor(QPalette::Base, palette().color(QPalette::Window));
    m_buffer->setPalette(pal);
    m_result->setPalette(pal);
    //Update the layout.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(mainLayout);
    //Show the widget.
    mainLayout->addWidget(m_selectFile);
    mainLayout->addWidget(m_buffer);
    mainLayout->addWidget(m_result);
    mainLayout->addWidget(m_copy, 0, Qt::AlignRight);
    mainLayout->addWidget(m_close, 0, Qt::AlignCenter);
    //Configure the text.
    QFont targetFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_buffer->setFont(targetFont);
    m_buffer->setReadOnly(true);
    m_result->setFont(targetFont);
    m_result->setReadOnly(true);
    //Link the button.
    connect(m_selectFile, &QPushButton::clicked, this, &KNToolHashFile::onSelectFiles);
    connect(m_copy, &QPushButton::clicked, this, &KNToolHashFile::onCopyResult);
    connect(m_close, &QPushButton::clicked, this, &KNToolHashFile::close);
    //Link the UI.
    knUi->addTranslate(this, &KNToolHashFile::retranslate);
}

void KNToolHashFile::retranslate()
{
    setWindowTitle(tr("Generate digest from file..."));
    m_selectFile->setText(tr("Choose files to generate digest..."));
    m_copy->setText(tr("Copy to Clipboard"));
    m_close->setText(tr("Close"));
}

void KNToolHashFile::onSelectFiles()
{
    //Select multiple files.
    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Open"));
    if(filePaths.isEmpty())
    {
        return;
    }
    //Update the buffer.
    QStringList result;
    result.reserve(filePaths.size());
    for(int i=0; i<filePaths.size(); ++i)
    {
        //Generate hash result from the file.
        QFileInfo fileInfo(filePaths.at(i));
        //Read the file and generate the MD5.
        QFile file(fileInfo.absoluteFilePath());
        QString resultBuf;
        if(file.open(QIODevice::ReadOnly))
        {
            resultBuf = m_hashFunc(file.readAll());
            file.close();
        }
        else
        {
            resultBuf = QString(m_length, '-');
        }
        resultBuf.append("  " + fileInfo.fileName());
        result.append(resultBuf);
        //Update the file path.
        filePaths[i] = QDir::toNativeSeparators(fileInfo.absoluteFilePath());
    }
    //Set the result.
    m_buffer->setPlainText(filePaths.join("\n"));
    m_result->setPlainText(result.join("\n"));
}

void KNToolHashFile::onCopyResult()
{
    //Copy the result.
    knGlobal->copyText(m_result->toPlainText());
}
