#include "codeeditor.h"

#include <QPainter>
#include <QDebug>
#include <QTextBlock>
#include <QKeyEvent>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent){
    mLineNumberArea = new LineNumberArea(this);
    /// blockCountChanged - Этот сигнал выдается всякий раз, когда изменяется количество блоков.
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    /// updateRequest - Этот сигнал выдается, когда текстовому документу требуется обновление указанного rect.
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    /// cursorPositionChanged - Этот сигнал выдается всякий раз, когда изменяется положение курсора.
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);


    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    setFocus();
}


int CodeEditor::lineNumberAreaWidth(){
    int digits = 1;
    /// blockCount() - содержит количество текстовых блоков в документе.
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    /// fontMetrics() - Возвращает показатели отображения шрифта (screen) для шрифта приложения.
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space + 30;
}

void CodeEditor::search(const QString& str){
    QList<QTextEdit::ExtraSelection> sel;
    moveCursor(QTextCursor::Start);
    while(find(str)){
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(QColor("green"));
        extra.cursor = textCursor();
        sel.append(extra);
    }
    if(sel.size() > 0){
        setExtraSelections(sel);
    }
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */){
    /// Устанавливает отступ вокруг области прокрутки на левое, верхнее, правое и нижнее.
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy){
    dy ? mLineNumberArea->scroll(0, dy) : mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* event){
    QPlainTextEdit::resizeEvent(event);
    /// resizeEvent для виджета нумерации строк
    QRect cr = contentsRect();
    mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}


void CodeEditor::highlightCurrentLine(){
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        /// Выделение
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(QColor(24,24,23)).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event){
    QPainter painter(mLineNumberArea);
    /// Заливка области нумерции строк цветом
    painter.fillRect(event->rect(), QColor(24,24,24));

    /// Линия отделающая нумерацию строк и тект файла
    /// painter.setPen(QColor("#eee"));
    /// painter.drawLine(QLine(event->rect().topRight(),event->rect().bottomRight()));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    /// qRound округление в ближайщую сторону
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    /// Отрисовка нумерации
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor("green"));
            painter.setFont(QFont("Consolas", 12));
            painter.drawText(0, top, mLineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignHCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}


