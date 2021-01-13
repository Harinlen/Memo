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
#ifndef KNFILEMANAGER_H
#define KNFILEMANAGER_H

#include <QWidget>

class QStackedWidget;
class QMenu;
class KNTextEditor;
class KNTabBar;
class KNEditMenu;
class KNSearchMenu;
class KNViewMenu;
class KNCodecMenu;
class KNToolMenu;
class KNWindowsMenu;
class KNTabSwitcher;
class KNRecentFileRecorder;
/*!
 * \brief The KNFileManager class is the major widget of the main window. It
 * manages all the files and their backends.
 */
class KNFileManager : public QWidget
{
    Q_OBJECT
public:
    enum MenuItems
    {
        New,
        Open,
        ShowInExplorer,
        OpenWithDefault,
        ReloadFile,
        Save,
        SaveAs,
        SaveACopyAs,
        SaveAll,
        Rename,
        Close,
        CloseAll,
        CloseAllElse,
        CloseAllLeft,
        CloseAllRight,
        CloseAllSaved,
        LoadSession,
        SaveSession,
        Print,
        PrintNow,
        Exit,
        MenuItemCount
    };

    /*!
     * \brief Construct a KNFileManager widget.
     * \param parent The parent widget.
     */
    explicit KNFileManager(QWidget *parent = nullptr);

    /*!
     * \brief Get the current window title to be used.
     * \return The current tab title.
     */
    QString currentWindowTitle() const;

    /*!
     * \brief Get the file manager menu.
     * \return The menu pointer.
     */
    QMenu *fileMenu() const;

    /*!
     * \brief Get the file menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *fileMenuItem(int index);

    /*!
     * \brief Get the edit menu.
     * \return The menu pointer.
     */
    QMenu *editMenu() const;

    /*!
     * \brief Get the edit menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *editMenuItem(int index);

    /*!
     * \brief Get the search menu.
     * \return The menu pointer.
     */
    QMenu *searchMenu() const;

    /*!
     * \brief Get the search menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *searchMenuItem(int index);

    /*!
     * \brief Get the current text editor.
     * \return The editor widget pointer.
     */
    KNTextEditor *currentEditor() const;

    /*!
     * \brief Get an existed editor based on the title or the file.
     * \param pathOrTitle The path or the title of the editor.
     * \return The text editor pointer.
     */
    KNTextEditor *locateEditor(const QString &pathOrTitle);

    /*!
     * \brief Get all the editor pointers.
     * \return The editor pointer list.
     */
    QVector<KNTextEditor *> allEditors() const;

    /*!
     * \brief Get the editor at a specific index.
     * \param index The editor id.
     * \return The editor pointer.
     */
    KNTextEditor *editorAt(int index) const;

    /*!
     * \brief Get the view menu.
     * \return The menu pointer.
     */
    QMenu *viewMenu() const;

    /*!
     * \brief Get the view menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *viewMenuItem(int index);

    /*!
     * \brief Get the codec menu.
     * \return The menu pointer.
     */
    QMenu *codecMenu() const;

    /*!
     * \brief Get the tool menu.
     * \return The menu pointer.
     */
    QMenu *toolMenu() const;

    /*!
     * \brief Get the window menu.
     * \return The menu pointer.
     */
    QMenu *windowsMenu() const;

signals:
    void titleChange(const QString &title);
    void editorChanged();

public slots:
    void createTab();
    void openTabs();
    void save();
    void saveAs();
    void saveCopyAs();
    void saveAll();
    void closeTab();
    bool closeAllTab();
    void closeAllLeft();
    void closeAllRight();
    void closeAllSaved();
    void setCurrentTab(int tabId);
    void setFocusOnEditor();
    void showCodecDialog();

private slots:
    void retranslate();
    void onShowSwitcher();
    void onShowEditor(KNTextEditor *editor);
    void onReloadCurrent();
    void onTabChange(int index);
    void onEditorTitleChange();
    void onEditorModified(bool modified);
    void onEditorReadOnly(bool isReadOnly);
    bool closeEditor(int editorId);
    void printCurrent();
    void printUseDefault();
    int createFileTab(const QString &filePath);
    void openFile(const QString &filePath);
    void openFiles(const QStringList &filePaths);
    void showCurrentSummary();
    void onReloadUseCodec(const QByteArray &codecName);
    void onSetCodec(const QByteArray &codecName);
    void onQuickSearch(const QString &keywords, Qt::CaseSensitivity cs,
                       int position);

private:
    void removeEditorAndTab(int index);
    void ensureEmptyTab();
    bool closeTabRange(int start, int length);
    void linkEditor(KNTextEditor *editor);
    void updateFileItemsEnabled(bool isSaved);
    bool isEditorNewEmpty(KNTextEditor *editor) const;
    int createEditorTab(KNTextEditor *editor);
    bool saveEditor(KNTextEditor *editor);
    bool saveAsEditor(KNTextEditor *editor);
    void printEditor(KNTextEditor *editor);
    enum SubMenus
    {
        CloseMore,
        SubMenuCount
    };

    QMenu *m_menu;
    KNEditMenu *m_editMenu;
    KNSearchMenu *m_searchMenu;
    KNViewMenu *m_viewMenu;
    KNCodecMenu *m_codecMenu;
    KNToolMenu *m_toolMenu;
    KNRecentFileRecorder *m_recent;
    QMenu *m_subMenus[SubMenuCount];
    QAction *m_menuItems[MenuItemCount];
    KNTabBar *m_tabBar;
    KNWindowsMenu *m_windowsMenu;
    QStackedWidget *m_editorPanel;
    KNTabSwitcher *m_tabSwitcher;

    quint32 m_newCounter;
};

#endif // KNFILEMANAGER_H
