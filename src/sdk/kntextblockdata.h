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
#ifndef KNTEXTBLOCKDATA_H
#define KNTEXTBLOCKDATA_H

#include <QTextBlockUserData>

/*!
 * \brief The KNTextBlockData class provides all the data for the KNTextEdit to
 * use and support to do search, code folding and bookmarks.
 */
class KNTextBlockData : public QTextBlockUserData
{
public:
    KNTextBlockData() { }

    bool hasBookmark = false;
};

#endif // KNTEXTBLOCKDATA_H
