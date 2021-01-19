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
#ifndef KNRUNMENU_H
#define KNRUNMENU_H

#include <QHash>
#include <QJsonObject>

#include <QMenu>

class KNRunDialog;
class KNConfigure;
class KNRunMenu : public QMenu
{
    Q_OBJECT
public:
    explicit KNRunMenu(QWidget *parent = nullptr);

signals:

private slots:
    void retranslate();
    void onAddAction(QAction *action, QString commandLine);
    void onExecute();

private:
    enum RunMenuItems
    {
        Execute,
        RunMenuItemCount
    };
    KNRunDialog *m_runDialog;
    QAction *m_actions[RunMenuItemCount], *m_commandSeperator;
    QVector<QAction *> m_userActions;
    QHash<QAction *, QString> m_commandMap;
};

#endif // KNRUNMENU_H
