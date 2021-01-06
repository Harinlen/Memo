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
#ifndef KNEDITMENU_H
#define KNEDITMENU_H

#include <QMenu>

class QDockWidget;
class KNTextEditor;
/*!
 * \brief The KNEditMenu class provides the edit menu of the main window. It is
 * able to connect to an text editor.
 */
class KNEditMenu : public QMenu
{
    Q_OBJECT
public:
    enum EditMenuItems
    {
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Delete,
        SelectAll,
        BeginEndSelect,
        ColunmMode,
        ColumnEditor,
        CharacterPanel,
        ClipboardHistory,
        SetReadOnly,
        ClearReadOnly,
        CopyFilePath,
        CopyFileName,
        CopyFileDir,
        IndentInc,
        IndentDec,
        CaseUpper,
        CaseLower,
        CaseProper,
        CaseReverse,
        CaseRandom,
        LineDuplicate,
        LineRemoveDuplicate,
        LineJoin,
        LineMoveUp,
        LineMoveDown,
        LineRemoveEmpty,
        LineRemoveBlank,
        LineSortLexAsc,
        LineSortIntAsc,
        LineSortDecCommaAsc,
        LineSortDecDotAsc,
        LineSortLexDec,
        LineSortIntDec,
        LineSortDecCommaDec,
        LineSortDecDotDec,
        TrimRight,
        TrimLeft,
        TrimAll,
        TrimEolToSpace,
        TrimAllAndEol,
        TrimTabToSpace,
        TrimSpaceToTab,
        TrimSpaceToTabLead,
        PasteHtml,
        PasteRtf,
        SelectOpen,
        SelectShowInExplorer,
        SelectSearchOnInternet,
        EditMenuItemCount
    };

    /*!
     * \brief Construct a KNEditMenu widget.
     * \param parent The parent widget.
     */
    explicit KNEditMenu(QWidget *parent = nullptr);

    /*!
     * \brief Set the managed text editor pointer.
     * \param editor The text editor pointer.
     */
    void setEditor(KNTextEditor *editor);

    /*!
     * \brief Get the edit menu action from the menu.
     * \param index The index of the action.
     * \return The action pointer.
     */
    QAction *menuItem(int index);

signals:

private slots:
    void retranslate();
    void onCopyAvailable(bool yes);
    void onReadOnlyChanged(bool isReadOnly);
    void editorCanPasteCheck();

    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();
    void onCopyFilePath();
    void onCopyFileName();
    void onCopyFileDir();
    void onIndentInc();
    void onIndentDec();
    void onToUpper();
    void onToLower();
    void onToProper();
    void onToReverse();
    void onToRandom();
    void onLineDuplicate();
    void onLineRemoveDuplicate();
    void onLineJoin();
    void onLineMoveUp();
    void onLineMoveDown();
    void onTrimEol();
    void onTrimEolAndSpace();
    void onTabToSpace();
    void onPasteHtml();
    void onPasteRtf();
    void onSelectOpen();
    void onSelectShowInExplorer();
    void onSelectNetSearch();
    void onSetReadOnly();
    void onClearReadOnly();
    void onInsertText(const QString &text);

private:
    enum SortMode
    {
        SortLexcial,
        SortInt,
        SortDoubleComma,
        SortDoubleDot
    };
    void sortLines(bool asc, int sortMode);
    enum TrimMode
    {
        TrimModeRight,
        TrimModeLeft,
        TrimModeBoth
    };
    void trimLines(int trimMode);
    void removeEmptyLines(bool blankCheck = false);

    enum EditSubMenus
    {
        CopyToClipboard,
        Indent,
        ConvertCase,
        LineOps,
        Comment,
        AutoComplete,
        EOLConvert,
        BlankOperations,
        PasteSpecial,
        OnSelection,
        EditSubMenuCount
    };

    QAction *m_menuItems[EditMenuItemCount];
    QMenu *m_subMenus[EditSubMenuCount];

    QVector<QMetaObject::Connection> m_connects;
    KNTextEditor *m_editor;
    QDockWidget *m_charPanel;
};

#endif // KNEDITMENU_H
