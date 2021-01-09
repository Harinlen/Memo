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
#ifndef KNTABSWITCHER_H
#define KNTABSWITCHER_H

#include <QListView>

class KNTabModel;
class KNTextEditor;
/*!
 * \brief The KNTabSwitcher class provides the listview of the switch board
 * widget.
 */
class KNTabSwitcher : public QListView
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNTabSwitcher widget.
     * \param parent The parent widget.
     */
    explicit KNTabSwitcher(QWidget *parent = nullptr);

signals:
    /*!
     * \brief Require to show the editor.
     * \param editor The editor pointer.
     */
    void requireShowEditor(KNTextEditor *editor);

public slots:
    /*!
     * \brief Add an editor to the switching model.
     * \param editor The editor pointer.
     */
    void addEditor(KNTextEditor *editor);

    /*!
     * \brief Remove the editor from the switching panel.
     * \param editor The editor pointer.
     */
    void removeEditor(KNTextEditor *editor);

    /*!
     * \brief Move one editor to the top of the model.
     * \param editor The editor pointer.
     */
    void topEditor(KNTextEditor *editor);

protected:
    /*!
     * \brief Reimplemented from QListView::hideEvent().
     */
    void hideEvent(QHideEvent *event) override;

    /*!
     * \brief Reimplemented from QListView::focusOutEvent().
     */
    void focusOutEvent(QFocusEvent *event) override;

    /*!
     * \brief Reimplemented from QListView::keyPressEvent().
     */
    void keyPressEvent(QKeyEvent *event) override;

    /*!
     * \brief Reimplemented from QListView::keyReleaseEvent().
     */
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    KNTabModel *m_tabModel;
};

#endif // KNTABSWITCHER_H
