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
#ifndef KNWINDOWMANAGER_H
#define KNWINDOWMANAGER_H

#include <QDialog>

#include <QAbstractTableModel>

class KNWindowModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit KNWindowModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    struct WindowInfo
    {
        int tabId;
        int size;
        QString path;
        QString name;
    };
    QList<WindowInfo> m_windowInfo;
};

class QTreeView;
class KNTabBar;
class KNWindowManager : public QDialog
{
    Q_OBJECT
public:
    explicit KNWindowManager(KNTabBar *tabBar,
                             QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();

private:
    KNTabBar *m_tabBar;
    QTreeView *m_tabView;
    QPushButton *m_activate, *m_save, *m_closes, *m_sort, *m_okay;
};

#endif // KNWINDOWMANAGER_H
