#pragma once

#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

/*!
 * \class CodeEditor
 * \brief Класс отвечающий за редактор кода и его обновление
 */
class CodeEditor : public QPlainTextEdit{
    Q_OBJECT
public:
    /// Конструктор
    CodeEditor(QWidget *parent = nullptr);
    /// Перерисовка виджета нумерации строк
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    /// Ширина виджета нумерации строк
    int lineNumberAreaWidth();
protected:
    /// Переопределение метода рекомендуемого размера виджета
    void resizeEvent(QResizeEvent *event) override;
private slots:
    /// Слот обновления ширины виджета нумерции строк
    void updateLineNumberAreaWidth(int newBlockCount);
    /// Слот выделения строки, на которой находится указатель
    void highlightCurrentLine();
    /// Слот обновление виджета нумерции строк
    void updateLineNumberArea(const QRect &rect, int dy);
private:
    /// Виджет нумерации строк
    QWidget *lineNumberArea;
};

/*!
 * \class LineNumberArea
 * \brief Класс отвечающий за виджет отображения нумерации строк в CodeEditor
 */
class LineNumberArea : public QWidget{
public:
    /// Конструктор с парметрами
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor){
    }
    /// Переопределение метода рекомендуемого размера виджета
    QSize sizeHint() const override{
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }
protected:
    /// Переопределение метода перерисовки
    void paintEvent(QPaintEvent *event) override{
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    /// Редактор текста
    CodeEditor *codeEditor;
};
