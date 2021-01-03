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
#ifndef KNUTIL_H
#define KNUTIL_H

#include <QString>
#include <QColor>
#include <QDateTime>
#include <QStringList>
#include <QUrl>

typedef QString (*HashFunc)(const QByteArray &);

/*!
 * \brief The KNUtil class cannot be construct. It will only used to provides
 * you several public static functions.
 */
class KNUtil
{
public:
    /*!
     * \brief Parse a color from a string. The format of the string can be: \n
     *  * #RGB
     *  * #RRGGBB
     *  * #RRGGBBAA
     * All the R, G, B and A above can be a hex number range 0-F.
     * \param data The color data string.
     * \return A QColor parsed from the string.
     */
    static QColor parseColor(const QString &data);

    /*!
     * \brief Simplified a file path. Sometimes the file path may be like:
     * /usr/bin//a, or /usr/bin/../bin/a. Both these path will be simplified to
     * /usr/bin/a.
     * \param path The file path.
     * \return The simplified file path.
     */
    static QString simplifiedPath(const QString &path);

    /*!
     * \brief This function will try to make the directory valid.
     * \param path The prefer path.
     * \return The simplified path of the prefer directory.
     */
    static QString ensureDirValid(const QString &path);

    /*!
     * \brief Use the system default file association to open a local file.
     * \param filePath The file path.
     */
    static void openLocalFile(const QString &filePath);

    /*!
     * \brief Show one file in graphical shell. Tried to locate that file in the
     * graphics user interface.
     * \param filePath The file path.
     */
    static void showInGraphicalShell(const QString &filePath);

private:
    KNUtil();
    KNUtil(const KNUtil &);
    KNUtil(KNUtil &&);
};

#endif // KNUTIL_H
