#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <QWidget>

#include <QPlainTextEdit>
#include <QProcess>
#include <QMenu>

class TermWidget : public QWidget
{
    Q_OBJECT
    
    QPlainTextEdit *out;
    int             outEnd;
    
    QProcess       *proc;
    
    QMenu          *menu;
    QAction        *menuActCopy;
    QAction        *menuActPaste;
    QAction        *menuActSelectAll;
    QAction        *menuActClear;
    
#ifdef Q_OS_WIN32
    QByteArray      inp;
#endif
    
public:
    TermWidget(QWidget *parent = 0);
   ~TermWidget();
    void clear();
public slots:
    int  setTextCursorAtInput();
    int  setTextCursorAtEnd();
    void paste();
    void execute();
    void processInput();
protected slots:
    void processOutput();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void error(QProcess::ProcessError error);
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // TERMWIDGET_H
