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
#include "knversion.h"

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
    //Set static application information.
    QString applicationName = "Memo";
    QApplication::setApplicationName(applicationName);
    QApplication::setApplicationVersion(APP_VERSION_STR);
#ifndef Q_OS_WIN
    QApplication::setApplicationDisplayName(applicationName);
#endif
    QApplication::setOrganizationName("Kreogist Dev Team");
    QApplication::setOrganizationDomain("http://kreogist.github.io/");
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
    //Initial suffix resource.
    m_suffixLists[ExtAll] << "*.*";
    m_suffixLists[ExtText] << "*.txt";
    m_suffixLists[ExtAda] << "*.ada" << "*.ads" << "*.adb";
    m_suffixLists[ExtAssembly] << "*.asm";
    m_suffixLists[ExtAbsSynNotOne] << "*.mib";
    m_suffixLists[ExtAsp] << "*.asp";
    m_suffixLists[ExtAutolt] << "*.au3";
    m_suffixLists[ExtAviSyn] << "*.avs" << "*.avsi";
    m_suffixLists[ExtBaanC] << "*.bc" << "*.cln";
    m_suffixLists[ExtUnix] << "*.bash" << "*.sh" << "*.bsh" << "*.csh" << "*.bash_profile" << "*.bashrc" << "*.profile";
    m_suffixLists[ExtBat] << "*.bat" << "*.cmd" << "*.nt";
    m_suffixLists[ExtBlitzBasic] << "*.bb";
    m_suffixLists[ExtC] << "*.c" << "*.lex";
    m_suffixLists[ExtCAMLan] << "*.ml" << "*.mli" << "*.sml" << "*.thy";
    m_suffixLists[ExtCMake] << "*.cmake";
    m_suffixLists[ExtCobol] << "*.cbl" << "*.cbd" << "*.cdb" << "*.cdc" << "*.cob" << "*.cpy" << "*.copy" << "*.lst";
    m_suffixLists[ExtCsound] << "*.orc" << "*.sco" << "*.csd";
    m_suffixLists[ExtCoffeeScript] << "*.coffee" << "*.litcoffee";
    m_suffixLists[ExtCpp] << "*.cpp" << "*.cxx" << "*.cc" << "*.h" << "*.hh" << "*.hpp" << "*.hxx" << "*.ino";
    m_suffixLists[ExtCSharp] << "*.cs";
    m_suffixLists[ExtCss] << "*.css";
    m_suffixLists[ExtD] << "*.d";
    m_suffixLists[ExtDiff] << "*.diff" << "*.patch";
    m_suffixLists[ExtErlang] << "*.erl" << "*.hrl";
    m_suffixLists[ExtEscript] << "*.src" << "*.em";
    m_suffixLists[ExtForth] << "*.forth";
    m_suffixLists[ExtFortranFree] << "*.f" << "*.for" << "*.f90" << "*.f95" << "*.f2k" << "*.f23";
    m_suffixLists[ExtFortranFix] << "*.f77";
    m_suffixLists[ExtFreeBasic] << "*.bas" << "*.bi";
    m_suffixLists[ExtHaskell] << "*.hs" << "*.lhs" << "*.las";
    m_suffixLists[ExtHtml] << "*.html" << "*.htm" << "*.shtml" << "*.shtm" << "*.xhtml" << "*.xht" << "*.hta";
    m_suffixLists[ExtMsIni] << "*.ini" << "*.inf" << "*.url" << "*.wer";
    m_suffixLists[ExtInnoSetup] << "*.iss";
    m_suffixLists[ExtIntelHex] << "*.hex";
    m_suffixLists[ExtJava] << "*.java";
    m_suffixLists[ExtJavaScript] << "*.js" << "*.jsm" << "*.jsx" << "*.ts" << "*.tsx";
    m_suffixLists[ExtJson] << "*.json";
    m_suffixLists[ExtJavaServer] << "*.jsp";
    m_suffixLists[ExtKiXart] << "*.kix";
    m_suffixLists[ExtLisp] << "*.lsp" << "*.lisp";
    m_suffixLists[ExtLatex] << "*.tex" << "*.sty";
    m_suffixLists[ExtLua] << "*.lua";
    m_suffixLists[ExtMakefile] << "*.mak" << "*.mk";
    m_suffixLists[ExtMatlab] << "*.m";
    m_suffixLists[ExtMmixal] << "*.mms";
    m_suffixLists[ExtNimrod] << "*.nim";
    m_suffixLists[ExtExtTab] << "*.tab" << "*.spf";
    m_suffixLists[ExtAscArt] << "*.nfo";
    m_suffixLists[ExtNullsoft] << "*.nsi" << "*.nsh";
    m_suffixLists[ExtOScript] << "*.osx";
    m_suffixLists[ExtObjectC] << "*.mm";
    m_suffixLists[ExtPascal] << "*.pas" << "*.pp" << "*.p" << "*.inc" << "*.lpr";
    m_suffixLists[ExtPerl] << "*.pl" << "*.pm" << "*.plx";
    m_suffixLists[ExtPhp] << "*.php" << "*.php3" << "*.php4" << "*.php5" << "*.php6" << "*.phps" << "*.phpt" << "*.phtml";
    m_suffixLists[ExtPostScript] << "*.ps";
    m_suffixLists[ExtPowerShell] << "*.ps1" << "*.psm1";
    m_suffixLists[ExtProperties] << "*.properties";
    m_suffixLists[ExtPureBasic] << "*.pb";
    m_suffixLists[ExtPython] << "*.py" << "*.pyw";
    m_suffixLists[ExtR] << "*.r" << "*.s" << "*.splus";
    m_suffixLists[ExtRebol] << "*.r2" << "*.r3" << "*.reb";
    m_suffixLists[ExtReg] << "*.reg";
    m_suffixLists[ExtResource] << "*.rc";
    m_suffixLists[ExtRuby] << "*.rb" << "*.rbw";
    m_suffixLists[ExtRust] << "*.rs";
    m_suffixLists[ExtScheme] << "*.scm" << "*.smd" << "*.ss";
    m_suffixLists[ExtSmallTalk] << "*.st";
    m_suffixLists[ExtSpice] << "*.scp" << "*.out";
    m_suffixLists[ExtSql] << "*.sql";
    m_suffixLists[ExtSysVerilog] << "*.sv" << "*.svh";
    m_suffixLists[ExtMotoS] << "*.mot" << "*.srec";
    m_suffixLists[ExtSwift] << "*.swift";
    m_suffixLists[ExtToolCmd] << "*.tcl";
    m_suffixLists[ExtTek] << "*.tek";
    m_suffixLists[ExtTex] << "*.tex";
    m_suffixLists[ExtVisualBasic] << "*.vb";
    m_suffixLists[ExtVBScript] << "*.vbs";
    m_suffixLists[ExtTxt2tags] << "*.t2t";
    m_suffixLists[ExtVerilog] << "*.v" << "*.vh";
    m_suffixLists[ExtVhdl] << "*.vhd" << "*.vhdl";
    m_suffixLists[ExtVisualProlog] << "*.pro" << "*.cl" << "*.i" << "*.pack" << "*.ph";
    m_suffixLists[ExtXml] << "*.xml" << "*.xaml" << "*.xsl" << "*.xslt" << "*.xsd" << "*.xul";
    m_suffixLists[ExtYaml] << "*.yml" << "*.yaml";
    //Link translator.
    knUi->addTranslate(this, &KNGlobal::retranslate);
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

QString KNGlobal::fileDialogSuffix() const
{
    return m_fileDialogSuffix;
}

QTextCharFormat KNGlobal::quickSearchFormat() const
{
    return m_quickSearchFormat;
}

QTextCharFormat KNGlobal::markFormat(int index) const
{
    return m_markFormat[index];
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

void KNGlobal::retranslate()
{
    //Update the file names.
    m_suffixNames[ExtAll]=tr("All Types");
    m_suffixNames[ExtText]=tr("Normal text file");
    m_suffixNames[ExtAda]=tr("Ada file");
    m_suffixNames[ExtAssembly]=tr("Assembly language source file");
    m_suffixNames[ExtAbsSynNotOne]=tr("Abstract Syntax Notation One file");
    m_suffixNames[ExtAsp]=tr("Active Server Pages script file");
    m_suffixNames[ExtAutolt]=tr("Autolt file");
    m_suffixNames[ExtAviSyn]=tr("AviSynth scripts files");
    m_suffixNames[ExtBaanC]=tr("BaanC file");
    m_suffixNames[ExtUnix]=tr("Unix script file");
    m_suffixNames[ExtBat]=tr("Windows Batch file");
    m_suffixNames[ExtBlitzBasic]=tr("BlitzBasic file");
    m_suffixNames[ExtC]=tr("C source file");
    m_suffixNames[ExtCAMLan]=tr("CAML / Categorical Abstract Machine Language file");
    m_suffixNames[ExtCMake]=tr("CMake file");
    m_suffixNames[ExtCobol]=tr("COBOL / COmmon Business Oriented Language file");
    m_suffixNames[ExtCsound]=tr("Csound file");
    m_suffixNames[ExtCoffeeScript]=tr("CoffeeScript file");
    m_suffixNames[ExtCpp]=tr("C++ source file");
    m_suffixNames[ExtCSharp]=tr("C# source file");
    m_suffixNames[ExtCss]=tr("CSS / Cascade Style Sheets file");
    m_suffixNames[ExtD]=tr("D programming language file");
    m_suffixNames[ExtDiff]=tr("Diff file");
    m_suffixNames[ExtErlang]=tr("Erlang file");
    m_suffixNames[ExtEscript]=tr("ESCRIPT file");
    m_suffixNames[ExtForth]=tr("Forth file");
    m_suffixNames[ExtFortranFree]=tr("Fortran free form source file");
    m_suffixNames[ExtFortranFix]=tr("Fortran fixed form source file");
    m_suffixNames[ExtFreeBasic]=tr("FreeBasic source file");
    m_suffixNames[ExtHaskell]=tr("Haskell source file");
    m_suffixNames[ExtHtml]=tr("HTML / Hyper Text Markup Language file");
    m_suffixNames[ExtMsIni]=tr("Microsoft Ini file");
    m_suffixNames[ExtInnoSetup]=tr("InnoSetup script");
    m_suffixNames[ExtIntelHex]=tr("Intel HEX binary data");
    m_suffixNames[ExtJava]=tr("Java source file");
    m_suffixNames[ExtJavaScript]=tr("JavaScript source file");
    m_suffixNames[ExtJson]=tr("JSON file");
    m_suffixNames[ExtJavaServer]=tr("JavaServer Pages script file");
    m_suffixNames[ExtKiXart]=tr("KiXtart file");
    m_suffixNames[ExtLisp]=tr("List Processing Language file");
    m_suffixNames[ExtLatex]=tr("LaTeX source file");
    m_suffixNames[ExtLua]=tr("Lua source file");
    m_suffixNames[ExtMakefile]=tr("Makefile");
    m_suffixNames[ExtMatlab]=tr("MATLAB / Matrix Laboratory");
    m_suffixNames[ExtMmixal]=tr("MMIXAL source file");
    m_suffixNames[ExtNimrod]=tr("Nimrod source file");
    m_suffixNames[ExtExtTab]=tr("Extended crontab source file");
    m_suffixNames[ExtAscArt]=tr("MSDOS Style/ASCII Art");
    m_suffixNames[ExtNullsoft]=tr("Nullsoft Scriptable Install System script file");
    m_suffixNames[ExtOScript]=tr("OScript source file");
    m_suffixNames[ExtObjectC]=tr("Objective-C source file");
    m_suffixNames[ExtPascal]=tr("Pascal source file");
    m_suffixNames[ExtPerl]=tr("Perl source file");
    m_suffixNames[ExtPhp]=tr("PHP Hypertext Preprocessor file");
    m_suffixNames[ExtPostScript]=tr("PostScript file");
    m_suffixNames[ExtPowerShell]=tr("Windows PowerShell file");
    m_suffixNames[ExtProperties]=tr("Properties file");
    m_suffixNames[ExtPureBasic]=tr("PureBasic file");
    m_suffixNames[ExtPython]=tr("Python file");
    m_suffixNames[ExtR]=tr("R programming language");
    m_suffixNames[ExtRebol]=tr("REBOL file");
    m_suffixNames[ExtReg]=tr("Windows Registry file");
    m_suffixNames[ExtResource]=tr("Windows Resource file");
    m_suffixNames[ExtRuby]=tr("Ruby file");
    m_suffixNames[ExtRust]=tr("Rust file");
    m_suffixNames[ExtScheme]=tr("Scheme file");
    m_suffixNames[ExtSmallTalk]=tr("Smalltalk file");
    m_suffixNames[ExtSpice]=tr("Spice file");
    m_suffixNames[ExtSql]=tr("SQL / Structured Query Language file");
    m_suffixNames[ExtSysVerilog]=tr("SystemVerilog file");
    m_suffixNames[ExtMotoS]=tr("Motorola S-Record binary data");
    m_suffixNames[ExtSwift]=tr("Swift file");
    m_suffixNames[ExtToolCmd]=tr("Tool Command Language file");
    m_suffixNames[ExtTek]=tr("Tektronix extended HEX binary data");
    m_suffixNames[ExtTex]=tr("TeX file");
    m_suffixNames[ExtVisualBasic]=tr("Visual Basic file");
    m_suffixNames[ExtVBScript]=tr("Visual Basic Script file");
    m_suffixNames[ExtTxt2tags]=tr("txt2tags file");
    m_suffixNames[ExtVerilog]=tr("Verilog file");
    m_suffixNames[ExtVhdl]=tr("VHDL / VHSIC Hardware Description Language file");
    m_suffixNames[ExtVisualProlog]=tr("Visual Prolog file");
    m_suffixNames[ExtXml]=tr("eXtensible Markup Language file");
    m_suffixNames[ExtYaml]=tr("YAML Ain't Markup Language file");
    //Construct the file suffixes.
    QVector<QString> results;
    for(int i=0; i<FileSuffixCount; ++i)
    {
        results.append(QString("%1 (%2)").arg(m_suffixNames[i], m_suffixLists[i].join(' ')));
    }
    m_fileDialogSuffix = results.join(";;");
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
    m_presetFont.setPointSize(knUi->heightF(10));
    m_scaledFont = m_presetFont;
    //Update the quick search format.
    m_quickSearchFormat.setBackground(QColor(155, 255, 155));
    //Update the format.
    m_markFormat[0].setBackground(QColor(255, 155, 155));
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
