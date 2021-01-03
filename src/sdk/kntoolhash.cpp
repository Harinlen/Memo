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
#include <QCryptographicHash>

#include "knfilemanager.h"
#include "kntexteditor.h"
#include "kntoolhashinput.h"
#include "kntoolhashfile.h"
#include "knuimanager.h"
#include "knglobal.h"

#include "kntoolhash.h"

KNToolHash::KNToolHash(int hashLength, HashFunc hashFunc,
                       KNFileManager *fileManager, QWidget *parent) :
    QMenu(parent),
    m_hashFunc(hashFunc),
    m_inputHash(new KNToolHashInput(hashFunc, parent)),
    m_fileHash(new KNToolHashFile(hashLength, hashFunc, parent)),
    m_fileManager(fileManager),
    m_generate(new QAction(this)),
    m_generateFromFiles(new QAction(this)),
    m_generateFromSelection(new QAction(this))
{
    //Add action to menu.
    addAction(m_generate);
    addAction(m_generateFromFiles);
    addAction(m_generateFromSelection);
    //Link translation.
    knUi->addTranslate(this, &KNToolHash::retranslate);
    //Link the target.
    connect(m_generate, &QAction::triggered,
            m_inputHash, &KNToolHashInput::exec);
    connect(m_generateFromFiles, &QAction::triggered,
            m_fileHash, &KNToolHashFile::exec);
    connect(m_generateFromSelection, &QAction::triggered,
            this, &KNToolHash::hashSelection);
}

void KNToolHash::retranslate()
{
    m_generate->setText(tr("Generate..."));
    m_generateFromFiles->setText(tr("Generate from files..."));
    m_generateFromSelection->setText(tr("Generate from selection into clipboard"));
}

void KNToolHash::hashSelection()
{
    //Get the selection from the current editor.
    auto editor = m_fileManager->currentEditor();
    if(editor)
    {
        //Check the selection text.
        QString selectedText = editor->selectedText();
        knGlobal->copyText(m_hashFunc(selectedText.toUtf8()));
    }
}

QString hashMd5(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

KNToolMd5::KNToolMd5(KNFileManager *fileManager, QWidget *parent) :
    KNToolHash(32, hashMd5, fileManager, parent)
{
    //Link translation.
    knUi->addTranslate(this, &KNToolMd5::retranslate);
}

void KNToolMd5::retranslate()
{
    setTitle(tr("MD5"));
}

QString hashSha256(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

KNToolSha256::KNToolSha256(KNFileManager *fileManager, QWidget *parent) :
    KNToolHash(64, hashSha256, fileManager, parent)
{
    //Link translation.
    knUi->addTranslate(this, &KNToolSha256::retranslate);
}

void KNToolSha256::retranslate()
{
    setTitle(tr("SHA-256"));
}
