// https://github.com/sbulgakov/TerminalWidget

#include "termwidget.h"
#include "terminalsyntaxhighlighter.h"

#include <QVBoxLayout>
#ifdef Q_OS_WIN32
#include <QTextCodec>
#endif
#include <QTextBlock>
#include <QTextCursor>

TermWidget::TermWidget(QWidget *parent): QWidget(parent),
    outEnd(0)
{
    out = new QPlainTextEdit(this);
    out->setStyleSheet("padding: 10px;");
    out->setLineWrapMode(QPlainTextEdit::NoWrap);
    out->installEventFilter(this);
    int wid = out->fontMetrics().width(QLatin1Char('9'));
    out->setCursorWidth(wid);
    out->setAcceptDrops(false);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setContentsMargins(0,0,9,9);
    l->addWidget(out);

    proc = new QProcess(this);
    connect(proc,SIGNAL(readyReadStandardOutput()),this,SLOT(processOutput()));
    connect(proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    connect(proc,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));

    menu = new QMenu(this);
    menuActCopy      = menu->addAction(tr("Copy"),  out,  SLOT(copy()),
                                       QKeySequence(Qt::CTRL  + Qt::Key_Insert));
    menuActCopy->setEnabled(false);
    connect(out, SIGNAL(copyAvailable(bool)), menuActCopy, SLOT(setEnabled(bool)));
    menuActPaste     = menu->addAction(tr("Paste"), this, SLOT(paste()),
                                       QKeySequence(Qt::SHIFT + Qt::Key_Insert));
    menuActSelectAll = menu->addAction(tr("Select All"), out, SLOT(selectAll()));
    menu->addSeparator();
    menuActClear     = menu->addAction(tr("Clear"), out, SLOT(clear()));

    execute();

    new TerminalSyntaxHighlighter(out->document());

}

TermWidget::~TermWidget(){

}

void TermWidget::clear() {
    // Clear the output widget
    out->clear();

    // Reset the outEnd to 0
    outEnd = 0;

    // If a process is currently running, kill it
    if (proc->state() == QProcess::Running) {
        proc->kill();
        proc->waitForFinished(); // Ensure the process is completely terminated
        out->clear();
    }

    // Start a new process
    execute();
}

int TermWidget::setTextCursorAtInput(){
    QTextCursor cur = out->textCursor();
    cur.setPosition(outEnd);
    out->setTextCursor(cur);

    return outEnd;
}

int TermWidget::setTextCursorAtEnd(){
    QTextBlock block = out->document()->lastBlock();
    int end = (block.position() + block.length() -1);

    QTextCursor cur = out->textCursor();
    cur.setPosition(end);
    out->setTextCursor(cur);

    return end;
}

void TermWidget::paste(){
    QTextCursor cur = out->textCursor();

    if(outEnd > cur.position()
            || (cur.hasSelection() && outEnd > cur.selectionStart()))
    {
        setTextCursorAtInput();
    }

    out->paste();
}

void TermWidget::execute(){
    proc->setProcessChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_WIN32
    proc->start("cmd");
#else
    proc->start("/bin/sh");
#endif
}

void TermWidget::processInput(){
    int end = setTextCursorAtEnd();

    QString input = out->toPlainText().right(end-outEnd);
#ifndef Q_OS_WIN32
    QByteArray      inp;
#endif
    inp = input.toLatin1().append('\n');

    proc->write(inp);
}

void TermWidget::processOutput(){
#ifdef Q_OS_WIN32
    QByteArray  data  = proc->readAllStandardOutput();

    if(data.startsWith(inp)) data.remove(0, inp.length());

    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    out->appendPlainText(codec->toUnicode(data));
#else
    out->appendPlainText(proc->readAllStandardOutput());
#endif
    outEnd = setTextCursorAtEnd();
}

void TermWidget::finished(int exitCode, QProcess::ExitStatus exitStatus){
    QString str("<b>Process has ");
    if( exitStatus == QProcess::NormalExit ) str += QString("finished");
    else                                     str += QString("crashed");
    str += QString(" with code %1</b>").arg(exitCode);
    out->appendHtml(str);

    proc->setProcessChannelMode(QProcess::SeparateChannels);
}

void TermWidget::error(QProcess::ProcessError error){
    switch(error){
    case QProcess::FailedToStart: out->appendHtml(QString("<b>%1</b>").arg(proc->errorString()));
        break;
    case QProcess::Crashed:       out->appendHtml("<b>Process crashed</b>");               break;
    case QProcess::Timedout:      out->appendHtml("<b>Process timedout</b>");              break;
    case QProcess::WriteError:    out->appendHtml("<b>Error writing to process</b>");      break;
    case QProcess::ReadError:     out->appendHtml("<b>Error reading from process</b>");    break;
    case QProcess::UnknownError:  out->appendHtml("<b>Process encountered error</b>");     break;
    }
}

bool TermWidget::eventFilter(QObject *obj, QEvent *event){

    if(obj == out)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if(keyEvent->key() == Qt::Key_Up
                    ||keyEvent->key() == Qt::Key_Down)
            {
                return true;
            }

            if(keyEvent->key() == Qt::Key_Backspace
                    ||keyEvent->key() == Qt::Key_Left)
            {
                if(outEnd < out->textCursor().position()) return false;

                return true;
            }

            if(keyEvent->key() == Qt::Key_Delete
                    ||keyEvent->key() == Qt::Key_Right)
            {
                if(outEnd <= out->textCursor().position()) return false;

                return true;
            }

            if(keyEvent->key() == Qt::Key_Return)
            {
                processInput();

                return true;
            }

            if(keyEvent->key() == Qt::Key_Home)
            {
                if(outEnd < out->textCursor().position())
                {
                    setTextCursorAtInput();
                }

                return true;
            }

            if(keyEvent->key() == Qt::Key_End)
            {
                if(outEnd <= out->textCursor().position())
                {
                    setTextCursorAtEnd();
                }

                return true;
            }

            if(keyEvent->key() == Qt::Key_Control
                    ||keyEvent->key() == Qt::Key_Shift)
            {
                return false;
            }

            if(keyEvent->key() == Qt::Key_Insert)
            {
                if(keyEvent->modifiers() & Qt::ControlModifier) return false;
                if((keyEvent->modifiers() & Qt::ShiftModifier)
                        && out->canPaste()) paste();

                return true;
            }

            if(outEnd > out->textCursor().position())
            {
                setTextCursorAtEnd();
            }

            return false;
        }
        else if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

            if(mouseEvent->buttons() & Qt::RightButton)
            {
                menuActPaste->setEnabled(out->canPaste());

                menu->exec(mouseEvent->globalPos());

                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
