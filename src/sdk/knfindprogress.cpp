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
#include <QAction>
#include <QCloseEvent>

#include "knuimanager.h"

#include "knfindprogress.h"

KNFindProgress::KNFindProgress(QWidget *parent) :
    QDialog(parent),
    m_progress(new QProgressBar(this)),
    m_cancel(new QPushButton(this)),
    m_filePath(new QLabel(this)),
    m_closeFlag(false)
{
    setWindowFlags(windowFlags() & (~Qt::WindowCloseButtonHint));
    //Construct the layout.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(mainLayout);
    //Configure the progress bar.
    m_progress->setTextVisible(false);
    //Add widget to layout.
    mainLayout->addWidget(m_filePath);
    mainLayout->addWidget(m_progress);
    mainLayout->addWidget(m_cancel, 0, Qt::AlignHCenter);
    //Link buttons.
    connect(m_cancel, &QPushButton::clicked, this, &KNFindProgress::quitSearch);
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

void KNFindProgress::engineClose()
{
    //Update the close flag.
    m_closeFlag = true;
    //Close the dialog.
    close();
}

void KNFindProgress::showEvent(QShowEvent *event)
{
    //Reset the close widget.
    QDialog::showEvent(event);
    //Clear the close flag.
    m_closeFlag = false;
}

void KNFindProgress::closeEvent(QCloseEvent *event)
{
    //Check the close flag.
    if(!m_closeFlag)
    {
        event->ignore();
        return;
    }
    //Continue to close.
    QDialog::closeEvent(event);
}

void KNFindProgress::retranslate()
{
    //Update the button information.
    setWindowTitle(tr("Find In Files progress..."));
    m_cancel->setText(tr("Cancel"));
    m_cancel->setFixedWidth(m_cancel->sizeHint().width());
}
