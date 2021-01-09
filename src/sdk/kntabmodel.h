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
#ifndef KNTABMODEL_H
#define KNTABMODEL_H

#include <QAbstractListModel>

class KNTextEditor;
/*!
 * \brief The KNTabModel class provides the editor tab model for quick
 * switching. When an editor is activating, creating or closing, it should
 * report to the tab model for maintaining the current tab states.
 */
class KNTabModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNTabModel object.
     * \param parent The parent object.
     */
    explicit KNTabModel(QObject *parent = nullptr);

    /*!
     * \brief Reimplemented from QAbstractListModel::rowCount().
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /*!
     * \brief Reimplemented from QAbstractListModel::data().
     */
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    /*!
     * \brief Reimplemented from QAbstractListModel::removeRows().
     */
    bool removeRows(int position,
                    int rows,
                    const QModelIndex &index = QModelIndex()) override;

    /*!
     * \brief Get the editor eidget pointer based on the row of the model item.
     * \param row The row of the model item.
     * \return The referenced editor pointer.
     */
    KNTextEditor *editorAt(int row);

signals:

public slots:
    /*!
     * \brief Add an editor to the model.
     * \param editor The editor pointer.
     */
    void addEditor(KNTextEditor *editor);

    /*!
     * \brief Remove an editor from the model.
     * \param editor The editor pointer.
     */
    void removeEditor(KNTextEditor *editor);

    /*!
     * \brief Move one editor pointer item to the top of the model, this
     * function should be called when an editor is activated.
     * \param editor The editor pointer.
     */
    void moveEditorToTop(KNTextEditor *editor);

private slots:

private:
    int editorIndex(KNTextEditor *editor);
    QList<KNTextEditor *> m_tabs;
};

#endif // KNTABMODEL_H
