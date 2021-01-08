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
#include <QPushButton>
#include <QBoxLayout>
#include <QListView>
#include <QTextCodec>
#include <QStringListModel>

#include "kntexteditor.h"
#include "knuimanager.h"

#include "kncodecdialog.h"

KNCodecDialog::KNCodecDialog(QWidget *parent) :
    QDialog(parent),
    m_label(new QLabel(this)),
    m_codecList(new QListView(this)),
    m_loadAs(new QPushButton(this)),
    m_saveAs(new QPushButton(this)),
    m_close(new QPushButton(this)),
    m_editor(nullptr)
{
    resize(knUi->size(579, 293));
    //Add widgets.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->addWidget(m_label);
    mainLayout->addWidget(m_codecList, 1);
    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout->addLayout(buttonLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_loadAs);
    buttonLayout->addWidget(m_saveAs);
    buttonLayout->addWidget(m_close);
    //Prepare the codec.
    QList<int> mibs = QTextCodec::availableMibs();
    std::sort(mibs.begin(), mibs.end());
    //Move all the positive mib to the begin of the list.
    foreach(int mib, mibs)
    {
        if (mib >= 0)
        {
            m_sortedMibs += mib;
        }
    }
    foreach(int mib, mibs)
    {
        if (mib < 0)
        {
            m_sortedMibs += mib;
        }
    }
    //Generate the encoding string list.
    QStringList encodings;
    foreach(int mib, m_sortedMibs)
    {
        QTextCodec *c = QTextCodec::codecForMib(mib);
        QString names = QString::fromLatin1(c->name());
        foreach (const QByteArray &alias, c->aliases())
        {
            names += QLatin1String(" / ") + QString::fromLatin1(alias);
        }
        encodings.append(names);
    }
    //Add encodings to list views.
    QStringListModel *model = new QStringListModel(encodings, this);
    m_codecList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_codecList->setModel(model);
    //Link the button.
    connect(m_loadAs, &QPushButton::clicked, this, &KNCodecDialog::onLoadAsCodec);
    connect(m_saveAs, &QPushButton::clicked, this, &KNCodecDialog::onSaveAsCodec);
    connect(m_close, &QPushButton::clicked, this, &KNCodecDialog::close);
    //Link translation.
    knUi->addTranslate(this, &KNCodecDialog::retranslate);
}

void KNCodecDialog::showDialog(KNTextEditor *editor)
{
    //Save the editor, show the dialog.
    m_editor = editor;
    //Update the dialog title.
    m_label->setText(tr("Select encoding for \"%1\"").arg(
                         m_editor->documentTitle()));
    //Get the codec name from the file.
    auto codec = QTextCodec::codecForName(m_editor->codecName().toLatin1());
    //Find the mib from the list.
    int mibIndex = m_sortedMibs.indexOf(codec->mibEnum());
    if(mibIndex != -1)
    {
        //Set the current index.
        m_codecList->setCurrentIndex(
                    m_codecList->model()->index(mibIndex, 0));
    }
    exec();
}

void KNCodecDialog::retranslate()
{
    //Update dialog title.
    setWindowTitle(tr("Text Encoding"));
    //Update the button.
    m_loadAs->setText(tr("Reload with Encoding"));
    m_saveAs->setText(tr("Save with Encoding"));
    m_close->setText(tr("Cancel"));
}

void KNCodecDialog::onLoadAsCodec()
{
    //Get the selected codec.
    auto index = m_codecList->currentIndex();
    if(index.isValid())
    {
        //Extract the mib.
        int selectedMib = m_sortedMibs.at(index.row());
        //Ask the editor to reload the document use the current encoding.
        m_editor->loadUseCodec(QTextCodec::codecForMib(selectedMib));
    }
}

void KNCodecDialog::onSaveAsCodec()
{
    //Get the selected codec.
    auto index = m_codecList->currentIndex();
    if(index.isValid())
    {
        //Extrat the mib.
        int selectedMib = m_sortedMibs.at(index.row());
        //Change the current codec to the file.
        auto codec = QTextCodec::codecForMib(selectedMib);
        //Set the codec of the file.
        m_editor->setCodecName(codec->name());
        m_editor->save();
        //Close the dialog.
        close();
    }
}
