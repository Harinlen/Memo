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
#ifndef KNTEXTEDITORPANEL_H
#define KNTEXTEDITORPANEL_H

#include <QWidget>

#define MARK_WIDTH (18)
#define FOLD_WIDTH (14)

class KNTextEditor;
/*!
 * \brief The KNTextEditorPanel class provides a text editor side panel widget.
 * It displays the line number, mark and code folding.
 */
class KNTextEditorPanel : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Construt a KNTextEditorPanel widget panel.
     * \param parent The parent widget.
     */
    explicit KNTextEditorPanel(KNTextEditor *parent = nullptr);

    /*!
     * \brief Get whether the bookmarks are shown.
     * \return If the bookmarks are shown, return true.
     */
    bool showMarks() const;

    /*!
     * \brief Update the show mark flag of the current panel.
     * \param showMarks The mark shown flags.
     */
    void setShowMarks(bool showMarks);

    /*!
     * \brief Get the panel basic width.
     * \return The panel base width.
     */
    int panelBaseWidth() const;

    /*!
     * \brief Get whether the fold are shown.
     * \return If the fold marks are shown, return true.
     */
    bool showFold() const;

    /*!
     * \brief Update the show fold flag of the current panel.
     * \param showFold The fold shown flags.
     */
    void setShowFold(bool showFold);

signals:

public slots:
    /*!
     * \brief Update the width of the line number.
     * \param width The width of the line number part.
     */
    void setLineNumberWidth(int width);

protected:
    /*!
     * \brief Reimplemented from QWidget::paintEvent().
     */
    void paintEvent(QPaintEvent *event);

private:
    int m_lineNumberWidth;
    bool m_showMarks, m_showFold;
};

#endif // KNTEXTEDITORPANEL_H
