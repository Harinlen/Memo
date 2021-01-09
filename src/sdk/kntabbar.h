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
#ifndef KNTABBAR_H
#define KNTABBAR_H

#include <QIcon>

#include <QTabBar>

/*!
 * \brief The KNTabBar class provides the tab bar widget of the tab manager.
 */
class KNTabBar : public QTabBar
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNTabBar widget.
     * \param parent The parent widget pointer.
     */
    explicit KNTabBar(QWidget *parent = nullptr);

    /*!
     * \brief Create a new tab with a specifc name.
     * \return The tab index.
     */
    int createTab(const QString &name);

signals:

public slots:
    /*!
     * \brief Set the modified state of the tab.
     * \param index The tab index.
     * \param modified The editor is modified or not.
     */
    void setModifiedState(int index, bool modified);

    /*!
     * \brief Set the read only state of the tab.
     * \param index The tab index.
     */
    void setReadOnlyState(int index);

    /*!
     * \brief Show the next tab.
     */
    void goToNextTab();

    /*!
     * \brief Show the previous tab.
     */
    void goToPrevTab();

    /*!
     * \brief Move the current tab to the next tab position.
     */
    void moveForward();

    /*!
     * \brief Move the current tab to the previous tab position.
     */
    void moveBackward();

private slots:

private:
    QIcon m_iconSaved, m_iconUnsaved, m_iconReadOnly;
};

#endif // KNTABBAR_H
