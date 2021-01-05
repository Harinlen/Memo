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
#ifndef KNSEARCHBAR_H
#define KNSEARCHBAR_H

#include <QFrame>

class QCheckBox;
class QPushButton;
class QLabel;
class KNLineEdit;
/*!
 * \brief The KNSearchBar class provides the quick search bar for the text
 * editor. It is used for the quick / incremental search.
 */
class KNSearchBar : public QFrame
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNSearchBar widget.
     * \param parent The parent widget.
     */
    explicit KNSearchBar(QWidget *parent = nullptr);

signals:
    /*!
     * \brief Require to set as the focus.
     */
    void requireSetFocus();

    /*!
     * \brief Require to do quick search in the current text editor.
     * \param keyword The keywords text.
     * \param cs The case sensitivity options.
     * \param pos The cursor position.
     */
    void requireQuickSearch(const QString &keyword, Qt::CaseSensitivity cs,
                            int pos);

public slots:
    /*!
     * \brief Execute the find next of the quick search.
     */
    void findNext();

    /*!
     * \brief Execute the find previous of the quick search.
     */
    void findPrevious();

    /*!
     * \brief Show the search bar and set the focus to the line edit.
     */
    void showAndSetFocus();

private slots:
    void retranslate();
    void onEditHasFocus();
    void onHideSearchBar();
    void onKeywordChanged();

private:
    QPushButton *genearteButton(const QString &mark);
    KNLineEdit *m_textBox;
    QPushButton *m_close, *m_next, *m_previous;
    QCheckBox *m_highlight, *m_matchCase;
    QLabel *m_findLabel, *m_result;
    int m_position;
};

#endif // KNSEARCHBAR_H
