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
#ifndef KNTEXTEDITOR_H
#define KNTEXTEDITOR_H

#include <QAction>
#include <QFuture>
#include <QJsonObject>

#include "kntextsearcher.h"

#include <QPlainTextEdit>

class QTextCodec;
class KNSyntaxHighlighter;
class KNTextBlockData;
class KNTextEditorPanel;
/*!
 * \brief The KNTextEditor class provides the text edit and view widget.
 */
class KNTextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    enum LetterCase
    {
        Upper,
        Lower,
        Proper,
        Reverse,
        Random
    };

    enum ShowSymbolMode
    {
        SymbolNormal,
        SymbolSpaces,
        SymbolEndOfBlock,
        SymbolAll
    };

    /*!
     * \brief Guess the codec from the exist data.
     * \param data The binary data to guess the string.
     * \param convert The convert result buffer.
     * \return The best match text codec.
     */
    static QTextCodec *codecFromData(const QByteArray &data,
                                     QString *convert = nullptr);

    /*!
     * \brief Construct a KNTextEditor widget.
     * \param titleName The tab name of the editor.
     * \param parent The parent widget.
     */
    explicit KNTextEditor(const QString &titleName,
                          const QString &filePath = QString(),
                          const QString &codec = QString(),
                          QWidget *parent = nullptr,
                          KNSyntaxHighlighter *highlighter = nullptr,
                          bool linkWithGlobal = true);

    /*!
     * \brief Draw the side bar panel.
     * \param painter The panel painter.
     * \param lineNumWidth The line number area width.
     * \param markWidth The line mark width.
     * \param markFold The fold panel width.
     */
    void paintSidebar(QPainter *painter, int lineNumWidth,
                      int markWidth, int foldWidth);

    /*!
     * \brief Create a text editor with the file open.
     * \param filePath The file path.
     * \param codec The codec of the file.
     * \param parent The parent widget of the editor.
     * \return The editor codec.
     */
    static KNTextEditor *fromFile(const QString &filePath,
                                  const QString &codec = QString(),
                                  QWidget *parent = nullptr);

    /*!
     * \brief Get the current file path.
     * \return The file path. If this editor contains an untitled document,
     * return an empty string.
     */
    QString filePath() const;

    /*!
     * \brief Get the session object of the current editor.
     * \return The editor session description.
     */
    QJsonObject sessionObject() const;

    /*!
     * \brief Restore the session states from the session object.
     * \param sess The json object of the session.
     */
    void loadSessionStates(const QJsonObject &sess);

    /*!
     * \brief Reimplemented from QPlainTextEdit::setOverwriteMode().
     */
    void setOverwriteMode(bool overwrite);

    /*!
     * \brief Check whether the file is saved at local disk.
     * \return If the file is saved, return true.
     */
    bool isOnDisk() const;

    /*!
     * \brief Reimplemented from QPlainTextEdit::setReadOnly().
     */
    void setReadOnly(bool ro);

    /*!
     * \brief Get the specific block data of the text block.
     * \param block The text block from the text editor.
     * \return The text block data.
     */
    KNTextBlockData *blockData(const QTextBlock &block);

    /*!
     * \brief Get the block data at the cursor line.
     * \return The cursor position block data.
     */
    KNTextBlockData *currentBlockData();

    /*!
     * \brief Check whether the text edit can do copy.
     * \return If there is any selection, return true.
     */
    bool canCopy() const;

    /*!
     * \brief Append a editor related connection to the cache.
     * \param connection The connection object.
     */
    void addLink(const QMetaObject::Connection &connection);

    /*!
     * \brief Remove all the cached connections.
     */
    void removeAllLinks();

    /*!
     * \brief Find the first non-space character position in the block.
     * \param block The text block.
     * \return The character position of the first non-space char.
     */
    static int firstNonSpacePos(const QTextBlock &block);

    /*!
     * \brief Get the celling space position of the a existed position.
     * \param spacePos The space position.
     * \param tabSpacing The tab spacing setting.
     * \return The space position of the nearest tab.
     */
    static int cellTabSpacing(int spacePos, int tabSpacing);

    /*!
     * \brief Get the file codec name.
     * \return The codec name of the current file.
     */
    QString codecName() const;

    /*!
     * \brief Get the current selected text.
     * \return The current selected text.
     */
    QString selectedText() const;

    /*!
     * \brief Reimplemented from QPlainTextEdit::find().
     */
    bool find(const QString &exp,
              QTextDocument::FindFlags options = QTextDocument::FindFlags());

    /*!
     * \brief Reimplemented from QPlainTextEdit::find().
     */
    bool find(const QRegularExpression &exp,
              QTextDocument::FindFlags options = QTextDocument::FindFlags());

    /*!
     * \brief Set whether the cursor should be visible or not.
     * \param yes To Make the cursor visible, set yes to true.
     */
    void setCursorVisible(bool yes);

    /*!
     * \brief Set whether the line number is visible or not.
     * \param yes To make the line number visible, set yes to true.
     */
    void setLineNumberVisible(bool yes);

    /*!
     * \brief Set whether the bookmark is visible or not.
     * \param yes To make the bookmark visible, set yes to true.
     */
    void setBookmarkVisible(bool yes);

    /*!
     * \brief Set whether editor would highlight the cursor line.
     * \param yes To highlight the cursor, set yes to true.
     */
    void setHighlightCursor(bool yes);

    void moveCurrentBlockUp();
    void moveCurrentBlockDown();

signals:
    /*!
     * \brief When the title of the document is changed, this signal is emitted.
     */
    void documentTitleChange();

    /*!
     * \brief When the text cursor position changed, this signal is emitted.
     * \param row The row of the file.
     * \param column The column of the file.
     * \param selected The length of the selected area.
     * \param selectedRow The rows of the selected area.
     */
    void cursorPosUpdate(int row, int column, int selected, int selectedRows);

    /*!
     * \brief When the total length is changed, this signal is emitted.
     * \param length The length of the file.
     * \param lines The lines of the file.
     */
    void lengthChange(int length, int lines);

    /*!
     * \brief When overwrite mode is change, this signal is emitted.
     * \param isOverwrite If the editor is in overwrite mode, this is true.
     */
    void overwriteModeChange(bool isOverwrite);

    /*!
     * \brief When read-only state is change, this signal is emitted.
     * \param isReadOnly If the editor is read-only, this is true.
     */
    void readOnlyChange(bool isReadOnly);

    /*!
     * \brief When the codec is changed, this signal is emitted.
     * \param codec The codec name of the file.
     */
    void fileCodecChange(const QString &codec);

public slots:
    /*!
     * \brief Reimplemented from QPlainTextEdit::undo().
     */
    void undo();

    /*!
     * \brief Reimplemented from QPlainTextEdit::redo().
     */
    void redo();

    /*!
     * \brief Reimplemented from QPlainTextEdit::cut().
     */
    void cut();

    /*!
     * \brief Reimplemented from QPlainTextEdit::copy().
     */
    void copy();

    /*!
     * \brief Open the file using a different codec.
     * \param codecName The name of the codec.
     */
    void loadUseCodec(const QByteArray &codecName);

    /*!
     * \brief Open the file using a different codec.
     * \param codec The codec pointer.
     */
    void loadUseCodec(QTextCodec *codec);

    /*!
     * \brief Load a specific file from file path to a constructed document.
     * \param filePath The file path.
     * \param codec The pointer to the target codec pointer.
     * \param document The document pointer.
     * \return If the document is successfully loaded, return true.
     */
    static bool loadToDocument(const QString &filePath, QTextCodec **codec,
                               QTextDocument *document);

    /*!
     * \brief Load the file to the current editor with a specific codec.
     * \param filePath The file path.
     * \param codec The codec pointer.
     */
    void loadFrom(const QString &filePath, QTextCodec *codec);

    /*!
     * \brief Load the file to the current editor with a specific codec.
     * \param filePath The file path.
     * \param codecName The file encoding codec.
     */
    void loadFrom(const QString &filePath, const QByteArray &codecName = QByteArray());

    /*!
     * \brief Save the file as current setting.
     * \return If the file is successfully saved as the recorded file path,
     * return true.
     */
    bool save();

    /*!
     * \brief Save the current document as the file path.
     * \param filePath The file path.
     * \return If the file is successfully saved as the file, return true.
     */
    bool saveAs(const QString &filePath);

    /*!
     * \brief Save the current content to target file path.
     * \param filePath The target file path.
     * \return If the file is sucessfully saved, return true.
     */
    bool saveToFile(const QString &filePath);

    /*!
     * \brief Set whether the editor should show invisible chars.
     * \param mode The symbol display mode.
     */
    void setSymbolDisplayMode(int mode);

    /*!
     * \brief This slot is used after the text editor is connected with status
     * bar.
     */
    void syncWithStatusBar();

    /*!
     * \brief Execute the delete operation at the cursor.
     */
    void cursorDelete();

    /*!
     * \brief Indent the current cursor.
     */
    void indentCursor();

    /*!
     * \brief Remove indent at the current cursor.
     */
    void unindentCursor();

    /*!
     * \brief Convert the letter case of the selection text.
     * \param letterCase It should be an instance of LetterCase.
     */
    void convertSelectCase(int letterCase);

    /*!
     * \brief Show the summary information dialog of the current dialog.
     */
    void showSummary();

    /*!
     * \brief Set the saving codec.
     * \param codecName The name of the codec.
     */
    void setCodecName(const QString &codecName);

    /*!
     * \brief Do quick search at the keywords for the current text editor.
     * \param keywords The keywords to be searched.
     * \param cs The case sensitive position.
     * \param position The default text cursor position.
     */
    void quickSearch(const QString &keywords, Qt::CaseSensitivity cs,
                     int position);

    /*!
     * \brief Go to the next quick search result.
     * \param position The text cursor position. To use the current text cursor
     * position, set to -1.
     */
    void quickSearchNext(int position = -1);

    /*!
     * \brief Go to the previous quick search result.
     * \param position The text cursor position. To use the current text cursor
     * position, set to -1.
     */
    void quickSearchPrev(int position = -1);

    /*!
     * \brief Update all the extra selections marks.
     */
    void updateExtraSelections();

protected:
    /*!
     * \brief Reimplemented from QPlainTextEdit::resizeEvent().
     */
    void resizeEvent(QResizeEvent *event) override;

    /*!
     * \brief Reimplemented from QPlainTextEdit::keyPressEvent().
     */
    void keyPressEvent(QKeyEvent *event) override;

    /*!
     * \brief Reimplemented from QPlainTextEdit::mousePressEvent().
     */
    void mousePressEvent(QMouseEvent *event) override;

    /*!
     * \brief Reimplemented from QPlainTextEdit::paintEvent().
     */
    void paintEvent(QPaintEvent *event) override;

    /*!
     * \brief Reimplemented from QPlainTextEdit::scrollContentsBy().
     */
    void scrollContentsBy(int dx, int dy) override;


private slots:
    void onBlockCountChanged(int newBlockCount);
    void updatePanelArea(const QRect &rect, int dy);
    void onCursorPositionChanged();
    void onTextChanged();
    void onCursorUpdate();
    void onEditorFontChanged();
    void onWrapModeChange(bool wrap);
    void onResultDisplayChange(bool showResult);
    void onAlignLeftChange(bool showLeft);
    bool quickSearchForward(const QTextCursor &cursor);
    bool quickSearchBackward(const QTextCursor &cursor);

private:
    enum TextEditorOptions
    {
        CursorVisible      = 1 << 0,
        CursorDisplay      = 1 << 1,
        LineNumberDisplay  = 1 << 2,
        HighlightCursor    = 1 << 3,
    };
    void quickSearchUi(const QTextBlock &block);
    void quickSearchCheck(const QTextBlock &block);
    void updateHighlighter(KNSyntaxHighlighter *highlighter = nullptr);
    QString textLevelString(int spaceLevel, int tabSpacing);
    static int spacePosition(const QTextBlock &block, int textPos,
                             int tabSpacing);
    static int textPosition(const QTextBlock &block, int spacePos,
                            int tabSpacing);
    void setFilePath(const QString &filePath);
    void updateViewportMargins();
//    QList<QTextCursor> columnCopy();
//    QList<QTextCursor> columnSelectionText(QString &selectionText) const;
//    void setColumnCursor(const QTextCursor &cursor);
//    void setColumnCursor(int blockNum, int offset);
//    void clearColumnCursor();
    bool isExtraCursorEnabled();
    bool isVerticalEnabled();
    void appendVerticalCursor(QTextCursor cursor);
    QVector<QTextCursor> m_extraCursors;
    int m_verticalSpacePos;
    bool m_verticalSelect;

    QScopedPointer<KNTextSearcher> m_quickSearchPrev, m_quickSearchNext;
    QFuture<void> m_futurePrev, m_futureNext;
    QString m_quickSearchKeyword;
    Qt::CaseSensitivity m_quickSearchSense;
    unsigned long long int m_quickSearchCode;
    bool m_showResults;

    QList<QMetaObject::Connection> m_connections;
    QTextEdit::ExtraSelection m_currentLine;

    QString m_filePath;
    QByteArray m_codecName;
    KNTextEditorPanel *m_panel;
    KNSyntaxHighlighter *m_highlighter;
    int m_editorOptions;

};

#endif // KNTEXTEDITOR_H
