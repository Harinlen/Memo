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
#ifndef KNLANGUAGEMODEL_H
#define KNLANGUAGEMODEL_H

#include "knsyntaxhighlighter.h"

class KNLanguageModel : public KNSyntaxHighlighter
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNLanguageModel object.
     * \param parent The parent widget.
     */
    explicit KNLanguageModel(QObject *parent = nullptr);

signals:

};

#endif // KNLANGUAGEMODEL_H
