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
#ifndef KNCHARPANEL_H
#define KNCHARPANEL_H

#include <QTreeView>

class QStandardItemModel;
class KNCharPanel : public QTreeView
{
    Q_OBJECT
public:
    explicit KNCharPanel(QWidget *parent = nullptr);

signals:
    void requireInsert(const QString &text);

private slots:
    void retranslate();
    void onActivate(const QModelIndex &index);

private:
    QStandardItemModel *m_model;
};

#endif // KNCHARPANEL_H
