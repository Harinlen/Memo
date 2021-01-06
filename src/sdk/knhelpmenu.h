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
#ifndef KNHELPMENU_H
#define KNHELPMENU_H

#include <QMenu>

/*!
 * \brief The KNHelpMenu class provides the help menu of the application.
 */
class KNHelpMenu : public QMenu
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNHelpMenu widget.
     * \param parent The parent widget.
     */
    explicit KNHelpMenu(QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();

private:
    enum HelpActions
    {
        About,
        AboutQt,
        HelpActionCount
    };

    QAction *m_actions[HelpActionCount];
};

#endif // KNHELPMENU_H
