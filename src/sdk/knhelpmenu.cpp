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
#include <QAction>
#include <QApplication>
#include <QMessageBox>

#include "knuimanager.h"

#include "knhelpmenu.h"

KNHelpMenu::KNHelpMenu(QWidget *parent) : QMenu(parent)
{
    //Construct all the actions.
    for(int i=0; i<HelpActionCount; ++i)
    {
        m_actions[i] = new QAction(this);
    }
    //Add actions to menu.
    addAction(m_actions[About]);
    addAction(m_actions[AboutQt]);
    //Link the actions.
    connect(m_actions[About], &QAction::triggered, [=]
    {
        QMessageBox msgBox;
        msgBox.setIconPixmap(QPixmap(":/memo.png").scaledToHeight(
                                 knUi->width(96),
                                 Qt::SmoothTransformation));
        QString content = QString("<p><h3><b>%1 %2</b></h3></p><p></p><p>").arg(
                    QApplication::applicationDisplayName(),
                    QApplication::applicationVersion());
        QString qtVer = QString("Qt %1.%2.%3").arg(
                    QString::number(QT_VERSION_MAJOR),
                    QString::number(QT_VERSION_MINOR),
                    QString::number(QT_VERSION_PATCH));
        content.append(tr("Based on %1").arg(qtVer));
        content.append("</p><p>");
        content.append(tr("Copyright 2020-2021 Kreogist Dev Team. All rights reserved."));
        content.append("</p><p>");
        content.append(tr("The program is provided and distributed under the HARERU Software License; "
                          "either version 1 of the license, or (at your option) any later version. "
                          "For more information, please see license file."));
        content.append("</p><p>");
        content.append(tr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE "
                          "WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."));
        content.append("</p>");
        msgBox.setWindowTitle(tr("About %1").arg(QApplication::applicationName()));
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(content);
        msgBox.exec();
    });
    connect(m_actions[AboutQt], &QAction::triggered,
            qApp, &QApplication::aboutQt);
    //Link the translator.
    knUi->addTranslate(this, &KNHelpMenu::retranslate);
}

void KNHelpMenu::retranslate()
{
    setTitle(tr("&Help"));
    m_actions[About]->setText(tr("About %1...").arg(QApplication::applicationName()));
    m_actions[AboutQt]->setText(tr("About Qt..."));
}
