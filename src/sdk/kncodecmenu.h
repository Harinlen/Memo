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
#ifndef KNCODECMENU_H
#define KNCODECMENU_H

#include <QMenu>

class KNCodecDialog;
/*!
 * \brief The KNCodecMenu class provides the codec menu. This is the Notepad++
 * like codec menu, however, removed multiple useless codec.
 */
class KNCodecMenu : public QMenu
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNCodecMenu menu.
     * \param parent The parent widget.
     */
    explicit KNCodecMenu(QWidget *parent = nullptr);

signals:
    /*!
     * \brief Request to reload the current file using the specific codec.
     * \param codecName The name of the codec.
     */
    void requireLoadAs(const QByteArray &codecName);

    /*!
     * \brief Request to set the current codec name.
     * \param codecName The name of the codec.
     */
    void requireSetCodec(const QByteArray &codecName);

public slots:
    /*!
     * \brief Show the codec dialog.
     */
    void showCodecDialog();

private slots:
    void retranslate();
    void loadUseCodec(int codecIndex);
    void setCurrentCodec(int codec);

private:
    enum CodecMenuItems
    {
        LoadAsSystem,
        LoadAsUtf8,
        LoadAsUtf16BE,
        LoadAsUtf16LE,
        AraISO88596,
        AraISO88596I,
        AraWindows1256,
        BalISO88594,
        BalISO885913,
        BalWindows1257,
        CelISO885914,
        CyrISO88595,
        CyrKOI8U,
        CyrKOI8RU,
        CyrKOI8R,
        CyrMacintosh,
        CyrWindows1251,
        CenEuroWindows1250,
        ChineseGB2312,
        ChineseGBK,
        ChineseGB18030,
        ChineseBig5HKSCS,
        ChineseBig5,
        ChineseBig5ETen,
        EasISO88592,
        GreISO88597,
        GreWindows1253,
        HebISO88598,
        HebISO88598I,
        HebWindows1255,
        JapaneseShiftJIS,
        JapaneseEUCJP,
        JapaneseISO2022JP,
        KoreanWindows949,
        KoreanEUCKR,
        ThaTIS620,
        ThuISO88593,
        ThuISO88599,
        ThuWindows1254,
        WesISO88591,
        WesISO885915,
        WesWindows1252,
        VieWindows1258,
        ToSystem,
        ToUtf8,
        ToUtf16BE,
        ToUtf16LE,
        CodecMenuItemCount,
        LoadActionCount = ToSystem
    };

    enum CodecSubMenus
    {
        CodecSets,
        CodecArabic,
        CodecBaltic,
        CodecCeltic,
        CodecCyrillic,
        CodecCentralEuro,
        CodecChinese,
        CodecEasternEuro,
        CodecGreek,
        CodecHebew,
        CodecJapanese,
        CodecKorean,
        CodecNorthEuro,
        CodecThai,
        CodecTurkish,
        CodecWesternEuro,
        CodecVietnamese,
        CodecSubMenuCount
    };

    KNCodecDialog *m_codecDialog;
    QByteArray m_codecNames[CodecMenuItemCount];
    QMenu *m_subMenus[CodecSubMenuCount];
    QAction *m_menuItems[CodecMenuItemCount];
};

#endif // KNCODECMENU_H
