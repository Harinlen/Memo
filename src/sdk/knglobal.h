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
#ifndef KNGLOBAL_H
#define KNGLOBAL_H

#include <QTextCharFormat>
#include <QPixmap>
#include <QFont>
#include <QObject>

/*!
 * \def knGlobal
 * A global pointer referring to the unique global object.
 */
#define knGlobal        (KNGlobal::instance())

/*!
 * \def knMainWindow
 * A global pointer referring to the unique main window object.
 */
#define knMainWindow    (KNGlobal::instance()->mainWindow())

namespace KNG
{
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
const Qt::KeyboardModifier META  = Qt::MetaModifier;
const Qt::KeyboardModifier SHIFT = Qt::ShiftModifier;
const Qt::KeyboardModifier CTRL  = Qt::ControlModifier;
const Qt::KeyboardModifier ALT   = Qt::AltModifier;
#endif
#ifdef Q_OS_MAC
const Qt::KeyboardModifier META  = Qt::ControlModifier,
const Qt::KeyboardModifier SHIFT = Qt::ShiftModifier,
const Qt::KeyboardModifier CTRL  = Qt::MetaModifier,
const Qt::KeyboardModifier ALT   = Qt::AltModifier
#endif
}

class KNConfigure;
class KNMainWindow;
/*!
 * \brief The KNGlobal class is a global instance which contains several public
 * function. \n
 * You should only construct this class once. If you construct the KNGlobal more
 * than once, the other instance will become no use.
 */
class KNGlobal : public QObject
{
    Q_OBJECT
public:
    enum Dirs
    {
        KreogistDir,
        ResourceDir,
        PluginDir,
        UserDir,
        UserResourceDir,
        UserLibraryDir,
        UserPluginDir,
        DirCount
    };
    ~KNGlobal();

    /*!
     * \brief Get the singleton instance of class KNGlobal.
     * \return The global instance of KNGlobal.
     */
    static KNGlobal *instance();

    /*!
     * \brief Initial the global class.
     * \param mainWindow The parent main window object pointer.
     */
    static void initial(KNMainWindow *parent = 0);

    /*!
     * \brief Get the global main window pointer.
     * \return The main window pointer.
     */
    KNMainWindow *mainWindow();

    /*!
     * \brief Start up the entire application.
     */
    void startUp();

    /*!
     * \brief Get the tab spacing counts.
     * \return The tab spacing count.
     */
    int tabSpacing() const;

    /*!
     * \brief Replace the tab as spacing.
     * \return If replacing tab using spaces, return true.
     */
    bool replaceTab() const;

    /*!
     * \brief Get the symbol display mode.
     * \return The text editor display mode.
     */
    int symbolDisplayMode() const;

    /*!
     * \brief Set the symbol display mode.
     * \param mode The symbol display mode.
     */
    void setSymbolDisplayMode(int mode);

    /*!
     * \brief Whether the text editor should wrap or not.
     * \return If the text editor should wrap, return true.
     */
    bool isWrap() const;

    /*!
     * \brief Set whether the editor should wrap the word.
     * \param wrap The word wrap mode of the editor.
     */
    void setWrap(bool wrap);

    /*!
     * \brief Copy a specific plain text to clipboard.
     * \param content The text to clipboard.
     */
    void copyText(const QString &content);

    /*!
     * \brief Get the default font for all the text editors.
     * \return The text editor font.
     */
    QFont editorDefaultFont() const;

    /*!
     * \brief Get the bookmark image.
     * \return The book mark image.
     */
    QPixmap bookmark() const;

    /*!
     * \brief Search URL the keywords in web browser.
     * \param keywords The keywords to search.
     */
    void searchOnline(const QString &keywords);

    /*!
     * \brief Get the cursor flash timer.
     * \return The cursor timer object pointer.
     */
    QTimer *cursorTimer() const;

    /*!
     * \brief Zoom all the editor in for 1 level.
     */
    void zoomIn();

    /*!
     * \brief Zoom all the editor out for 1 level.
     */
    void zoomOut();

    /*!
     * \brief Reset the zoom scalar.
     */
    void zoomReset();

    /*!
     * \brief Get the quick search char format.
     * \return The format of the search result.
     */
    QTextCharFormat quickSearchFormat() const;

    /*!
     * \brief Check whether the quick search display result is shown.
     * \return If the result should be marked, return true.
     */
    bool isSearchResultShown() const;

    /*!
     * \brief Set whether the quick search result should be display.
     * \param shown To mark the result, set this value to true.
     */
    void setSearchResultShown(bool shown);

signals:
    /*!
     * \brief When the display font is change, this signal is emitted.
     */
    void editorFontChange();

    /*!
     * \brief When the symbol display mode is changed, this signal is emitted.
     * \param mode The character display mode.
     */
    void editorDisplayModeChange(int mode);

    /*!
     * \brief When the editor wrap mode is changed, this signal is emitted.
     * \param wrap The word wrap mode.
     */
    void editorWrapModeChange(bool wrap);

    /*!
     * \brief When the editor search result is changed, this signal is emitted.
     * \param result If the result display is true, set this to true.
     */
    void editorResultDisplayChange(bool result);

private:
    explicit KNGlobal(KNMainWindow *parent = nullptr);
    //Disable the copy of the instance.
    KNGlobal(const KNGlobal &);
    KNGlobal(KNGlobal &&);
    static KNGlobal *ins;

    qreal zoomScalar() const;
    void setZoomScalar(const qreal &value);

    QString m_dirPath[DirCount];
    QString m_searchUrl[5];
    QPixmap m_bookmark;
    QTextCharFormat m_quickSearchFormat;
    KNMainWindow *m_mainWindow;
    KNConfigure *m_configure;
    QTimer *m_cursorFlash;
    QFont m_presetFont, m_scaledFont;
    int m_searchUrlIndex;
};

#endif // KNGLOBAL_H
