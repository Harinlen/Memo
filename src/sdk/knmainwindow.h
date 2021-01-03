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
#ifndef KNMAINWINDOW_H
#define KNMAINWINDOW_H

#include <QMainWindow>

class KNFileManager;
class KNStatusBar;
/*!
 * \brief The KNMainWindow class provides the main window of the Memo. We are
 * aiming to rebuild the Notepad++ into a mordern UI like Sublime Text or
 * TextMate across all platforms. All the widgets will use the standard widget
 * to construct, except the standard dialogs.
 */
class KNMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNMainWindow widget.
     * \param parent The parent widget.
     */
    explicit KNMainWindow(QWidget *parent = nullptr);

    /*!
     * \brief Initalize the main window.
     */
    void initalize();

signals:

protected:
    /*!
     * \brief Reimplemented from QMainWindow::closeEvent().
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    void retranslate();
    void updateTitle(const QString &fileTitle);
    void onEditorChanged();

private:
    QString m_title;
    QToolBar *m_toolbar;
    KNFileManager *m_fileManager;
    KNStatusBar *m_statusBar;
};

#endif // KNMAINWINDOW_H
