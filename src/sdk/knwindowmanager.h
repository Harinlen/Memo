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

class KNTabBar;
class KNFileManager;
class KNWindowModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit KNWindowModel(KNTabBar *tabBar, KNFileManager *manager, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /*!
     * \brief Reimplemented from QAbstractListModel::removeRows().
     */
    bool removeRows(int position,
                    int rows,
                    const QModelIndex &index = QModelIndex()) override;

    void clearModel();

    void synchronizeModel();

    int tabId(int row) const;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    QVector<int> tabIdList() const;

private slots:
    void retranslate();

private:
    struct WindowInfo
    {
        int tabId;
        int size;
        QString path;
        QString name;
    };
    QList<WindowInfo> m_windowInfo;
    QString m_sectionName[3];
    KNTabBar *m_tabBar;
    KNFileManager *m_manager;
};

class QTreeView;
class KNWindowManager : public QDialog
{
    Q_OBJECT
public:
    explicit KNWindowManager(KNTabBar *tabBar,
                             QWidget *parent = nullptr);

signals:

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void retranslate();
    void onSelectChange();
    void onActivate();
    void onSave();
    void onCloses();
    void onSort();

private:
    KNTabBar *m_tabBar;
    KNFileManager *m_fileManager;
    QTreeView *m_tabView;
    KNWindowModel *m_tabModel;
    QPushButton *m_activate, *m_save, *m_closes, *m_sort, *m_okay;
};

#endif // KNWINDOWMANAGER_H
