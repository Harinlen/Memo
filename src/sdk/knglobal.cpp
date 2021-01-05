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
#include <QStandardPaths>
#include <QApplication>
#include <QClipboard>
#include <QUrl>
#include <QTimer>

#include "knuimanager.h"
#include "knconfigure.h"
#include "knconfiguremanager.h"
#include "knmainwindow.h"
#include "knutil.h"
#include "knsyntaxhighlighter.h"

#include "knglobal.h"

//Initial the instance pointer to null.
KNGlobal *KNGlobal::ins=nullptr;

KNGlobal::~KNGlobal()
{
}

KNGlobal *KNGlobal::instance()
{
    //Return the instance pointer.
    return ins;
}

void KNGlobal::initial(KNMainWindow *parent)
{
    //Check if the singleton instance variable is null. Set the pointer to this
    //object if this is the first constructed object.
    if(ins==nullptr)
    {
        ins=new KNGlobal(parent);
    }
}

KNGlobal::KNGlobal(KNMainWindow *parent) : QObject(parent),
    m_bookmark(QPixmap(":/bookmark.png")),
    m_mainWindow(parent),
    m_cursorFlash(new QTimer(this)),
    m_searchUrlIndex(0)
{
    //Geneate the configure manager.
    KNConfigureManager::initial(this);
    //Generate the UI manager.
    KNUiManager::initial(this);
    //Load the infrastructures.
    //Initial the paths.
    /*
     * Initial the default path of the dirs.
     * Kreogist Dir:
     *     Windows: My documents/Kreogist/
     *    Mac OS X: Documents/Kreogist/
     *       Linux: ~/.kreogist/
     * Resource Dir:
     *     Windows: Application path
     *    Mac OS X: Application path/../Resources
     *       Linux: ~/.kreogist/memo
     * User Data Dir:
     *     Windows: My documents/Kreogist/Memo
     *    Mac OS X: Documents/Kreogist/Memo
     *       Linux: ~/.kreogist/memo
     * Library Dir:
     *      $UserDataDir$/Library
     * Plugin Dir:
     *      $UserDataDir$/Plugins
     */
#ifdef Q_OS_WIN //No matter Win32/Win64
    m_dirPath[KreogistDir] = KNUtil::simplifiedPath(
                QStandardPaths::writableLocation(
                    QStandardPaths::DocumentsLocation) + "/Kreogist");
    m_dirPath[UserDir]=m_dirPath[KreogistDir]+"/Memo";
    m_dirPath[ResourceDir]=qApp->applicationDirPath();
#endif
#ifdef Q_OS_MACX
    m_dirPath[KreogistDir]=
                KNUtil::simplifiedPath(QStandardPaths::writableLocation(
                                           QStandardPaths::DocumentsLocation)
                                       +"/Kreogist");
    m_dirPath[UserDir]=m_dirPath[KreogistDir]+"/Memo";
    m_dirPath[ResourceDir]=
            KNUtil::simplifiedPath(qApp->applicationDirPath()+"/../Resources");
#endif
#ifdef Q_OS_LINUX
    m_dirPath[KreogistDir]=
            KNUtil::simplifiedPath(QStandardPaths::writableLocation(
                                       QStandardPaths::HomeLocation))
            + "/.kreogist";
    m_dirPath[UserDir]=m_dirPath[KreogistDir]+"/memo";
    m_dirPath[ResourceDir]="/usr/share/Kreogist/memo/";
#endif
    m_dirPath[UserResourceDir]=
            KNUtil::simplifiedPath(m_dirPath[UserDir]+"/Resources");
    m_dirPath[UserLibraryDir]=
            KNUtil::simplifiedPath(m_dirPath[UserDir]+"/Library");
    m_dirPath[UserPluginDir]=
            KNUtil::simplifiedPath(m_dirPath[UserDir]+"/Plugins");
    //Search URLs.
    m_searchUrl[0] = "https://www.baidu.com/s?wd=%1";
    //Configure the configure manager.
    //Set the configure folder path, and the path will not be changed.
    knConf->setFolderPath(m_dirPath[UserDir] + "/Configure",
                          m_dirPath[KreogistDir] + "/Account");
    //Configure the UI manager.
    // Configure the Fonts.
    knUi->loadFonts(m_dirPath[ResourceDir]+"/Fonts");
    // Configure the DPI
    knUi->setDpi(m_mainWindow->logicalDpiX(), m_mainWindow->logicalDpiY());
    // Update the main window size.
    m_mainWindow->setMinimumSize(knUi->size(800, 450));
    // Load all the themes.
    //Load the theme in theme folder. It's familiar with the language folder.
    knUi->loadThemes(m_dirPath[ResourceDir]+"/Theme");
    knUi->loadThemes(m_dirPath[UserResourceDir]+"/Theme");
    //Register the main window.
    knUi->addThemeWidget(m_mainWindow, "MainWindow");
    // Load the locale.
    knUi->loadLanguages(m_dirPath[ResourceDir]+"/Language");
    knUi->loadLanguages(m_dirPath[UserResourceDir]+"/Language");
    //Configure the flash timer.
    m_cursorFlash->setInterval(QApplication::cursorFlashTime() >> 1);
    QApplication::setCursorFlashTime(0);
}

qreal KNGlobal::zoomScalar() const
{
    return m_configure->data("ZoomParam", 1.0).toReal();
}

void KNGlobal::setZoomScalar(const qreal &value)
{
    m_configure->setData("ZoomParam", value);
    //Update the present font.
    m_scaledFont.setPointSize(m_presetFont.pointSize() * value);
    //Emit the font change signal.
    emit editorFontChange();
}

QTextCharFormat KNGlobal::quickSearchFormat() const
{
    return m_quickSearchFormat;
}

bool KNGlobal::isSearchResultShown() const
{
    return m_configure->data("ShowQuickSearchResult", false).toBool();
}

void KNGlobal::setSearchResultShown(bool shown)
{
    //Save the result.
    m_configure->setData("ShowQuickSearchResult", shown);
    //Emit the signal.
    emit editorResultDisplayChange(isSearchResultShown());
}

QTimer *KNGlobal::cursorTimer() const
{
    return m_cursorFlash;
}

void KNGlobal::zoomIn()
{
    //Update the scalar.
    qreal scalar = zoomScalar();
    if(scalar >= 3.0)
    {
        return;
    }
    //Update the scalar.
    scalar = qMin(3.0, scalar + 0.1);
    setZoomScalar(scalar);
}

void KNGlobal::zoomOut()
{
    //Update the scalar.
    qreal scalar = zoomScalar();
    if(scalar <= 0.5)
    {
        return;
    }
    //Update the scalar.
    scalar = qMax(0.5, scalar - 0.1);
    setZoomScalar(scalar);
}

void KNGlobal::zoomReset()
{
    //Reset the zoom scalar.
    setZoomScalar(1.0);
}

QPixmap KNGlobal::bookmark() const
{
    return m_bookmark;
}

void KNGlobal::searchOnline(const QString &keywords)
{
    //Generate the url.
    KNUtil::openLocalFile(m_searchUrl[m_searchUrlIndex].arg(
                              QUrl::toPercentEncoding(keywords)));
}

KNMainWindow *KNGlobal::mainWindow()
{
    return m_mainWindow;
}

void KNGlobal::startUp()
{
    //Get the global instance.
    m_configure = knConf->configure(KNConfigureManager::User);
    //Set the default font.
    m_presetFont = m_configure->data("DefaultFont",
                                     QFontDatabase::systemFont(QFontDatabase::FixedFont)).value<QFont>();
    m_scaledFont = m_presetFont;
    //Update the quick search format.
    m_quickSearchFormat.setBackground(QColor(155, 255, 155));
    // Initialize the main window.
    m_mainWindow->initalize();
    // Set the theme based on the settings.
    knUi->setTheme(0);
    knUi->setLanguage(0);
    // Show the main window.
    m_mainWindow->show();
    //Text cursor timer start.
    m_cursorFlash->start();
}

int KNGlobal::tabSpacing() const
{
    //Fetch the tab spacing data.
    return m_configure->data("TabSpacing", 4).toInt();
}

bool KNGlobal::replaceTab() const
{
    //Fetch the information.
    return m_configure->data("ReplaceTabAsSpace", true).toBool();
}

int KNGlobal::symbolDisplayMode() const
{
    //Fetch the information.
    return m_configure->data("SymbolDisplay", 0).toInt();
}

void KNGlobal::setSymbolDisplayMode(int mode)
{
    //Set the data.
    m_configure->setData("SymbolDisplay", mode);
    //Emit the display mode change signal.
    emit editorDisplayModeChange(mode);
}

bool KNGlobal::isWrap() const
{
    //Fetch the information.
    return m_configure->data("TextWrap", false).toBool();
}

void KNGlobal::setWrap(bool wrap)
{
    //Update the data.
    m_configure->setData("TextWrap", wrap);
    //Emit the signal for all the configure.
    emit editorWrapModeChange(wrap);
}

void KNGlobal::copyText(const QString &content)
{
    auto clipboard = QApplication::clipboard();
    //Set the text.
    clipboard->setText(content);
}

QFont KNGlobal::editorDefaultFont() const
{
    //Add font to the editor.
    return m_scaledFont;
}
