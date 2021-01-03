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
};

#endif // KNTEXTEDITORPANEL_H
