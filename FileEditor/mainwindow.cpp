#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QInputDialog>
#include <QDebug>
#include <QModelIndex>
#include <QtGui>
#include <QThread>
#include <QDir>

#include "codeeditor.h"
#include "notification.h"
#include "searchwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    setWindowIcon(QIcon("resources/icons/appIcon.png"));

    this->statusBar()->setSizeGripEnabled(false);
    this->initMenuBar();
    this->mCodeEditor = new CodeEditor();

    QGridLayout* gridEditor = new QGridLayout();
    gridEditor->addWidget(mCodeEditor);
    ui->frame_editor->setLayout(gridEditor);

    this->mLabelFilename = new QLabel(this);
    this->mLabelFilename->setStyleSheet("color: #eee; font-style: italic;");

    ui->statusbar->addWidget(mLabelFilename);

    this->mNotification = new Notification(this);

    this-> mSearch = new SearchWidget(mCodeEditor);
    this->mSearch->hide();

    connect(mSearch, &SearchWidget::search, this->mCodeEditor, &CodeEditor::search);
}

MainWindow::~MainWindow(){
    delete mCodeEditor;
    delete mNotification;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    if (mCodeEditor) {
        this->setSearchWidgetGeometry();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    int key = event->key();
    if(event->modifiers() == Qt::ControlModifier && key == Qt::Key_F) {
        mSearch->setSearchText("");
        if (mCodeEditor) {
            this->setSearchWidgetGeometry();
            this->mSearch->setSearchFocus();
        }
        this->mSearch->show();

        QTextCursor cursor = mCodeEditor->textCursor();

        // Check if there is a selection
        if (cursor.hasSelection()) {
            // Retrieve the selected text
            QString selectedText = cursor.selectedText();
            if (selectedText.size() > 0){
                mSearch->setSearchText(selectedText);
            }
        }
    }

    if(event->modifiers() == Qt::ControlModifier && key == Qt::Key_S) {
        fileSave();
    }
    if((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && key == Qt::Key_S) {
        fileSaveAs();
    }
}

void MainWindow::fileCreate(){
    mCodeEditor->setReadOnly(false);
    mFilename = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
    if(mFilename.size() > 0){
        this->mCodeEditor->clear();
        this->mLabelFilename->setText(this->mFilename);
        this->mCodeEditor->setFileExtension(this->mFilename);
        this->mCodeEditor->updateSyntaxHighlighter();
    }
}

void MainWindow::fileOpen(){
    this->mCodeEditor->clear();
    this->mCodeEditor->setReadOnly(false);
    this->mFilename = QFileDialog::getOpenFileName(this, "Open file", this->mFilename, "Text files (*.*)");

    if(this->mFilename==""){
        return;
    }
    QFile file(mFilename);
    if(file.open(QIODevice::ReadWrite)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buf = stream.readAll();
        this->mCodeEditor->setSourceText(buf);
        this->mCodeEditor->appendPlainText(buf);
        this->mLabelFilename->setText(this->mFilename);
        this->mCodeEditor->setFileExtension(this->mFilename);
        this->mCodeEditor->updateSyntaxHighlighter();
    }else{
        QMessageBox mBox;
        mBox.setWindowIcon(QIcon("resources/icons/appIcon.png"));
        mBox.setIcon(QMessageBox::Warning);
        mBox.setText("File opening error!");
        mBox.setButtonText(QMessageBox::Ok, "Ok");
        mBox.exec();
    }
    file.close();
}

void MainWindow::fileOpen(QString &path){
    this->mFilename = path;
    if (mFilename == ""){
        return;
    }

    QFile file(mFilename);
    if(file.open(QIODevice::ReadWrite)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buf = stream.readAll();
        this->mCodeEditor->setPlainText(buf);
        this->mLabelFilename->setText(this->mFilename);
        this->mCodeEditor->setFileExtension(this->mFilename);
        this->mCodeEditor->updateSyntaxHighlighter();
        this->mCodeEditor->setNeedSave(false);
    }else{
        QMessageBox mBox;
        mBox.setWindowIcon(QIcon("resources/icons/appIcon.png"));
        mBox.setIcon(QMessageBox::Warning);
        mBox.setText("File opening error!");
        mBox.setButtonText(QMessageBox::Ok, "Ok");
        mBox.exec();
    }
    file.close();
}

void MainWindow::initMenuBar(){
    QMenu *menuFile = menuBar()->addMenu("&File");
    auto *openFile = new QAction("&Open", this);
    auto *createFile = new QAction("&Create", this);
    auto *saveFile = new QAction("&Save", this);
    auto *saveAsFile = new QAction("&Save as", this);
    auto *closeFile = new QAction("&Close", this);

    menuFile->addAction(openFile);
    menuFile->addAction(createFile);
    menuFile->addAction(saveFile);
    menuFile->addAction(saveAsFile);
    menuFile->addAction(closeFile);

    connect(createFile, SIGNAL(triggered()), this, SLOT(fileCreate()));
    connect(openFile, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(saveFile, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(saveAsFile, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(closeFile, SIGNAL(triggered()), this, SLOT(fileClose()));
}

void MainWindow::setSearchWidgetGeometry(){
    int editorWidth = mCodeEditor->width();
    int searchWidth = mSearch->width();
    int searchHeight = mSearch->height();

    this->mSearch->setGeometry(editorWidth - searchWidth - 20, 0, searchWidth, searchHeight);
}

void MainWindow::fileSave(){
    QFileInfo info(this->mFilename);

    if(info.exists()){
        QFile file(this->mFilename);
        if(file.open(QIODevice::WriteOnly)){
            QString buf = mCodeEditor->toPlainText();
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            stream << buf;
            file.close();
            this->mNotification->setNotificationText("Saved");
            this->mNotification->show();

            mCodeEditor->setNeedSave(false);
        }
    }
    else{
        this->fileSaveAs();
    }
}

void MainWindow::fileSaveAs(){
    this->mFilename = QFileDialog::getSaveFileName(this,"Save As", this->mFilename, "Text files (*.*)");
    if(mFilename==""){
        return;
    }

    QFile file(this->mFilename);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buf = mCodeEditor->toPlainText();
        stream << buf;
        file.close();
        this->mNotification->setNotificationText("Saved");
        this->mNotification->show();

        mCodeEditor->setNeedSave(false);
    }
}

void MainWindow::fileClose(){
    int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?" , QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No, QMessageBox::Save);
    if(ret == QMessageBox::Save){
        this->mLabelFilename->setText("");
        QFile file(mFilename);
        if(!(file.open(QIODevice::WriteOnly | QIODevice::Text))){
            this->fileSave();
        }
        else{
            this->fileSaveAs();
        }
        return;
    }
    else if(ret == QMessageBox::Cancel){
        return;
    }
    else{
        this->mFilename = "";
        this->mCodeEditor->clear();
        this->mLabelFilename->setText("");
        this->mCodeEditor->setFileExtension("");
        this->mCodeEditor->updateSyntaxHighlighter();
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent* /*event*/){

    if (this->mCodeEditor->needSave()){
        int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?" , QMessageBox::Save | QMessageBox::No, QMessageBox::Save);
        if(ret == QMessageBox::Save){
            this->fileSave();
        }
    }

    QApplication::closeAllWindows();
}
