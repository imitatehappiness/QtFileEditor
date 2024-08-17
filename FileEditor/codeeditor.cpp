#include "codeeditor.h"

#include <QPainter>
#include <QDebug>
#include <QTextBlock>
#include <QKeyEvent>
#include <QFileInfo>

#include "cppsyntaxhighlighter.h"
#include "types.h"

CodeEditor::CodeEditor(QWidget *parent, const QString &text) : QPlainTextEdit(parent) {
    this->setSourceText(text);

    this->mLineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    setLineWrapMode(QPlainTextEdit::NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    this->updateLineNumberAreaWidth(0);
    this->highlightCurrentLine();
    setFocus();

    mSearchForward = forwardTypes::all;
    mNeedSave = false;

    connect(this, &QPlainTextEdit::textChanged, this, &CodeEditor::onTextChanged);
}

int CodeEditor::lineNumberAreaWidth(){
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    const int baseWidth = 3;
    const int additionalPadding = 30;
    int space = baseWidth + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space + additionalPadding;
}

void CodeEditor::setFileExtension(QString filename){
    QFileInfo fileInfo(filename);
    mFileExt = fileInfo.suffix();
}

void CodeEditor::search(const QString& str, forwardTypes forward) {
    const QColor highlightColor("#376489");
    const QColor textColor("#EEEEEE");

    if (str.isEmpty()) {
        setExtraSelections({});
        return;
    }

    QList<QTextEdit::ExtraSelection> sel;
    QTextCursor cursor = textCursor(); // Get the current cursor
    QTextDocument::FindFlags flags = QTextDocument::FindCaseSensitively;

    if (forward == forwardTypes::all) {
        QTextCursor startCursor = QTextCursor(document()->begin()); // Start from the beginning of the document
        QTextCursor result = document()->find(str, startCursor, flags);

        while (!result.isNull()) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(highlightColor);
            extra.format.setForeground(textColor);
            extra.cursor = result;
            sel.append(extra);

            startCursor = result; // Update the start cursor
            result = document()->find(str, startCursor, flags);
        }

        setExtraSelections(sel);
    } else if (forward == forwardTypes::prev) {
        QTextCursor startCursor = textCursor(); // Start from the current cursor
        QTextCursor result = document()->find(str, startCursor, QTextDocument::FindBackward | flags);

        if (result.isNull()) {
            startCursor.movePosition(QTextCursor::End);
            startCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            result = document()->find(str, startCursor, QTextDocument::FindBackward | flags);
        }

        if (!result.isNull()) {
            if (result.selectionStart() >= startCursor.selectionStart()){
                QTextEdit::ExtraSelection extra;
                extra.format.setBackground(highlightColor);
                extra.cursor = result;
                sel.append(extra);
                setExtraSelections(sel);
                setTextCursor(result); // Move the cursor to the found occurrence
            } else {
                startCursor.movePosition(QTextCursor::End);
                startCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                if (!result.isNull()) {
                    QTextEdit::ExtraSelection extra;
                    extra.format.setBackground(highlightColor);
                    extra.cursor = result;
                    sel.append(extra);
                    setExtraSelections(sel);
                    setTextCursor(result); // Move the cursor to the found occurrence
                } else {
                    setExtraSelections({});
                }
            }
        } else {
            setExtraSelections({});
        }
    } else if (forward == forwardTypes::next) {
        QTextCursor startCursor = textCursor(); // Start from the current cursor
        QTextCursor result = document()->find(str, startCursor, flags);

        if (result.isNull()) {
            // If nothing is found, search from the beginning of the document
            startCursor = QTextCursor(document()->begin());
            result = document()->find(str, startCursor, flags);
        }

        if (!result.isNull()) {
            // If found and within the valid range
            if (result.selectionStart() >= startCursor.selectionStart()) {
                QTextEdit::ExtraSelection extra;
                extra.format.setBackground(highlightColor);
                extra.cursor = result;
                sel.append(extra);
                setExtraSelections(sel);
                setTextCursor(result); // Move the cursor to the found occurrence
            } else {
                // If result does not exceed the start cursor, initialize search from the beginning
                startCursor = QTextCursor(document()->begin());
                result = document()->find(str, startCursor, flags);
                if (!result.isNull()) {
                    QTextEdit::ExtraSelection extra;
                    extra.format.setBackground(highlightColor);
                    extra.cursor = result;
                    sel.append(extra);
                    setExtraSelections(sel);
                    setTextCursor(result); // Move the cursor to the found occurrence
                } else {
                    setExtraSelections({});
                }
            }
        } else {
            setExtraSelections({});
        }
    }
}

void CodeEditor::onTextChanged() {
    QString currentText = this->toPlainText();
    currentText.replace("\r\n", "\n");

    this->mSourceText.replace("\r\n", "\n");
    this->mNeedSave = this->mSourceText != currentText;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */){
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy){
    if (dy != 0) {
        this->mLineNumberArea->scroll(0, dy);
    } else {
        this->mLineNumberArea->update(0, rect.y(), this->mLineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())){
        this->updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::setSourceText(const QString &newSourceText){
    this->mSourceText = newSourceText;
}

void CodeEditor::setNeedSave(const bool &newNeedSave){
    this->mNeedSave = newNeedSave;
}

bool CodeEditor::needSave() const{
    return mNeedSave;
}

void CodeEditor::resizeEvent(QResizeEvent* event){
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    this->mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), this->lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::updateSyntaxHighlighter(){
    if (mFileExt == "cpp" || mFileExt == "h"){
        new CppSyntaxHighlighter(this->document());
    }
    new CppSyntaxHighlighter();
}

void CodeEditor::highlightCurrentLine(){
    if (!isReadOnly()) {
        const QColor highlightColor = QColor(24, 24, 23).lighter(160);

        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(highlightColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);

        setExtraSelections(extraSelections);
    }
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event){
    const QColor lineNumberAreaColor(24, 24, 24);
    const QColor separatorLineColor(33, 33, 33);
    const QColor lineNumberColor("green");
    const QFont lineNumberFont("Consolas", 12);

    QPainter painter(this->mLineNumberArea);
    painter.fillRect(event->rect(), lineNumberAreaColor);

    painter.setPen(separatorLineColor);
    painter.drawLine(QLine(event->rect().topRight(), event->rect().bottomRight()));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(lineNumberColor);
            painter.setFont(lineNumberFont);
            painter.drawText(0, top, mLineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignHCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
