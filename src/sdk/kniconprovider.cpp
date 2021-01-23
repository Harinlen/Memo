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
#include "kniconprovider.h"

KNIconProvider::KNIconProvider() :
    QFileIconProvider(),
    m_folderIcon(QIcon(":/icons/project_folder_close.png")),
    m_fileIcon(QIcon(":/icons/project_file.png"))
{
}

#if QT_VERSION_MAJOR > 5
QIcon KNIconProvider::icon(QAbstractFileIconProvider::IconType type) const
{
    return type == File ? m_fileIcon : m_folderIcon;
}
#endif

QIcon KNIconProvider::icon(const QFileInfo &info) const
{
    return info.isDir() ? m_folderIcon : m_fileIcon;
}
