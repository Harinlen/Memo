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
#ifndef KNFINDWINDOW_H
#define KNFINDWINDOW_H

#include <QThread>

#include "knfindengine.h"

#include <QDialog>

class QTabBar;
class QLabel;
class QGroupBox;
class QGridLayout;
class QComboBox;
class QCheckBox;
class QRadioButton;
class QSlider;
class QBoxLayout;
class KNTextEditor;
class KNConfigure;
class KNSearchResult;
class KNFindProgress;
/*!
 * \brief The KNFindWindow class provides the find dialog.
 */
class KNFindWindow : public QDialog
{
    Q_OBJECT
public:
    enum DialogMode
    {
        ModeFind,
        ModeReplace,
        ModeFindInFiles,
        ModeMark,
        DialogModes
    };

    /*!
     * \brief Construct a KNFindWindow dialog.
     * \param parent The parent widget.
     */
    explicit KNFindWindow(KNSearchResult *result,
                          QWidget *parent = nullptr);
    ~KNFindWindow();

signals:
    void requireStartSearch();

public slots:
    /*!
     * \brief Show the dialog in a specific mode.
     * \param mode The display mode.
     */
    void showMode(int mode);

    /*!
     * \brief Execute find next based on the current settings.
     */
    void findNext();

    /*!
     * \brief Execute find previous based on the current settings.
     */
    void findPrevious();

    /*!
     * \brief Set the target text as the text in search box.
     * \param text The target text.
     */
    void setSearchText(const QString &text);

protected:
    /*!
     * \brief Reimplemented from QDialog::resizeEvent().
     */
    void resizeEvent(QResizeEvent *event) override;

    /*!
     * \brief Reimplemented from QDialog::showEvent().
     */
    void showEvent(QShowEvent *event) override;

    /*!
     * \brief Reimplemented from QDialog::event().
     */
    bool event(QEvent *e) override;

private slots:
    void retranslate();
    void onTransSet();
    void onTabChanged(int tabIndex);
    void onFindNext();
    void onCount();
    void onFindInCurrentDoc();
    void onFindInAllDoc();
    void onReplace();
    void onReplaceAll();
    void onMarkAll();
    void onClearMarks();
    void onCancelSearchEngine();

private:
    enum Buttons
    {
        FindNext,
        Count,
        FindAll,
        FindAllOpen,
        Close,
        Replace,
        ReplaceAll,
        ReplaceAllOpen,
        FindFilesAll,
        FindFilesReplace,
        MarkAll,
        ClearMarks,
        ButtonCount
    };
    enum Labels
    {
        LabelFind,
        LabelReplace,
        LabelFilter,
        LabelCount
    };
    enum MatchOptions
    {
        OptionBookmarkLine,
        OptionPurge,
        OptionBackward,
        OptionMatchWhole,
        OptionMatchCase,
        OptionWrapAround,
        MatchOptionCount
    };
    void searchNext(bool revserse);
    inline QComboBox *generateBox();

    bool cursorMatches(const QTextCursor &tc, Qt::CaseSensitivity cs);
    KNFindEngine::SearchCache createSearchCache();
    QTextCursor performSearch(KNTextEditor *editor, const QTextCursor &tc,
                              bool backward = false);
    QTextCursor performSearch(KNTextEditor *editor, const QTextCursor &tc,
                              QTextDocument::FindFlags flags);
    static QTextCursor cacheSearch(QTextDocument *editor, const QTextCursor &tc,
                            const KNFindEngine::SearchCache &cache,
                            QTextDocument::FindFlags flags);
    QTextDocument::FindFlags getOneWaySearchFlags();
    QTextDocument::FindFlags getSearchFlags(bool backward);
    QRegularExpression getRegExp(const QString &exp);
    inline void setWidget(QWidget *widget, int row, int column,
                          int rowSpan, int columnSpan,
                          Qt::Alignment align = Qt::Alignment());
    void applyTransparent();
    void removeTransparent();
    void updateSelectBox();
    void updateOptionBox();
    KNConfigure *m_config;
    QGridLayout *m_layout;
    QTabBar *m_tabBar;
    QLabel *m_labels[LabelCount], *m_message;
    QComboBox *m_findText, *m_replaceText, *m_filters;
    QRadioButton *m_optionNormal, *m_optionExtend, *m_optionReg,
                 *m_transOnLose, *m_transAlways;
    QSlider *m_transValue;
    QGroupBox *m_selectBox, *m_searchMode, *m_transparent;
    QCheckBox *m_inSelection, *m_dotExtend, *m_transparentEnable,
              *m_matchOption[MatchOptionCount];
    QPushButton *m_buttons[ButtonCount];

    QThread m_engineThread;
    KNFindEngine *m_engine;
    KNFindProgress *m_progressWindow;
    KNSearchResult *m_resultWindow;
};

#endif // KNFINDWINDOW_H
