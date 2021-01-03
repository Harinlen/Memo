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
#include <QTabBar>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>

#include "knconfiguremanager.h"
#include "knconfigure.h"
#include "kntexteditor.h"
#include "knuimanager.h"
#include "knfilemanager.h"

#include "knfindwindow.h"

QString errorText(const QString &content)
{
    return QString("<font color=\"red\">%1</font>").arg(content);
}

QString infoText(const QString &content)
{
    return QString("<font color=\"blue\">%1</font>").arg(content);
}

KNFindWindow::KNFindWindow(QWidget *parent) :
    QDialog(parent),
    m_config(knConf->userConfigure()->getConfigure("FindDialog")),
    m_layout(new QGridLayout()),
    m_tabBar(new QTabBar(this)),
    m_message(new QLabel(this)),
    m_findText(generateBox()),
    m_replaceText(generateBox()),
    m_filters(generateBox()),
    m_optionNormal(new QRadioButton(this)),
    m_optionExtend(new QRadioButton(this)),
    m_optionReg(new QRadioButton(this)),
    m_transOnLose(new QRadioButton(this)),
    m_transAlways(new QRadioButton(this)),
    m_transValue(new QSlider(Qt::Horizontal, this)),
    m_selectBox(new QGroupBox(this)),
    m_searchMode(new QGroupBox(this)),
    m_transparent(new QGroupBox(this)),
    m_inSelection(new QCheckBox(this)),
    m_dotExtend(new QCheckBox(this)),
    m_transparentEnable(new QCheckBox(m_transparent))
{
    //Configure the window.
    setFocusPolicy(Qt::ClickFocus);
    setFocusProxy(m_findText);
    setMinimumWidth(knUi->width(570));
    setFixedHeight(knUi->height(330));
    //Create all widgets.
    for(int i=0; i<ButtonCount; ++i)
    {
        m_buttons[i] = new QPushButton(this);
    }
    for(int i=0; i<LabelCount; ++i)
    {
        m_labels[i] = new QLabel(this);
        m_labels[i]->setAlignment(Qt::AlignRight);
    }
    for(int i=0; i<MatchOptionCount; ++i)
    {
        m_matchOption[i] = new QCheckBox(this);
    }
    //Create the main layout.
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    //Add tab bar.
    mainLayout->addWidget(m_tabBar);
    QWidget *container = new QWidget(this);
    container->setLayout(m_layout);
    container->lower();
    mainLayout->addWidget(container, 1);
    //Configure the layout.
    m_layout->setContentsMargins(knUi->margins(5, 10, 5, 5));
    m_layout->setSpacing(knUi->width(5));
    //Configure frames.
    //Add default widgets.
    m_layout->setColumnStretch(2, 1);
    m_layout->setRowStretch(5, 1);
    m_layout->addWidget(m_labels[LabelFind], 0, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    m_layout->addWidget(m_findText, 0, 1, 1, 3);
    QBoxLayout *bottomLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    bottomLayout->setContentsMargins(knUi->margins(5, 5, 5, 5));
    mainLayout->addLayout(bottomLayout);
    m_message->setContentsMargins(knUi->margins(5, 0, 5, 0));
    mainLayout->addWidget(m_message);
    bottomLayout->addWidget(m_searchMode);
    bottomLayout->addStretch(2);
    bottomLayout->addWidget(m_transparent);
    //Configure transparent.
    m_transparent->setTitle("  ");
    m_transparentEnable->setAutoFillBackground(true);
    connect(m_transparentEnable, &QCheckBox::stateChanged,
            [=](int state)
    {
        bool enabled = state == Qt::Checked;
        //Set the state.
        m_transOnLose->setEnabled(enabled);
        m_transAlways->setEnabled(enabled);
        m_transValue->setEnabled(enabled);
        //Manually set the state.
        if(!enabled)
        {
            //Clear the transparent settings.
            removeTransparent();
        }
        else
        {
            //Apply the transparent if the always is set.
            if(m_transAlways->isChecked())
            {
                applyTransparent();
            }
        }
    });
    connect(m_transAlways, &QRadioButton::clicked, this, &KNFindWindow::onTransSet);
    connect(m_transOnLose, &QRadioButton::clicked, this, &KNFindWindow::onTransSet);
    QBoxLayout *transLayout = new QBoxLayout(QBoxLayout::TopToBottom,
                                             m_transparent);
    transLayout->setContentsMargins(knUi->margins(7, 10, 6, 0));
    transLayout->setSpacing(knUi->height(1));
    m_transparent->setLayout(transLayout);
    transLayout->addWidget(m_transOnLose);
    transLayout->addWidget(m_transAlways);
    transLayout->addWidget(m_transValue);
    m_transValue->setMaximumHeight(knUi->height(9));
    m_transValue->setRange(50, 200);
    connect(m_transValue, &QSlider::valueChanged,
            [=]
    {
        //Check the current settings.
        if(m_transparentEnable->isChecked() && m_transAlways->isChecked())
        {
            //Update the transparent.
            applyTransparent();
        }
    });
    //Configure options.
    QBoxLayout *optionLayout = new QBoxLayout(QBoxLayout::TopToBottom,
                                              m_searchMode);
    optionLayout->setContentsMargins(knUi->margins(7, 0, 0, 0));
    optionLayout->setSpacing(knUi->height(1));
    m_searchMode->setLayout(optionLayout);
    optionLayout->addWidget(m_optionNormal);
    optionLayout->addWidget(m_optionExtend);
    QBoxLayout *regLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    regLayout->setSpacing(knUi->width(7));
    optionLayout->addLayout(regLayout);
    regLayout->addWidget(m_optionReg);
    regLayout->addWidget(m_dotExtend, 1);
    //Show the wrap and case match options.
    m_selectBox->lower();
    //Add and config tabs.
    m_tabBar->setExpanding(false);
    //Link the tab bar.
    connect(m_tabBar, &QTabBar::currentChanged,
            this, &KNFindWindow::onTabChanged);
    for(int i=0; i<DialogModes; ++i)
    {
        m_tabBar->addTab("");
    }
    //Link the translator.
    knUi->addTranslate(this, &KNFindWindow::retranslate);
    //Link buttons.
    connect(m_buttons[FindNext], &QPushButton::clicked, [=]
    {
        //Check whether the reverse is checked.
        if(m_matchOption[OptionBackward]->isChecked())
        {
            findPrevious();
        }
        else
        {
            findNext();
        }
    });
    connect(m_buttons[Count], &QPushButton::clicked, this, &KNFindWindow::onCount);
    connect(m_buttons[Close], &QPushButton::clicked, this, &KNFindWindow::close);
    //Set the default option.
    //!FIXME: Load from config.
    m_optionNormal->setChecked(true);
    m_transparentEnable->setChecked(true);
    m_transOnLose->setChecked(true);
    m_transValue->setValue(180);
}

void KNFindWindow::showMode(int mode)
{
    //Switch to the mode.
    m_tabBar->setCurrentIndex(mode);
    //Update the option box.
    updateOptionBox();
    //Update the dialog.
    show();
    //Active the dialog.
    activateWindow();
    //Move to find part.
    m_findText->setFocus();
}

void KNFindWindow::resizeEvent(QResizeEvent *event)
{
    //Resize the dialog.
    QDialog::resizeEvent(event);
    //Update box positions.
    updateOptionBox();
    updateSelectBox();
}

void KNFindWindow::showEvent(QShowEvent *event)
{
    //Clear the message box.
    m_message->clear();
    //Show the dialog.
    QDialog::showEvent(event);
    //Update select frame.
    updateSelectBox();
}

bool KNFindWindow::event(QEvent *e)
{
    //Check the event type.
    switch(e->type())
    {
    case QEvent::WindowDeactivate:
        //Check the window opacity settings.
        if(m_transparentEnable->isChecked())
        {
            //Change the window opacity.
            applyTransparent();
        }
        break;
    case QEvent::WindowActivate:
    {
        //Update the window opacity settings.
        if(m_transparentEnable->isChecked() && m_transOnLose->isChecked())
        {
            //Disable the transparent.
            removeTransparent();
        }
        //Check the text cursor has selection or not.
        auto manager = static_cast<KNFileManager *>(parentWidget());
        //Fetch the current editor.
        KNTextEditor *editor = manager->currentEditor();
        if(editor)
        {
            //Update the selection enable state.
            m_inSelection->setEnabled(editor->textCursor().hasSelection());
        }
        break;
    }
    default:
        break;
    }
    return QDialog::event(e);
}

void KNFindWindow::retranslate()
{
    //Update the tab text.
    m_tabBar->setTabText(ModeFind, tr("Find"));
    m_tabBar->setTabText(ModeReplace, tr("Replace"));
    m_tabBar->setTabText(ModeFindInFiles, tr("Find in Files"));
    m_tabBar->setTabText(ModeMark, tr("Mark"));
    //Update widgets.
    m_labels[LabelFind]->setText(tr("Find what :"));
    m_labels[LabelReplace]->setText(tr("Replace with :"));
    m_labels[LabelFilter]->setText(tr("Filters :"));
    //Summary the label size.
    m_inSelection->setText(tr("In select&ion"));
    m_optionNormal->setText(tr("&Normal"));
    m_optionExtend->setText(tr("&Extended (\\n, \\r, \\t, \\0, \\x...)"));
    m_optionReg->setText(tr("Re&gular expression"));
    m_transOnLose->setText(tr("On losing focus"));
    m_transAlways->setText(tr("Always"));
    m_dotExtend->setText(tr("&. matches newline"));
    m_transparentEnable->setText(tr("Transparenc&y"));
    m_matchOption[OptionBookmarkLine]->setText(tr("Book&mark line"));
    m_matchOption[OptionPurge]->setText(tr("Purge for each search"));
    m_matchOption[OptionBackward]->setText(tr("Backward direction"));
    m_matchOption[OptionMatchWhole]->setText(tr("Match &whole word only"));
    m_matchOption[OptionMatchCase]->setText(tr("Match &case"));
    m_matchOption[OptionWrapAround]->setText(tr("Wra&p around"));
    //Update buttons.
    m_buttons[FindNext]->setText(tr("Find Next"));
    m_buttons[Count]->setText(tr("Coun&t"));
    m_buttons[FindAll]->setText(tr("Find All in Current\n&Document"));
    m_buttons[FindAllOpen]->setText(tr("Find All in All &Opened\nDocuments"));
    m_buttons[Close]->setText(tr("Close"));
    m_buttons[Replace]->setText(tr("&Replace"));
    m_buttons[ReplaceAll]->setText(tr("Replace &All"));
    m_buttons[ReplaceAllOpen]->setText(tr("Replace All in All Opened\nDoc&uments"));
    m_buttons[FindFilesAll]->setText(tr("Find All"));
    m_buttons[FindFilesReplace]->setText(tr("Replace in Files"));
    m_buttons[MarkAll]->setText(tr("Mark All"));
    m_buttons[ClearMarks]->setText(tr("Clear all marks"));
    //Clear the message box.
    m_message->clear();
    //Sum the maximum button width.
    int maxWidth = -1;
    for(int i=0; i<ButtonCount; ++i)
    {
        maxWidth = qMax(maxWidth, m_buttons[i]->sizeHint().width());
    }
    for(int i=0; i<ButtonCount; ++i)
    {
        m_buttons[i]->setFixedWidth(maxWidth);
    }
    //Sum the maximum label width.
    maxWidth = -1;
    for(int i=0; i<LabelCount; ++i)
    {
        maxWidth = qMax(maxWidth, m_labels[i]->sizeHint().width());
    }
    maxWidth += (maxWidth >> 1);
    for(int i=0; i<LabelCount; ++i)
    {
        m_labels[i]->setMinimumWidth(maxWidth);
    }
    //Update container widget size.
    for(int i=0; i<MatchOptionCount; ++i)
    {
        m_matchOption[i]->setFixedSize(m_matchOption[i]->sizeHint());
    }
    //Update option box here.
    updateOptionBox();
    //Update the title.
    m_searchMode->setTitle(tr("Search Mode"));
    //Update the window title.
    setWindowTitle(m_tabBar->tabText(m_tabBar->currentIndex()));

}

void KNFindWindow::onTransSet()
{
    if(m_transparentEnable->isChecked() && m_transOnLose->isChecked())
    {
        //Disable the transparent.
        removeTransparent();
    }
    else
    {
        //Apply the transparent right now.
        applyTransparent();
    }
}

void KNFindWindow::onTabChanged(int tabIndex)
{
    //Update the position.
    updateOptionBox();
    //Update the title text.
    setWindowTitle(m_tabBar->tabText(m_tabBar->currentIndex()));
    //Hide all the optional widgets.
    m_matchOption[OptionBookmarkLine]->hide();
    m_matchOption[OptionPurge]->hide();
    m_matchOption[OptionBackward]->hide();
    m_matchOption[OptionWrapAround]->hide();
    m_labels[LabelReplace]->hide();
    m_labels[LabelFilter]->hide();
    m_replaceText->hide();
    m_selectBox->hide();
    m_inSelection->hide();
    m_filters->hide();
    for(int i=0; i<ButtonCount; ++i)
    {
        m_buttons[i]->hide();
    }
    //Based on tab index, show the widgets.
    switch(tabIndex)
    {
    case ModeFind:
        setWidget(m_buttons[FindNext], 0, 5, 1, 1);
        m_buttons[FindNext]->setDefault(true);
        m_selectBox->show();
        setWidget(m_buttons[Count], 1, 5, 1, 1);
        setWidget(m_buttons[FindAll], 2, 5, 1, 1);
        setWidget(m_buttons[FindAllOpen], 3, 5, 1, 1);
        setWidget(m_buttons[Close], 4, 5, 1, 1);
        setWidget(m_inSelection, 2, 3, 1, 1);
        m_matchOption[OptionBackward]->show();
        m_matchOption[OptionWrapAround]->show();
        //Update select frame.
        updateSelectBox();
        break;
    case ModeReplace:
        setWidget(m_buttons[FindNext], 0, 5, 1, 1);
        m_buttons[FindNext]->setDefault(true);
        m_selectBox->show();
        setWidget(m_buttons[Replace], 1, 5, 1, 1);
        setWidget(m_buttons[ReplaceAll], 2, 5, 1, 1);
        setWidget(m_buttons[ReplaceAllOpen], 3, 5, 1, 1);
        setWidget(m_buttons[Close], 4, 5, 1, 1);
        setWidget(m_labels[LabelReplace], 1, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
        setWidget(m_replaceText, 1, 1, 1, 3);
        setWidget(m_inSelection, 2, 3, 1, 1);
        m_matchOption[OptionBackward]->show();
        m_matchOption[OptionWrapAround]->show();
        //Update select frame.
        updateSelectBox();
        break;
    case ModeFindInFiles:
        setWidget(m_buttons[FindFilesAll], 0, 5, 1, 1);
        m_buttons[FindFilesAll]->setDefault(true);
        setWidget(m_labels[LabelReplace], 1, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
        setWidget(m_labels[LabelFilter], 2, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
        setWidget(m_replaceText, 1, 1, 1, 3);
        setWidget(m_filters, 2, 1, 1, 3);
        setWidget(m_buttons[FindFilesReplace], 1, 5, 1, 1);
        setWidget(m_buttons[Close], 2, 5, 1, 1);
        break;
    case ModeMark:
        setWidget(m_buttons[MarkAll], 0, 5, 1, 1);
        m_buttons[MarkAll]->setDefault(true);
        setWidget(m_buttons[ClearMarks], 1, 5, 1, 1);
        setWidget(m_buttons[Close], 2, 5, 1, 1);
        setWidget(m_inSelection, 2, 3, 1, 1);
        m_matchOption[OptionBookmarkLine]->show();
        m_matchOption[OptionPurge]->show();
        m_matchOption[OptionBackward]->show();
        m_matchOption[OptionWrapAround]->show();
        break;
    }
}

void KNFindWindow::onCount()
{
    //Convert the find next.
    auto manager = static_cast<KNFileManager *>(parentWidget());
    //Fetch the current editor.
    KNTextEditor *editor = manager->currentEditor();
    if(!editor)
    {
        return;
    }
    //Perform the search until the end, check the result.
    QTextCursor tc = editor->textCursor();
    quint64 count = 0;
    //Construct the flags and cache.
    QTextDocument::FindFlags flags;
    if(m_matchOption[OptionWrapAround]->isChecked())
    {
        //Then search from top to bottom.
        tc.setPosition(0);
        flags = getSearchFlags(false);
    }
    else
    {
        flags = getSearchFlags(m_matchOption[OptionBackward]->isChecked());
    }
    auto searchCache = createSearchCache();
    //Start the first search.
    tc = cacheSearch(editor, tc, searchCache, flags);
    //Loop and search for the result.
    while(!tc.isNull())
    {
        //Search the next result.
        ++count;
        //Keep searching.
        tc = cacheSearch(editor, tc, searchCache, flags);
    }
    //Update the count result.
    m_message->setText(infoText(tr("Count: %1 match(es).").arg(
                                    QString::number(count))));
}

void KNFindWindow::findNext()
{
    //Convert the find next.
    auto manager = static_cast<KNFileManager *>(parentWidget());
    //Fetch the current editor.
    KNTextEditor *editor = manager->currentEditor();
    if(!editor)
    {
        return;
    }
    //Clear the message box.
    m_message->clear();
    //Perform search.
    auto result = performSearch(editor, editor->textCursor());
    if(result.isNull())
    {
        if(m_matchOption[OptionWrapAround]->isChecked())
        {
            //Check from the start.
            QTextCursor tc = editor->textCursor();
            tc.movePosition(QTextCursor::Start);
            //Find again.
            result = performSearch(editor, tc);
            if(!result.isNull())
            {
                //Set to target position.
                editor->setTextCursor(result);
                return;
            }
        }
        //Can't find the text, show the result.
        m_message->setText(
                    errorText(tr("Find: Can't find the text \"%1\"").arg(
                               m_findText->currentText())));
        return;
    }
    //Set to target position.
    editor->setTextCursor(result);
}

void KNFindWindow::findPrevious()
{
    //Convert the find next.
    auto manager = static_cast<KNFileManager *>(parentWidget());
    //Fetch the current editor.
    KNTextEditor *editor = manager->currentEditor();
    if(!editor)
    {
        return;
    }
    //Clear the message box.
    m_message->clear();
    //Perform search.
    auto result = performSearch(editor, editor->textCursor(), true);
    if(result.isNull())
    {
        if(m_matchOption[OptionWrapAround]->isChecked())
        {
            //Check from the start.
            QTextCursor tc = editor->textCursor();
            tc.movePosition(QTextCursor::End);
            editor->setTextCursor(tc);
            //Find again.
            result = performSearch(editor, tc, true);
            if(!result.isNull())
            {
                editor->setTextCursor(result);
                return;
            }
        }
        //Can't find the text, show the result.
        m_message->setText(
                    errorText(tr("Find: Can't find the text \"%1\"").arg(
                               m_findText->currentText())));
        return;
    }
    //Set to target position.
    editor->setTextCursor(result);
}

void KNFindWindow::setSearchText(const QString &text)
{
    //Set the find text as the target text.
    m_findText->setCurrentText(text);
}

QComboBox *KNFindWindow::generateBox()
{
    QComboBox *textBox = new QComboBox(this);
    textBox->setEditable(true);
    return textBox;
}

KNFindWindow::SearchCache KNFindWindow::createSearchCache()
{
    SearchCache cache;
    if(m_optionNormal->isChecked())
    {
        cache.keywords = m_findText->currentText();
        cache.useReg = false;
    }
    else if(m_optionExtend->isChecked())
    {
        cache.keywords = m_findText->currentText();
        //Replace the keywords.
        cache.keywords.replace("\\n", "\n");
        cache.keywords.replace("\\r", "\r");
        cache.keywords.replace("\\t", "\t");
        cache.keywords.replace("\\0", "\0");
        cache.useReg = false;
    }
    else if(m_optionReg->isChecked())
    {
        //Construct the flags.
        cache.regExp = getRegExp();
        cache.useReg = true;
    }
    return cache;
}

QTextCursor KNFindWindow::performSearch(KNTextEditor *editor,
                                        const QTextCursor &tc,
                                        bool backward)
{
    //Extract the flag.
    return performSearch(editor, tc, getSearchFlags(backward));
}

QTextCursor KNFindWindow::performSearch(KNTextEditor *editor,
                                        const QTextCursor &tc,
                                        QTextDocument::FindFlags flags)
{
    return cacheSearch(editor, tc, createSearchCache(), flags);
}

QTextCursor KNFindWindow::cacheSearch(KNTextEditor *editor,
                                      const QTextCursor &tc,
                                      const KNFindWindow::SearchCache &cache,
                                      QTextDocument::FindFlags flags)
{
    auto doc = editor->document();
    //Check the cache regular expression settings.
    if(cache.useReg)
    {
        return doc->find(cache.regExp, tc, flags);
    }
    //Perform normal search.
    return doc->find(cache.keywords, tc, flags);
}

QTextDocument::FindFlags KNFindWindow::getSearchFlags(bool backward)
{
    auto flags = QTextDocument::FindFlags();
    if(backward)
    {
        flags |= QTextDocument::FindBackward;
    }
    if(m_matchOption[OptionMatchCase]->isChecked())
    {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if(m_matchOption[OptionMatchWhole]->isChecked())
    {
        flags |= QTextDocument::FindWholeWords;
    }
    return flags;
}

QRegularExpression KNFindWindow::getRegExp()
{
    auto regFlags = QRegularExpression::PatternOptions();
    //Append default options.
    regFlags |= QRegularExpression::MultilineOption;
    if(!m_matchOption[OptionMatchCase]->isChecked())
    {
        regFlags |= QRegularExpression::CaseInsensitiveOption;
    }
    if(m_dotExtend->isChecked())
    {
        regFlags |= QRegularExpression::DotMatchesEverythingOption;
    }
    return QRegularExpression(m_findText->currentText(), regFlags);
}

void KNFindWindow::setWidget(QWidget *widget, int row, int column,
                             int rowSpan, int columnSpan, Qt::Alignment align)
{
    //Add the widget to layout.
    m_layout->addWidget(widget, row, column, rowSpan, columnSpan, align);
    //Show the widget.
    widget->show();
}

void KNFindWindow::applyTransparent()
{
    //Set the window transparent based on the value.
    setWindowOpacity(static_cast<float>(m_transValue->value()) / 255.0);
}

void KNFindWindow::removeTransparent()
{
    setWindowOpacity(1.0);
}

void KNFindWindow::updateSelectBox()
{
    //Update the selected frame.
    if(m_selectBox->isVisible())
    {
        //Update the height.
        int yOff = m_tabBar->rect().bottom();
        //Update the frame.
        if(m_tabBar->currentIndex() == 0)
        {
            //Get the top left pos.
            QRect topLeft = m_layout->cellRect(1, 3);
            int boxX = topLeft.x() - knUi->width(3),
                    boxY = topLeft.y() - knUi->height(3) + yOff;
            m_selectBox->move(boxX, boxY);
            QRect bottomRight = m_layout->cellRect(2, 5);
            m_selectBox->resize(bottomRight.right() - boxX + knUi->width(4),
                                bottomRight.bottom() - boxY + knUi->height(5) + yOff);
        }
        else if(m_tabBar->currentIndex() == 1)
        {
            //Get the top left pos.
            QRect topLeft = m_layout->cellRect(2, 3);
            int boxX = topLeft.x() - knUi->width(3),
                    boxY = topLeft.y() - knUi->height(3) + yOff;
            m_selectBox->move(boxX, boxY);
            QRect bottomRight = m_layout->cellRect(2, 5);
            m_selectBox->resize(bottomRight.right() - boxX + knUi->width(4),
                                bottomRight.bottom() - boxY + knUi->width(5) + yOff);
        }
    }
}

void KNFindWindow::updateOptionBox()
{
    //Update option position.
    int optionX = m_searchMode->x() + knUi->width(8),
            optionY = m_searchMode->y() - knUi->height(5);
    for(int i=MatchOptionCount - 1; i>-1; --i)
    {
        optionY -= m_matchOption[i]->sizeHint().height();
        //Update the option position.
        m_matchOption[i]->move(optionX, optionY);
    }
}
