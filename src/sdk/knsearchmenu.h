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
#ifndef KNSEARCHMENU_H
#define KNSEARCHMENU_H

#include <QMenu>

class QDockWidget;
class KNFindWindow;
class KNTextEditor;
class KNGotoWindow;
class KNSearchBar;
class KNSearchResult;
/*!
 * \brief The KNSearchMenu class provides the search menu among the tab manager.
 */
class KNSearchMenu : public QMenu
{
    Q_OBJECT
public:
    enum SearchMenuItems
    {
        Find,
        FindInFiles,
        FindNext,
        FindPrev,
        SelectAndFindNext,
        SelectAndFindPrev,
        FindVolatileNext,
        FindVolatilePrev,
        Replace,
        IncrementSearch,
        SearchResults,
        NextResult,
        PrevResult,
        GoTo,
        GoToMatch,
        SelectBetween,
        Mark,
        BookmarkToggle,
        BookmarkNext,
        BookmarkPrev,
        BookmarkClear,
        BookmarkCut,
        BookmarkCopy,
        BookmarkReplace,
        BookmarkRemoveAll,
        BookmarkRemoveLeft,
        BookmarkInverse,
        FindChars,
        SearchMenuItemCount
    };

    /*!
     * \brief Construct a KNSearchMenu widget.
     * \param parent The parent widget.
     */
    explicit KNSearchMenu(QWidget *parent = nullptr);

    /*!
     * \brief Set the target text editor.
     * \param editor The editor pointer.
     */
    void setEditor(KNTextEditor *editor);

    /*!
     * \brief Get the quick search bar.
     * \return The search bar widget.
     */
    KNSearchBar *searchBar() const;

    /*!
     * \brief Get the search menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *menuItem(int index);

signals:

private slots:
    void retranslate();

    void onFind();
    void onFindInFiles();
    void onGoto();
    void onSelectFindNext();
    void onSelectFindPrevious();
    void onReplace();
    void onMark();
    void onBookmarkToggle();
    void onBookmarkNext();
    void onBookmarkPrev();
    void onBookmarkClear();
    void onBookmarkCut();
    void onBookmarkCopy();
    void onBookmarkReplace();
    void onBookmarkRemove(bool reverse = false);
    void onBookmarkInverse();

private:
    enum SearchSubMenus
    {
        MarkAll,
        UnmarkAll,
        JumpUp,
        JumpDown,
        Bookmark,
        SearchSubMenuCount
    };

    KNSearchResult *m_searchResult;
    KNFindWindow *m_searchWindow;
    KNGotoWindow *m_gotoWindow;
    KNSearchBar *m_searchBar;
    KNTextEditor *m_editor;
    QAction *m_menuItems[SearchMenuItemCount];
    QMenu *m_subMenus[SearchSubMenuCount];
};

#endif // KNSEARCHMENU_H
