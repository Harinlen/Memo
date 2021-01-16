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
#ifndef KNICONPROVIDER_H
#define KNICONPROVIDER_H

#include <QFileIconProvider>

class KNIconProvider : public QFileIconProvider
{
public:
    explicit KNIconProvider();

    QIcon icon(QFileIconProvider::IconType type) const override;

    QIcon icon(const QFileInfo &info) const override;

private:
    QIcon m_folderIcon, m_fileIcon;
};

#endif // KNICONPROVIDER_H
