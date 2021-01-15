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
#ifndef KNVIEWMENU_H
#define KNVIEWMENU_H

#include <QMenu>

class QDockWidget;
class KNDocumentMap;
class KNTextEditor;
/*!
 * \brief The KNViewMenu class provides the view menu of the application.
 */
class KNViewMenu : public QMenu
{
    Q_OBJECT
public:
    enum ViewMenuItems
    {
        AlwaysOnTop,
        FullScreen,
        WordWrap,
        FoldAll,
        UnfoldAll,
        CollapseCurrentLevel,
        UncollapseCurrentLevel,
        Summary,
        FolderAsWorkspace,
        DocumentMap,
        FunctionList,
        TextDirectionRTL,
        TextDirectionLTR,
        Monitoring,
        ShowWhiteSpace,
        ShowEOF,
        ShowAllChars,
        ZoomIn,
        ZoomOut,
        ZoomReset,
        TabNo1,
        TabNo2,
        TabNo3,
        TabNo4,
        TabNo5,
        TabNo6,
        TabNo7,
        TabNo8,
        TabNo9,
        TabNext,
        TabPrev,
        TabMoveForward,
        TabMoveBackward,
        ViewMenuItemCount
    };

    /*!
     * \brief Construct a KNViewMenu widget.
     * \param parent The parent widget.
     */
    explicit KNViewMenu(QWidget *parent = nullptr);

    /*!
     * \brief Get the view menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *menuItem(int index);

    /*!
     * \brief Set the target text editor.
     * \param editor The editor pointer.
     */
    void setEditor(KNTextEditor *editor);

signals:
    /*!
     * \brief Request to show a specific document tab.
     * \param tabIndex The index of the tab.
     */
    void requireToTab(int tabIndex);

    /*!
     * \brief Request to move to next tab.
     */
    void requireToNextTab();

    /*!
     * \brief Request to move to previous tab.
     */
    void requireToPrevTab();

    /*!
     * \brief Request to move the current tab forward.
     */
    void requireMoveForward();

    /*!
     * \brief Request to move the current tab backward.
     */
    void requireMoveBackward();

    /*!
     * \brief Request to show the summary of the current document.
     */
    void requireToShowSummary();

private slots:
    void retranslate();
    void onAlwaysOnTopToggle(bool checked);
    void onFullScreenToggle(bool checked);
    void onWordWrap(bool checked);
    void onShowWhitespace(bool checked);
    void onShowEof(bool checked);
    void onShowAll(bool checked);

private:
    enum ViewSubMenus
    {
        ShowSymbol,
        Zoom,
        Tab,
        CollapseLevel,
        UncollapseLevel,
        ViewSubMenuCount
    };

    QAction *m_menuItems[ViewMenuItemCount];
    QMenu *m_subMenus[ViewSubMenuCount];
    QDockWidget *m_mapDockWidget;
    KNDocumentMap *m_docMap;
};

#endif // KNVIEWMENU_H
