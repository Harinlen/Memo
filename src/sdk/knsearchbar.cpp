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
#include <QAction>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

#include "knlineedit.h"
#include "knfilemanager.h"
#include "kntexteditor.h"
#include "knglobal.h"

#include "knuimanager.h"

#include "knsearchbar.h"

KNSearchBar::KNSearchBar(QWidget *parent) : QFrame(parent),
    m_textBox(new KNLineEdit(this)),
    m_close(genearteButton("x")),
    m_next(genearteButton(">")),
    m_previous(genearteButton("<")),
    m_highlight(new QCheckBox(this)),
    m_matchCase(new QCheckBox(this)),
    m_findLabel(new QLabel(this)),
    m_result(new QLabel(this)),
    m_position(0)
{
    //Configure the UI.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    setLayout(mainLayout);
    //Add widgets to UI.
    mainLayout->addWidget(m_close);
    mainLayout->addWidget(m_findLabel);
    mainLayout->addWidget(m_textBox);
    mainLayout->addWidget(m_previous);
    mainLayout->addWidget(m_next);
    mainLayout->addWidget(m_highlight);
    mainLayout->addWidget(m_matchCase);
    mainLayout->addWidget(m_result);
    mainLayout->addStretch();
    setTabOrder(m_textBox, m_next);
    setTabOrder(m_next, m_previous);
    //Link the translator.
    knUi->addTranslate(this, &KNSearchBar::retranslate);
    //Link button signals.
    connect(m_next, &QPushButton::clicked, this, &KNSearchBar::findNext);
    connect(m_previous, &QPushButton::clicked, this, &KNSearchBar::findPrevious);
    connect(m_textBox, &KNLineEdit::textChanged, this, &KNSearchBar::onKeywordChanged);
    connect(m_close, &QPushButton::clicked, this, &KNSearchBar::onHideSearchBar);
    connect(m_highlight, &QCheckBox::clicked, knGlobal, &KNGlobal::setSearchResultShown);
    //Link the text box focus event.
    connect(m_textBox, &KNLineEdit::editHasFocus, this, &KNSearchBar::onEditHasFocus);
    //Add the local close action.
    QAction *hideSearch = new QAction(this);
    hideSearch->setShortcut(QKeySequence(Qt::Key_Escape));
    hideSearch->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(hideSearch, &QAction::triggered, this, &KNSearchBar::onHideSearchBar);
    addAction(hideSearch);
}

void KNSearchBar::findNext()
{
    KNFileManager *fileManager = static_cast<KNFileManager *>(parentWidget());
    //Extract the editor.
    auto editor = fileManager->currentEditor();
    if(!editor)
    {
        return;
    }
    //Call the function directly.
    editor->quickSearchNext();
}

void KNSearchBar::findPrevious()
{
    KNFileManager *fileManager = static_cast<KNFileManager *>(parentWidget());
    //Extract the editor.
    auto editor = fileManager->currentEditor();
    if(!editor)
    {
        return;
    }
    //Call the function directly.
    editor->quickSearchPrev();
}

void KNSearchBar::showAndSetFocus()
{
    show();
    //Set focus to line edit.
    m_textBox->setFocus();
}

void KNSearchBar::retranslate()
{
    //Update the text of the widgets.
    m_findLabel->setText(tr("Find :"));
    m_highlight->setText(tr("Highlight all"));
    m_matchCase->setText(tr("Match case"));
    m_result->clear();
}

void KNSearchBar::onEditHasFocus()
{
    KNFileManager *fileManager = static_cast<KNFileManager *>(parentWidget());
    //Extract the editor.
    auto editor = fileManager->currentEditor();
    if(!editor)
    {
        return;
    }
    //Fetch the editor text cursor here.
    m_position = editor->textCursor().position();
}

void KNSearchBar::onHideSearchBar()
{
    //Hide the search bar and request to set focus.
    hide();
    emit requireSetFocus();
}

void KNSearchBar::onKeywordChanged()
{
    //Require to search the entire text editor.
    emit requireQuickSearch(m_textBox->text(),
                            m_matchCase->isChecked() ? Qt::CaseSensitive :
                                                       Qt::CaseInsensitive,
                            m_position);
}

QPushButton *KNSearchBar::genearteButton(const QString &mark)
{
    QPushButton *button = new QPushButton(mark, this);
    QSize hint = button->sizeHint();
    button->setFixedSize(hint.height(), hint.height());
    return button;
}
