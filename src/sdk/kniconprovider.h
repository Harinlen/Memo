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

/*!
 * \brief The KNIconProvider class provides the icon for the file project panel.
 */
class KNIconProvider : public QFileIconProvider
{
public:
    /*!
     * \brief Construct a KNIconProvider object.
     */
    explicit KNIconProvider();

#if QT_VERSION_MAJOR > 5
    QIcon icon(QFileIconProvider::IconType type) const override;
#endif

    /*!
     * \brief Reimplemented from QFileIconProvider::icon().
     */
    QIcon icon(const QFileInfo &info) const override;

private:
    QIcon m_folderIcon, m_fileIcon;
};

#endif // KNICONPROVIDER_H
