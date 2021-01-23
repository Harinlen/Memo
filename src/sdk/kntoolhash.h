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
#ifndef KNTOOLHASH_H
#define KNTOOLHASH_H

#include "knutil.h"

#include <QMenu>

class KNFileManager;
class KNToolHashFile;
class KNToolHashInput;
/*!
 * \brief The KNToolHash class provides the base of the tool menu.
 */
class KNToolHash : public QMenu
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNToolHash widget.
     * \param parent The parent widget.
     */
    explicit KNToolHash(int hashLength, HashFunc hashFunc,
                        KNFileManager *fileManager,
                        QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();
    void hashSelection();

private:
    HashFunc m_hashFunc;
    KNFileManager *m_fileManager;
    KNToolHashInput *m_inputHash;
    KNToolHashFile *m_fileHash;
    QAction *m_generate, *m_generateFromFiles, *m_generateFromSelection;
};

/*!
 * \brief The KNToolMd5 class provides the MD5 hash function as the tool.
 */
class KNToolMd5 : public KNToolHash
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNToolMd5 tool menu.
     * \param fileManager The file manager.
     * \param parent The parent widget.
     */
    explicit KNToolMd5(KNFileManager *fileManager, QWidget *parent = nullptr);

private slots:
    void retranslate();
};

/*!
 * \brief The KNToolSha256 class provides the MD5 hash function as the tool.
 */
class KNToolSha256 : public KNToolHash
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNToolSha256 tool menu.
     * \param fileManager The file manager.
     * \param parent The parent widget.
     */
    explicit KNToolSha256(KNFileManager *fileManager, QWidget *parent = nullptr);

private slots:
    void retranslate();
};

#endif // KNTOOLHASH_H
