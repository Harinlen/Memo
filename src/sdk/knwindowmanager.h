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
/*!
 * \brief The KNWindowModel class is the internal tab bar model for the window
 * manager. This class should be only used inside the window manager.
 */
class KNWindowModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNWindowModel object.
     * \param tabBar The tab bar widget pointer.
     * \param manager The file manager pointer.
     * \param parent The parent widget pointer.
     */
    explicit KNWindowModel(KNTabBar *tabBar,
                           KNFileManager *manager, QObject *parent = nullptr);

    /*!
     * \brief Reimplemented from QAbstractTableModel::rowCount().
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /*!
     * \brief Reimplemented from QAbstractTableModel::columnCount().
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /*!
     * \brief Reimplemented from QAbstractTableModel::data().
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /*!
     * \brief Reimplemented from QAbstractTableModel::headerData().
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /*!
     * \brief Reimplemented from QAbstractListModel::removeRows().
     */
    bool removeRows(int position,
                    int rows,
                    const QModelIndex &index = QModelIndex()) override;

    /*!
     * \brief Empty the entire model data.
     */
    void clearModel();

    /*!
     * \brief Synchronize the model data from the tab manager.
     */
    void synchronizeModel();

    /*!
     * \brief Get the tab ID from the row of the model.
     * \param row The row of the model.
     * \return The tab ID of the specific row.
     */
    int tabId(int row) const;

    /*!
     * \brief Reimplemented from QAbstractTableModel::sort().
     */
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    /*!
     * \brief Extract the tab ID list from the model.
     * \return The tab ID list.
     */
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
/*!
 * \brief The KNWindowManager class provides a window manager for all the
 * existed tabs.
 */
class KNWindowManager : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNWindowManager dialog.
     * \param tabBar The tab bar widget manager.
     * \param parent The parent widget.
     */
    explicit KNWindowManager(KNTabBar *tabBar,
                             QWidget *parent = nullptr);

signals:

protected:
    /*!
     * \brief Reimplemented from QDialog::showEvent().
     */
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
