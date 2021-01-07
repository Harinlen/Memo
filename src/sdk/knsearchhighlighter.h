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
#ifndef KNSEARCHHIGHLIGHTER_H
#define KNSEARCHHIGHLIGHTER_H

#include "knsyntaxhighlighter.h"

class KNSearchHighlighter : public KNSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit KNSearchHighlighter(QObject *parent = nullptr);

    /*!
     * \brief Reimplemented from KNSyntaxHighlighter::hasCodeLevel().
     */
    bool hasCodeLevel() const override;

signals:

protected:
    void syntaxProcess(const QString &text, KNTextBlockData *data) override;

private:
    QTextCharFormat m_resultFormat, m_fileFormat, m_lineFormat;

};

#endif // KNSEARCHHIGHLIGHTER_H
