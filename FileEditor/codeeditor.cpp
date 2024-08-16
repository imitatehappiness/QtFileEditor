#include "codeeditor.h"

#include <QPainter>
#include <QDebug>
#include <QTextBlock>
#include <QKeyEvent>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent){
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

void CodeEditor::search(const QString& str){
    const QColor highlightColor("green");

    QList<QTextEdit::ExtraSelection> sel;
    moveCursor(QTextCursor::Start);
    while(find(str)){
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(highlightColor);
        extra.cursor = textCursor();
        sel.append(extra);
    }
    if(!sel.isEmpty()){
        setExtraSelections(sel);
    }
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

void CodeEditor::resizeEvent(QResizeEvent* event){
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    this->mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), this->lineNumberAreaWidth(), cr.height()));
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
