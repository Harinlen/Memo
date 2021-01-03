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
#include <QResource>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWidget>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTranslator>
#include <QFontDatabase>

#include "knutil.h"

#include "knuimanager.h"

KNUiManager *KNUiManager::ins = nullptr;

KNUiManager *KNUiManager::instance()
{
    //Return the instance pointer.
    return ins;
}

void KNUiManager::initial(QObject *parent)
{
    //Check if the singleton instance variable is null. Set the pointer to this
    //object if this is the first constructed object.
    if(ins == nullptr)
    {
        ins=new KNUiManager(parent);
    }
}

void KNUiManager::setDpi(int dpiX, int dpiY)
{
#ifdef Q_OS_MACX
    Q_UNUSED(dpiX)
    Q_UNUSED(dpiY)
    //Use the 1.0 as the default scaler. Nothing need to be changed.
    //For Mac OS X, I have no idea what is happened, but it seems that it could
    //automatically support the Retina display.
    m_xScaler=1.0;
    m_yScaler=1.0;
    return;
#else
    //For other OS, it needs scaling. The DPI should be several times of 72.
    //When you changing the scaling on Windows, like 150%, 200%, you actually
    //change this settings.
    m_xScaler=(qreal)dpiX/96.0;
    m_yScaler=(qreal)dpiY/96.0;
#endif
}

QSize KNUiManager::size(int width, int height) const
{
    //Scale the width and height.
    return QSize(scaleX(width), scaleY(height));
}

QPoint KNUiManager::pos(int x, int y) const
{
    //Scaled the position.
    return QPoint(scaleX(x), scaleY(y));
}

QMargins KNUiManager::margins(int left, int top, int right, int bottom) const
{
    return QMargins(scaleX(left), scaleY(top), scaleX(right), scaleY(bottom));
}

QPointF KNUiManager::posF(qreal x, qreal y) const
{
    //Scale the position.
    return QPointF(scaleXF(x), scaleYF(y));
}

QRect KNUiManager::rect(int x, int y, int width, int height) const
{
    return QRect(scaleX(x), scaleY(y), scaleX(width), scaleY(height));
}

QRectF KNUiManager::rectF(qreal x, qreal y, qreal width, qreal height) const
{
    return QRectF(scaleXF(x), scaleYF(y), scaleXF(width), scaleYF(height));
}

qreal KNUiManager::heightF(qreal rawHeight) const
{
    return scaleYF(rawHeight);
}

qreal KNUiManager::widthF(qreal rawWidth) const
{
    return scaleXF(rawWidth);
}

int KNUiManager::height(int rawHeight) const
{
    return scaleY(rawHeight);
}

int KNUiManager::width(int rawWidth) const
{
    return scaleX(rawWidth);
}

QString KNUiManager::themeResourcePath(const QString &path) const
{
    return QString("%1%2").arg(m_themePrefix, path);
}

void KNUiManager::addThemeWidget(QWidget *widget, const QString &tagName)
{
    m_themeWidgets.insert(widget, tagName);
}

void KNUiManager::loadFonts(const QString &folderPath)
{
    QDir fontDir(folderPath);
    //Check the folder is exsist or not.
    if(fontDir.exists())
    {
        //Get all the files, load all the fonts.
        QFileInfoList fontFiles=fontDir.entryInfoList();
        for(auto i : fontFiles)
        {
            //Ignore the dot(.) and dot-dot(..).
            if(i.fileName()=="." || i.fileName()=="..")
            {
                continue;
            }
            //If the type of current file is File, then try to load the font.
            if(i.isFile())
            {
                loadFontFile(i.absoluteFilePath());
            }
        }
    }
}

void KNUiManager::loadFontFile(const QString &filePath)
{
    //Directly add to font database.
    QFontDatabase::addApplicationFont(filePath);
}

void KNUiManager::addLanguage(const QString &name, const QString &nativeName,
                              const QString &filePath, const QPixmap &icon)
{
    //Check whether the item is already contained in the list.
    for(auto i : m_languageList)
    {
        if(i.name == name)
        {
            return;
        }
    }
    //Add item to language list if the item is not in the list.
    Translation item;
    item.name=name;
    item.nativeName=nativeName;
    item.filePath=filePath;
    item.icon=icon;
    m_languageList.append(item);
}

void KNUiManager::loadLanguages(const QString &folderPath)
{
    //Load all the languages.
    Q_UNUSED(folderPath)
}

void KNUiManager::loadThemes(const QString &folderPath)
{
    //Load all the theme.
    Q_UNUSED(folderPath)
}

void KNUiManager::setTheme(int index)
{
    //Check whether previous theme is loaded.
    if(m_currentTheme != -1)
    {
        //Unregister the theme file.
        QResource::unregisterResource(m_themePaths.at(m_currentTheme));
        //Clear the current theme.
        m_currentTheme = -1;
    }
    //Load the theme file.
    if(index > 0 && QResource::registerResource(m_themePaths.at(index)))
    {
        //Load the customized theme successfully.
        m_themePrefix = ":/theme/";
        m_currentTheme = index;
    }
    else
    {
        //Fall back the index to 0.
        index = 0;
    }
    if(index == 0)
    {
        //Set the prefix to be "default_theme".
        m_themePrefix = ":/default_theme/";
        m_currentTheme = 0;
    }
    //Load the theme.
    QFile themeFile(themeResourcePath("theme.json"));
    themeFile.open(QIODevice::ReadOnly);
    QJsonObject themeData =
            QJsonDocument::fromJson(themeFile.readAll()).object();
    themeFile.close();
    //Construct the palette and font map.
    QHash<QString, QPalette> themePalette;
    QHash<QString, QFont> themeFonts;
    for(auto i = themeData.constBegin(); i!=themeData.constEnd(); ++i)
    {
        //Loop and check the key, which is the name of the palette.
        QString &&paletteName = i.key();
        //Check whether the palette name has been parsed before.
        if(themePalette.contains(paletteName))
        {
            continue;
        }
        //Parse the palette.
        parsePalette(paletteName, themeData, themePalette, themeFonts);
    }
    //Update the application font and palette first.
    QApplication::setFont(themeFonts.value("Application"));
    QApplication::setPalette(themePalette.value("Application"));
    //Update all the widget.
    auto iterator = m_themeWidgets.constBegin();
    while(iterator != m_themeWidgets.constEnd())
    {
        //Update the palette.
        QWidget *widget = iterator.key();
        widget->setPalette(themePalette.value(iterator.value()));
        widget->setFont(themeFonts.value(iterator.value()));
        //Move to next widget.
        ++iterator;
    }
    //Emit the theme update signals.
    emit themeChanged();
}

void KNUiManager::setTheme(const QString &themeName)
{
    //Check the theme index.
    int themeIndex = m_themeIndex.value(themeName, -1);
    if(themeIndex == -1)
    {
        return;
    }
    //Set the theme.
    setTheme(themeIndex);
}

void KNUiManager::setLanguage(int index)
{
    //Check whether the index is inside the range of the language list.
    if(index>-1 && index<m_languageList.size())
    {
        //Save the language index.
        m_currentLangauge=index;
        //Remove the current translator.
        if(!m_translator->isEmpty())
        {
            qApp->removeTranslator(m_translator);
        }
        //Load the translate file to translator. If sucessful, install the
        //translator to application.
        if(!m_translator->load(m_languageList.at(m_currentLangauge).filePath))
        {
            qApp->installTranslator(m_translator);
        }
        //Ask to retranslate the entire application.
        emit languageChanged();
    }
}

void KNUiManager::setLanguage(const QString &name)
{
    //Loop and find the language.
    for(int i=0; i<m_languageList.size(); ++i)
    {
        //Check the name matches.
        if(m_languageList.at(i).name == name)
        {
            //Set to target language.
            setLanguage(i);
            return;
        }
    }
}

KNUiManager::KNUiManager(QObject *parent) : QObject(parent),
    m_currentTheme(-1),
    m_xScaler(1.0),
    m_yScaler(1.0),
    m_noLanguageIcon(QPixmap(":/empty_language.png")),
    m_translator(new QTranslator(this)),
    m_currentLangauge(-1)
{
    // -- Theme Manager --
    //Initial the global color role map.
    m_colorRoleList["WindowText"]=QPalette::WindowText;
    m_colorRoleList["Button"]=QPalette::Button;
    m_colorRoleList["Light"]=QPalette::Light;
    m_colorRoleList["Midlight"]=QPalette::Midlight;
    m_colorRoleList["Dark"]=QPalette::Dark;
    m_colorRoleList["Mid"]=QPalette::Mid;
    m_colorRoleList["Text"]=QPalette::Text;
    m_colorRoleList["BrightText"]=QPalette::BrightText;
    m_colorRoleList["ButtonText"]=QPalette::ButtonText;
    m_colorRoleList["Base"]=QPalette::Base;
    m_colorRoleList["Window"]=QPalette::Window;
    m_colorRoleList["Shadow"]=QPalette::Shadow;
    m_colorRoleList["Highlight"]=QPalette::Highlight;
    m_colorRoleList["HighlightedText"]=QPalette::HighlightedText;
    m_colorRoleList["Link"]=QPalette::Link;
    m_colorRoleList["LinkVisited"]=QPalette::LinkVisited;
    m_colorRoleList["AlternateBase"]=QPalette::AlternateBase;
    m_colorRoleList["NoRole"]=QPalette::NoRole;
    m_colorRoleList["ToolTipBase"]=QPalette::ToolTipBase;
    m_colorRoleList["ToolTipText"]=QPalette::ToolTipText;
    //Add the default theme.
    m_themeIndex.insert("Default", 0);
    m_themePaths.append("");
    // -- Locale Manager --
    //Load the language translations.
    //This file stored all the language name and it's translation to their
    //native language.
    QFile languageNativeNameFile(":/language_native_name.json");
    if(languageNativeNameFile.open(QIODevice::ReadOnly))
    {
        //Load the json object from the file.
        QJsonObject &&languageNames=
                QJsonDocument::fromJson(
                    languageNativeNameFile.readAll()).object();
        //Close the file.
        languageNativeNameFile.close();
        //Load the pair one by one.
        for(auto i=languageNames.constBegin(); i!=languageNames.constEnd(); ++i)
        {
            //Insert the translation to hash list.
            m_languageTranslation.insert(i.key(), i.value().toString());
        }
    }
    //Add English language item, English will always be the first language, it's
    //the default embedded language.
    addLanguage("English", "English", "", QPixmap("://public/English.png"));
}

inline float KNUiManager::scaleXF(float rawWidth) const
{
    return m_xScaler * rawWidth;
}

float KNUiManager::scaleYF(float rawHeight) const
{
    return m_yScaler * rawHeight;
}

int KNUiManager::scaleX(int rawWidth) const
{
    return static_cast<int>(scaleXF(rawWidth));
}

int KNUiManager::scaleY(int rawHeight) const
{
    return static_cast<float>(scaleYF(rawHeight));
}

void KNUiManager::parsePalette(const QString &name,
                               const QJsonObject &themeData,
                               QHash<QString, QPalette> &themePalette,
                               QHash<QString, QFont> &themeFonts)
{
    //Extract the current name object.
    QJsonObject &&paletteData = themeData.value(name).toObject();
    //Prepare the palette.
    QPalette palette;
    QFont font;
    //Check whether it is inherit from a parent palette.
    if(paletteData.contains("Parent"))
    {
        //If current palette has a parent palette, check whether the parent
        //palette has been load.
        QString &&parentName = paletteData.value("Parent").toString();
        if(!themePalette.contains(parentName))
        {
            //If the parent hasn't been loaded, load the parent palette
            //recursively.
            parsePalette(parentName, themeData, themePalette, themeFonts);
        }
        //Extract the palette and font.
        palette = themePalette.value(parentName);
        font = themeFonts.value(parentName);
    }
    else
    {
        //Extract and use the application font and palette.
        palette = QApplication::palette();
        font = QApplication::font();
    }
    //Parse the current palette.
    for(auto i = paletteData.constBegin(); i != paletteData.constEnd(); ++i)
    {
        //Apply the role settings.
        if(m_colorRoleList.contains(i.key()))
        {
            //Set the palette.
            palette.setBrush(m_colorRoleList.value(i.key()),
                             KNUtil::parseColor(i.value().toString()));
        }
    }
    //Insert the palette map.
    themePalette.insert(name, palette);
    //Check font settings.
    if(paletteData.contains("Font"))
    {
        //Parse the font information.
        QJsonObject &&fontData = paletteData.value("Font").toObject();
        //Check the data type.
        if(fontData.contains("Family"))
        {
            font.setFamily(fontData.value("Family").toString());
        }
        if(fontData.contains("Size"))
        {
            font.setPixelSize(height(fontData.value("Size").toInt()));
        }
        if(fontData.contains("Bold"))
        {
            font.setBold(fontData.value("Bold").toBool());
        }
    }
    //Insert the font data.
    themeFonts.insert(name, font);
}
