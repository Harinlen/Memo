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
#ifndef KNWINDOWSMENU_H
#define KNWINDOWSMENU_H

#include <QMenu>

class KNTabBar;
class KNWindowManager;
/*!
 * \brief The KNWindowsMenu class provides the menu of window, or actually tab
 * management. Except for the last window manager, all the other function seems
 * to be the same as the Tab menu in the View menu.
 */
class KNWindowsMenu : public QMenu
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNWindowsMenu widget.
     * \param tabBar The tab bar widget to be used for management.
     * \param parent The parent widget.
     */
    explicit KNWindowsMenu(KNTabBar *tabBar, QWidget *parent = nullptr);

signals:

protected:
    /*!
     * \brief Reimplemented from QMenu::showEvent().
     */
    void showEvent(QShowEvent *event);

private slots:
    void retranslate();
    void onShowTab(int tabId);

private:
    enum WindowMenuItems
    {
        Window1,
        Window2,
        Window3,
        Window4,
        Window5,
        Window6,
        Window7,
        Window8,
        Window9,
        Window10,
        WindowManager,
        WindowMenuItemCount
    };

    QAction *m_menuItems[WindowMenuItemCount];
    KNTabBar *m_tabBar;
    KNWindowManager *m_manager;
};

#endif // KNWINDOWSMENU_H
