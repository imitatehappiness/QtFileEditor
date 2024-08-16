#pragma once

#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>

class LineNumberArea;

/*!
 * \class CodeEditor
 * \brief The class responsible for the code editor and its updates
 */
class CodeEditor : public QPlainTextEdit{
    Q_OBJECT
public:
    /// Constructor
    CodeEditor(QWidget *parent = nullptr);
    /// Redraws the line number area widget
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    /// Returns the width of the line number area widget
    int lineNumberAreaWidth();
protected:
    /// Overrides the recommended size method for the widget
    void resizeEvent(QResizeEvent* event);
signals:
    /// Signal to display the text search widget
    void showWidgetFileSearch();
public slots:
    /// Slot for file search
    void search(const QString& str);
private slots:
    /// Slot to update the width of the line number area widget
    void updateLineNumberAreaWidth(int newBlockCount);
    /// Slot to highlight the line where the cursor is located
    void highlightCurrentLine();
    /// Slot to update the line number area widget
    void updateLineNumberArea(const QRect &rect, int dy);
private:
    /// Line number area widget
    QWidget *mLineNumberArea = nullptr;
};

/*!
 * \class LineNumberArea
 * \brief The class responsible for the line number area widget in CodeEditor
 */
class LineNumberArea : public QWidget{
public:
    /// Constructor with parameters
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor){
    }
    /// Overrides the recommended size method for the widget
    QSize sizeHint() const override{
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }
protected:
    /// Overrides the paint event method
    void paintEvent(QPaintEvent *event) override{
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    /// Text editor
    CodeEditor *codeEditor;
};
