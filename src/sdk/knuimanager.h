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
#ifndef KNUIMANAGER_H
#define KNUIMANAGER_H

#include <QPalette>
#include <QMargins>
#include <QRectF>
#include <QHash>

#include <QObject>

/*!
 * \def UI_FRAME_INTERVAL
 * The timeline frame update interval, support for 120Hz frame rate.
 */
#define UI_FRAME_INTERVAL   (7)

/*!
 * \def knUi
 * A global pointer referring to the unique global UI manager object.
 */
#define knUi                (KNUiManager::instance())

class QTranslator;
/*!
 * \brief The KNUiManager class provides a unique interface to all the previous
 * manager functions, including locale, theme, shortcut and so on. All the
 * signals of these managers are now belongs to UI manager.
 */
class KNUiManager : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Get the global instance of the UI manager.
     * \return The global UI manager instance.
     */
    static KNUiManager *instance();

    /*!
     * \brief Initial the manager, generate the instance with the given parent
     * object.\n
     * Only the first time will create a instance.
     */
    static void initial(QObject *parent = 0);

    /*!
     * \brief Set the application window horizontal and vertical DPI. These
     * information should be provided from the main window.
     * \param dpiX The horizontal DPI.
     * \param dpiY The vertical DPI.
     */
    void setDpi(int dpiX, int dpiY);

    /*!
     * \brief Get the scale of the size.
     * \param width The original width.
     * \param height The original height.
     * \return The scaled width and height.
     */
    QSize size(int width, int height) const;

    /*!
     * \brief Get the scaled position of the point.
     * \param x The X position.
     * \param y The Y position.
     * \return The scaled position.
     */
    QPoint pos(int x, int y) const;

    /*!
     * \brief Get the scaled margins.
     * \param left The left margin.
     * \param top The top margin.
     * \param right The right margin.
     * \param bottom The bottom margin.
     * \return The scaled margin.
     */
    QMargins margins(int left, int top, int right, int bottom) const;

    /*!
     * \brief Get the scaled position of the floating point.
     * \param x The X position.
     * \param y The Y position.
     * \return The scaled position.
     */
    QPointF posF(qreal x, qreal y) const;

    /*!
     * \brief Get the scaled rect.
     * \param x The X pos of the rect.
     * \param y The Y pos of the rect.
     * \param width The width of the rect.
     * \param height The height of the rect.
     * \return The result of the scaled rectangle.
     */
    QRect rect(int x, int y, int width, int height) const;

    /*!
     * \brief Get the scaled float rect.
     * \param x The X pos of the rect.
     * \param y The Y pos of the rect.
     * \param width The width of the rect.
     * \param height The height of the rect.
     * \return The result of the scaled rectangle.
     */
    QRectF rectF(qreal x, qreal y, qreal width, qreal height) const;

    /*!
     * \brief This is an override function.\n
     * Get the scaled position of the floating point.
     * \param rawPos The original point.
     * \return The scaled position.
     */
    QPointF posF(const QPointF rawPos)
    {
        return posF(rawPos.x(), rawPos.y());
    }

    /*!
     * \brief Get the scaled height, this is the float point version.
     * \param rawHeight The original height.
     * \return The scaled height.
     */
    qreal heightF(qreal rawHeight) const;

    /*!
     * \brief Get the scaled width, this is the float point version.
     * \param rawWidth The original width.
     * \return The scaled width.
     */
    qreal widthF(qreal rawWidth) const;

    /*!
     * \brief Get the scaled height.
     * \param rawHeight The original height.
     * \return The scaled height.
     */
    int height(int rawHeight) const;

    /*!
     * \brief Get the scaled width.
     * \param rawWidth The original width.
     * \return The scaled width.
     */
    int width(int rawWidth) const;

    /*!
     * \brief Get the full path of the target theme resource.
     * \param path The resource path.
     * \return The full path with the resource prefix.
     */
    QString themeResourcePath(const QString &path) const;

    /*!
     * \brief Add the widget with the tag to the theme manager, this allows to
     * use theme manager to manage the palette and font.
     * \param widget The widget pointer.
     * \param tagName The tag name of the theme palette and font.
     */
    void addThemeWidget(QWidget *widget, const QString &tagName);

    template<typename Func2>
    /*!
     * \brief Link the language change signal with the retranslate slot which
     * should be provide by the receiver.
     * \param receiver The recevier object.
     * \param retranslate The retranslate slot.
     */
    void addTranslate(
            const typename QtPrivate::FunctionPointer<Func2>::Object *receiver,
            Func2 retranslate)
    {
        //Link the language change signal to the receiver's retranslate slot.
        connect(this, &KNUiManager::languageChanged, receiver, retranslate);
    }

signals:
    /*!
     * \brief Signals for widget to know that the theme has been changed.
     */
    void themeChanged();

    /*!
     * \brief When a new language file is loaded, this signal will emitted.
     */
    void languageChanged();

public slots:
    /*!
     * \brief Load the palettes information from the specific theme dir.
     * \param themePath The theme directory path.
     */
    void loadThemes(const QString &folderPath);

    /*!
     * \brief Load the information about languages, load all the data about the
     * language file. According to langauge directory list.
     */
    void loadLanguages(const QString &folderPath);

    /*!
     * \brief Load all the font in from a folder. Notice: this function won't
     * find file in the sub folder.
     * \param folderPath The folder path.
     */
    void loadFonts(const QString &folderPath);

    /*!
     * \brief Set the current theme according to the index in the theme list.
     * \param index The theme index.
     */
    void setTheme(int index);

    /*!
     * \brief Set the theme according to the theme name.
     * \param themeName The theme name.
     */
    void setTheme(const QString &themeName);

    /*!
     * \brief Set the langauge according to the index in the language list.
     * \param index The language index.
     */
    void setLanguage(int index);

    /*!
     * \brief Set current langauge via the language key.
     * \param name The language English name.
     */
    void setLanguage(const QString &name);

private:
    static KNUiManager *ins;

    explicit KNUiManager(QObject *parent = nullptr);
    KNUiManager(const KNUiManager &);
    KNUiManager(KNUiManager &&);

    inline float scaleXF(float rawWidth) const;
    inline float scaleYF(float rawHeight) const;
    inline int scaleX(int rawWidth) const;
    inline int scaleY(int rawHeight) const;

    void parsePalette(const QString &name,
                      const QJsonObject &themeData,
                      QHash<QString, QPalette> &themePalette,
                      QHash<QString, QFont> &themeFonts);
    void loadFontFile(const QString &filePath);
    void addLanguage(const QString &name, const QString &nativeName,
                     const QString &filePath, const QPixmap &icon);

    //Theme manager.
    QHash<QString, QPalette::ColorRole> m_colorRoleList;
    QStringList m_themePaths;
    QHash<QWidget *, QString> m_themeWidgets;
    QHash<QString, int> m_themeIndex;
    QString m_themePrefix;
    int m_currentTheme;
    //Dpi manager.
    float m_xScaler, m_yScaler;
    //Locale manager.
    struct Translation
    {
        QString name;
        QString nativeName;
        QString filePath;
        QPixmap icon;
    };

    QList<Translation> m_languageList;
    QHash<QString, QString> m_languageTranslation;
    QPixmap m_noLanguageIcon;
    QTranslator *m_translator;
    int m_currentLangauge;
};

#endif // KNUIMANAGER_H
