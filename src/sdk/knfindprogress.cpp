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
#include <QProgressBar>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>

#include "knuimanager.h"

#include "knfindprogress.h"

KNFindProgress::KNFindProgress(QWidget *parent) :
    QDialog(parent),
    m_progress(new QProgressBar(this)),
    m_cancel(new QPushButton(this)),
    m_filePath(new QLabel(this))
{
    //Construct the layout.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(mainLayout);
    //Add widget to layout.
    mainLayout->addWidget(m_filePath);
    mainLayout->addWidget(m_progress);
    mainLayout->addWidget(m_cancel);
    //Update the text.
    knUi->addTranslate(this, &KNFindProgress::retranslate);
}

void KNFindProgress::setMaxCount(int count)
{
    m_progress->setRange(0, count);
}

void KNFindProgress::setFilePath(int id, const QString &path)
{
    //Update the id and lable.
    m_progress->setValue(id);
    m_filePath->setText(path);
}

void KNFindProgress::retranslate()
{
    //Update the button information.
    setWindowTitle(tr("Find In Files progress..."));
    m_cancel->setText(tr("Cancel"));
}
