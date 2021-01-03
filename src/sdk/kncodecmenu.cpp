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
#include <QSignalMapper>

#include "knuimanager.h"

#include "kncodecmenu.h"

KNCodecMenu::KNCodecMenu(QWidget *parent) : QMenu(parent)
{
    //Construct items and menus.
    for(int i=0; i<CodecSubMenuCount; ++i)
    {
        m_subMenus[i] = new QMenu(this);
    }
    for(int i=0; i<CodecMenuItemCount; ++i)
    {
        m_menuItems[i] = new QAction(this);
    }
    //Add items to menu.
    addAction(m_menuItems[LoadAsSystem]);
    addAction(m_menuItems[LoadAsUtf8]);
    addAction(m_menuItems[LoadAsUtf16BE]);
    addAction(m_menuItems[LoadAsUtf16LE]);
    addMenu(m_subMenus[CodecSets]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecArabic]);
    m_subMenus[CodecArabic]->addAction(m_menuItems[AraISO88596]);
    m_subMenus[CodecArabic]->addAction(m_menuItems[AraISO88596I]);
    m_subMenus[CodecArabic]->addAction(m_menuItems[AraWindows1256]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecBaltic]);
    m_subMenus[CodecBaltic]->addAction(m_menuItems[BalISO88594]);
    m_subMenus[CodecBaltic]->addAction(m_menuItems[BalISO885913]);
    m_subMenus[CodecBaltic]->addAction(m_menuItems[BalWindows1257]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecCeltic]);
    m_subMenus[CodecCeltic]->addAction(m_menuItems[CelISO885914]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecCyrillic]);
    m_subMenus[CodecCyrillic]->addAction(m_menuItems[CyrISO88595]);
    m_subMenus[CodecCyrillic]->addAction(m_menuItems[CyrKOI8U]);
    m_subMenus[CodecCyrillic]->addAction(m_menuItems[CyrKOI8RU]);
    m_subMenus[CodecCyrillic]->addAction(m_menuItems[CyrKOI8R]);
    m_subMenus[CodecCyrillic]->addAction(m_menuItems[CyrMacintosh]);
    m_subMenus[CodecCyrillic]->addAction(m_menuItems[CyrWindows1251]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecCentralEuro]);
    m_subMenus[CodecCentralEuro]->addAction(m_menuItems[CenEuroWindows1250]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecChinese]);
    m_subMenus[CodecChinese]->addAction(m_menuItems[ChineseGB2312]);
    m_subMenus[CodecChinese]->addAction(m_menuItems[ChineseGBK]);
    m_subMenus[CodecChinese]->addAction(m_menuItems[ChineseGB18030]);
    m_subMenus[CodecChinese]->addAction(m_menuItems[ChineseBig5HKSCS]);
    m_subMenus[CodecChinese]->addAction(m_menuItems[ChineseBig5]);
    m_subMenus[CodecChinese]->addAction(m_menuItems[ChineseBig5ETen]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecEasternEuro]);
    m_subMenus[CodecEasternEuro]->addAction(m_menuItems[EasISO88592]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecGreek]);
    m_subMenus[CodecGreek]->addAction(m_menuItems[GreISO88597]);
    m_subMenus[CodecGreek]->addAction(m_menuItems[GreWindows1253]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecHebew]);
    m_subMenus[CodecHebew]->addAction(m_menuItems[HebISO88598]);
    m_subMenus[CodecHebew]->addAction(m_menuItems[HebISO88598I]);
    m_subMenus[CodecHebew]->addAction(m_menuItems[HebWindows1255]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecJapanese]);
    m_subMenus[CodecJapanese]->addAction(m_menuItems[JapaneseShiftJIS]);
    m_subMenus[CodecJapanese]->addAction(m_menuItems[JapaneseEUCJP]);
    m_subMenus[CodecJapanese]->addAction(m_menuItems[JapaneseISO2022JP]);
    m_subMenus[CodecJapanese]->addAction(m_menuItems[JapaneseSJIS]);
    m_subMenus[CodecJapanese]->addAction(m_menuItems[JapaneseShiftJIS]);
    m_subMenus[CodecJapanese]->addAction(m_menuItems[JapaneseJIS7]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecKorean]);
    m_subMenus[CodecKorean]->addAction(m_menuItems[KoreanWindows949]);
    m_subMenus[CodecKorean]->addAction(m_menuItems[KoreanEUCKR]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecThai]);
    m_subMenus[CodecThai]->addAction(m_menuItems[ThaTIS620]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecTurkish]);
    m_subMenus[CodecTurkish]->addAction(m_menuItems[ThuISO88593]);
    m_subMenus[CodecTurkish]->addAction(m_menuItems[ThuISO88599]);
    m_subMenus[CodecTurkish]->addAction(m_menuItems[ThuWindows1254]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecWesternEuro]);
    m_subMenus[CodecWesternEuro]->addAction(m_menuItems[WesISO88591]);
    m_subMenus[CodecWesternEuro]->addAction(m_menuItems[WesISO885915]);
    m_subMenus[CodecWesternEuro]->addAction(m_menuItems[WesWindows1252]);
    m_subMenus[CodecSets]->addMenu(m_subMenus[CodecVietnamese]);
    m_subMenus[CodecVietnamese]->addAction(m_menuItems[VieWindows1258]);
    addSeparator();
    addAction(m_menuItems[ToSystem]);
    addAction(m_menuItems[ToUtf8]);
    addAction(m_menuItems[ToUtf16BE]);
    addAction(m_menuItems[ToUtf16LE]);
    //Codec names are fixed, not need to be updated.
    m_menuItems[LoadAsSystem]->setText("System");
    m_menuItems[LoadAsUtf8]->setText("UTF-8");
    m_menuItems[LoadAsUtf16BE]->setText("UTF-16 BE");
    m_menuItems[LoadAsUtf16LE]->setText("UTF-16 LE");
    m_menuItems[AraISO88596]->setText("ISO 8859-6");
    m_menuItems[AraISO88596I]->setText("ISO 8859-6-I");
    m_menuItems[AraWindows1256]->setText("Windows 1256");
    m_menuItems[BalISO88594]->setText("ISO 8859-4");
    m_menuItems[BalISO885913]->setText("ISO 8859-13");
    m_menuItems[BalWindows1257]->setText("Windows 1257");
    m_menuItems[CelISO885914]->setText("ISO 8859-14");
    m_menuItems[CyrISO88595]->setText("ISO 8859-5");
    m_menuItems[CyrKOI8U]->setText("KOI8-U");
    m_menuItems[CyrKOI8RU]->setText("KOI8-RU");
    m_menuItems[CyrKOI8R]->setText("KOI8-R");
    m_menuItems[CyrMacintosh]->setText("Macintosh");
    m_menuItems[CyrWindows1251]->setText("Windows 1251");
    m_menuItems[CenEuroWindows1250]->setText("Windows 1250");
    m_menuItems[ChineseGB2312]->setText("GB2312");
    m_menuItems[ChineseGBK]->setText("GBK");
    m_menuItems[ChineseGB18030]->setText("GB18030");
    m_menuItems[ChineseBig5HKSCS]->setText("Big5 HKSCS");
    m_menuItems[ChineseBig5]->setText("Big5");
    m_menuItems[ChineseBig5ETen]->setText("Big5 ETen");
    m_menuItems[EasISO88592]->setText("ISO 8859-2");
    m_menuItems[GreISO88597]->setText("ISO 8859-7");
    m_menuItems[GreWindows1253]->setText("Windows 1253");
    m_menuItems[HebISO88598]->setText("ISO 8859-8");
    m_menuItems[HebISO88598I]->setText("ISO 8859-8-I");
    m_menuItems[HebWindows1255]->setText("Windows 1255");
    m_menuItems[JapaneseShiftJIS]->setText("Shift JIS");
    m_menuItems[JapaneseEUCJP]->setText("EUC-JP");
    m_menuItems[JapaneseISO2022JP]->setText("ISO 2022-JP");
    m_menuItems[JapaneseSJIS]->setText("SJIS");
    m_menuItems[JapaneseMSKanji]->setText("MS Kanji");
    m_menuItems[JapaneseJIS7]->setText("JIS7");
    m_menuItems[KoreanWindows949]->setText("Windows 949");
    m_menuItems[KoreanEUCKR]->setText("EUC-KR");
    m_menuItems[ThaTIS620]->setText("TIS-620");
    m_menuItems[ThuISO88593]->setText("ISO 8859-3");
    m_menuItems[ThuISO88599]->setText("ISO 8859-9");
    m_menuItems[ThuWindows1254]->setText("Windows 1254");
    m_menuItems[WesISO88591]->setText("ISO 8859-1");
    m_menuItems[WesISO885915]->setText("ISO 8859-15");
    m_menuItems[WesWindows1252]->setText("Windows 1252");
    m_menuItems[VieWindows1258]->setText("Windows 1258");
    //Construct the label.
    m_codecNames[LoadAsSystem] = "System";
    m_codecNames[LoadAsUtf8] = "UTF-8";
    m_codecNames[LoadAsUtf16BE] = "UTF-16 BE";
    m_codecNames[LoadAsUtf16LE] = "UTF-16 LE";
    m_codecNames[ToSystem] = m_codecNames[LoadAsSystem];
    m_codecNames[ToUtf8] = m_codecNames[LoadAsUtf8];
    m_codecNames[ToUtf16BE] = m_codecNames[LoadAsUtf16BE];
    m_codecNames[ToUtf16LE] = m_codecNames[LoadAsUtf16LE];
    m_codecNames[AraISO88596] = "ISO-8859-6";
    m_codecNames[AraISO88596I] = "ISO-8859-6-I";
    m_codecNames[AraWindows1256] = "windows-1256";
    m_codecNames[BalISO88594] = "ISO-8859-4";
    m_codecNames[BalISO885913] = "ISO-8859-13";
    m_codecNames[BalWindows1257] = "windows-1257";
    m_codecNames[CelISO885914] = "ISO-8859-14";
    m_codecNames[CyrISO88595] = "ISO-8859-5";
    m_codecNames[CyrKOI8U] = "KOI8-U";
    m_codecNames[CyrKOI8RU] = "KOI8-RU";
    m_codecNames[CyrKOI8R] = "KOI8-R";
    m_codecNames[CyrMacintosh] = "macintosh";
    m_codecNames[CyrWindows1251] = "windows-1251";
    m_codecNames[CenEuroWindows1250] = "windows-1250";
    m_codecNames[ChineseGB2312] = "GB2312";
    m_codecNames[ChineseGBK] = "GBK";
    m_codecNames[ChineseGB18030] = "GB18030";
    m_codecNames[ChineseBig5HKSCS] = "Big5-HKSCS";
    m_codecNames[ChineseBig5] = "Big5";
    m_codecNames[ChineseBig5ETen] = "Big5-ETen";
    m_codecNames[EasISO88592] = "ISO-8859-2";
    m_codecNames[GreISO88597] = "ISO-8859-7";
    m_codecNames[GreWindows1253] = "windows-1253";
    m_codecNames[HebISO88598] = "ISO-8859-8";
    m_codecNames[HebISO88598I] = "ISO 8859-8-I";
    m_codecNames[HebWindows1255] = "windows-1255";
    m_codecNames[JapaneseShiftJIS] = "Shift_JIS";
    m_codecNames[JapaneseEUCJP] = "EUC-JP";
    m_codecNames[JapaneseISO2022JP] = "ISO-2022-JP";
    m_codecNames[JapaneseSJIS] = "SJIS";
    m_codecNames[JapaneseMSKanji] = "MS_Kanji";
    m_codecNames[JapaneseJIS7] = "JIS7";
    m_codecNames[KoreanWindows949] = "windows-949";
    m_codecNames[KoreanEUCKR] = "EUC-KR";
    m_codecNames[ThaTIS620] = "TIS-620";
    m_codecNames[ThuISO88593] = "ISO-8859-3";
    m_codecNames[ThuISO88599] = "ISO-8859-9";
    m_codecNames[ThuWindows1254] = "windows-1254";
    m_codecNames[WesISO88591] = "ISO-8859-1";
    m_codecNames[WesISO885915] = "ISO-8859-15";
    m_codecNames[WesWindows1252] = "windows-1252";
    m_codecNames[VieWindows1258] = "windows-1258";
    //Link the translator.
    knUi->addTranslate(this, &KNCodecMenu::retranslate);
    //Link the actions.
    QSignalMapper *loadMapper = new QSignalMapper(this);
    for(int i=0; i<LoadActionCount; ++i)
    {
        connect(m_menuItems[i], &QAction::triggered,
                loadMapper,
                static_cast<void (QSignalMapper::*)(void)>(&QSignalMapper::map));
        loadMapper->setMapping(m_menuItems[i], i);
    }
    connect(loadMapper, &QSignalMapper::mappedInt,
            this, &KNCodecMenu::loadUseCodec);
    //Connect the convert settings.
    QSignalMapper *saveMapper = new QSignalMapper(this);
    for(int i=ToSystem; i<CodecMenuItemCount; ++i)
    {
        connect(m_menuItems[i], &QAction::triggered,
                saveMapper,
                static_cast<void (QSignalMapper::*)(void)>(&QSignalMapper::map));
        saveMapper->setMapping(m_menuItems[i], i);
    }
    connect(saveMapper, &QSignalMapper::mappedInt,
            this, &KNCodecMenu::setCurrentCodec);
}

void KNCodecMenu::retranslate()
{
    //Update the title.
    setTitle(tr("E&ncoding"));
    QString convertTo = tr("Convert to %1");
    m_menuItems[ToSystem]->setText(convertTo.arg("System"));
    m_menuItems[ToUtf8]->setText(convertTo.arg("UTF-8"));
    m_menuItems[ToUtf16BE]->setText(convertTo.arg("UTF-16 BE"));
    m_menuItems[ToUtf16LE]->setText(convertTo.arg("UTF-16 LE"));
    //Update the sub menu.
    m_subMenus[CodecSets]->setTitle(tr("Character sets"));
    m_subMenus[CodecArabic]->setTitle(tr("Arabic"));
    m_subMenus[CodecBaltic]->setTitle(tr("Baltic"));
    m_subMenus[CodecCeltic]->setTitle(tr("Celtic"));
    m_subMenus[CodecCyrillic]->setTitle(tr("Cyrillic"));
    m_subMenus[CodecCentralEuro]->setTitle(tr("Central European"));
    m_subMenus[CodecChinese]->setTitle(tr("Chinese"));
    m_subMenus[CodecEasternEuro]->setTitle(tr("Eastern European"));
    m_subMenus[CodecGreek]->setTitle(tr("Greek"));
    m_subMenus[CodecHebew]->setTitle(tr("Hebrew"));
    m_subMenus[CodecJapanese]->setTitle(tr("Japanese"));
    m_subMenus[CodecKorean]->setTitle(tr("Korean"));
    m_subMenus[CodecNorthEuro]->setTitle(tr("North European"));
    m_subMenus[CodecThai]->setTitle(tr("Thai"));
    m_subMenus[CodecTurkish]->setTitle(tr("Turkish"));
    m_subMenus[CodecWesternEuro]->setTitle(tr("Western European"));
    m_subMenus[CodecVietnamese]->setTitle(tr("Vietnamese"));
}

void KNCodecMenu::loadUseCodec(int codecIndex)
{
    //Emit the codec signal.
    emit requireLoadAs(m_codecNames[codecIndex]);
}

void KNCodecMenu::setCurrentCodec(int codec)
{
    //Emit the codec signal.
    emit requireSetCodec(m_codecNames[codec]);
}
