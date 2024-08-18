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
#include <QPushButton>
#include <QTabBar>

#include "codeeditor.h"
#include "notification.h"
#include "searchwidget.h"

// Основной конструктор и деструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setWindowOpacity(0.97);
    setWindowIcon(QIcon("resources/icons/appIcon.png"));

    this->statusBar()->setSizeGripEnabled(false);
    this->initMenuBar();

    mTabWidget = new QTabWidget();
    mTabWidget->setContentsMargins(0, 0, 0, 0); // Убирает отступы для QTabWidget

    mTabWidget->setTabsClosable(true);
    mTabWidget->setMovable(true);

    connect(mTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(mTabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentTab);

    QPushButton *addTabButton = new QPushButton(this);
    addTabButton->setIcon(QIcon(":resources/icons/plus.png"));
    addTabButton->setStyleSheet("background-color: rgb(24,24,24); border: 1px solid rgb(24,24,24); margin-left: 10px; padding-right: 10px; padding-bottom: 1px;");
    mTabWidget->setCornerWidget(addTabButton, Qt::TopRightCorner);
    connect(addTabButton, &QPushButton::clicked, this, &MainWindow::addNewTab);

    mNotification = new Notification(this);
    mSearch = new SearchWidget(this);
    mSearch->hide();
    mLabelFilename = new QLabel();
    this->mLabelFilename->setStyleSheet("color: #eee; font-style: italic;");
    ui->statusbar->addWidget(mLabelFilename);

    addNewTab();
    setCentralWidget(mTabWidget);
}

MainWindow::~MainWindow() {
    qDeleteAll(mCodeEditors);
    delete ui;
}

void MainWindow::addNewTab() {
    // Создание новых объектов для вкладки
    CodeEditor *newEditor = new CodeEditor();
    this->mCodeEditors.append(newEditor);

    QString filename = "Untitled";

    QLabel *newLabel = new QLabel(this);
    newLabel->setStyleSheet("color: #eee; font-style: italic;");
    this->mFilenames.append(filename);

    QWidget *newTab = new QWidget();
    newTab->setStyleSheet("background-color: rgb(24,24,24); border: 1px solid rgb(24,24,24);");
    QVBoxLayout *layout = new QVBoxLayout(newTab);

    layout->addWidget(newEditor);
    newTab->setLayout(layout);

    mTabWidget->addTab(newTab, filename);
    mTabWidget->setCurrentWidget(newTab);

    ui->statusbar->addWidget(newLabel);

    connect(mSearch, &SearchWidget::search, newEditor, &CodeEditor::search);

    mLabelFilename->setText(filename);

    mTabWidget -> tabBar() -> setExpanding(true);

}


// Закрытие вкладки
void MainWindow::closeTab(int index) {
    if (index >= 0 && index < mTabWidget->count()) {
        QWidget *widget = mTabWidget->widget(index);
        if (widget) {
            mTabWidget->removeTab(index);
            delete widget;
        }

        if (index < mCodeEditors.size()) {
            mCodeEditors.remove(index);
            mFilenames.remove(index);
        }

        if (mCodeEditors.size() > 0){
            index = mTabWidget->currentIndex();
            mLabelFilename->setText(mFilenames[index]);
        }

    }

    if (index==0 && mCodeEditors.size() == 0){
        addNewTab();
    }
}

// Обновление текущей вкладки
void MainWindow::updateCurrentTab(int index) {
    qDebug() << "index: " << index;
    qDebug() << "mFilenames: " << mFilenames.size();
    if (index >= 0){
        mLabelFilename->setText(mFilenames[index]);
    }else if(mFilenames.size() >= 1){
        mLabelFilename->setText(mFilenames[0]);
    }

}

// Обработка события изменения размера окна
void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    setSearchWidgetGeometry();
}

// Обработка нажатий клавиш
void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    int index = mTabWidget->currentIndex();

    if (mCodeEditors.size() > 0 && event->modifiers() == Qt::ControlModifier && key == Qt::Key_F) {
        setSearchWidgetGeometry();
        mSearch->setSearchFocus();
        mSearch->show();

        QTextCursor cursor = mCodeEditors[index]->textCursor();
        if (cursor.hasSelection()) {
            QString selectedText = cursor.selectedText();
            if (selectedText.size() > 0) {
                mSearch->setSearchText(selectedText);
            }
        }
    }

    if (event->modifiers() == Qt::ControlModifier && key == Qt::Key_S) {
        fileSave();
    }
    if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && key == Qt::Key_S) {
        fileSaveAs();
    }
}

// Установка геометрии виджета поиска
void MainWindow::setSearchWidgetGeometry() {

    if (mCodeEditors.size() > 0){
        int index = mTabWidget->currentIndex();
        int editorWidth = mCodeEditors[index]->width();
        int searchWidth = mSearch->width();
        int searchHeight = mSearch->height();

        mSearch->setGeometry(editorWidth - searchWidth - 20, 61, searchWidth, searchHeight);
        mSearch->raise();
    }
}

// Создание нового файла
void MainWindow::fileCreate() {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < mCodeEditors.size()) {
        mCodeEditors[index]->setReadOnly(false);
        QString filename = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
        if (filename.size() > 0) {
            mCodeEditors[index]->clear();
            mLabelFilename->setText(filename);
            mCodeEditors[index]->setFileExtension(filename);
            mCodeEditors[index]->updateSyntaxHighlighter();

            QString tabName = QFileInfo(filename).fileName();
            mTabWidget->setTabText(index, tabName);

            mLabelFilename->setText(filename);
        }
    }
}

// Открытие файла
void MainWindow::fileOpen() {
    int index = mTabWidget->currentIndex();
    qDebug() << "Current index changed to:" << index;
    if (index >= 0 && index < mCodeEditors.size()) {
        mCodeEditors[index]->clear();
        mCodeEditors[index]->setReadOnly(false);
        QString filename = QFileDialog::getOpenFileName(this, "Open file", "", "Text files (*.*)");

        if (filename == "") {
            return;
        }
        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            QString buf = stream.readAll();
            mCodeEditors[index]->setSourceText(buf);
            mCodeEditors[index]->appendPlainText(buf);
            mLabelFilename->setText(filename);
            mCodeEditors[index]->setFileExtension(filename);
            mCodeEditors[index]->updateSyntaxHighlighter();
            mFilenames[index] = filename;

            QString tabName = QFileInfo(filename).fileName();
            mTabWidget->setTabText(index, tabName.length() > 15 ? "..." + tabName.right(15) : tabName);

            mLabelFilename->setText(filename);
        } else {
            QMessageBox mBox;
            mBox.setWindowIcon(QIcon("resources/icons/appIcon.png"));
            mBox.setIcon(QMessageBox::Warning);
            mBox.setText("File opening error!");
            mBox.setButtonText(QMessageBox::Ok, "Ok");
            mBox.exec();
        }
        file.close();
    }
}

// Открытие файла с заданным путем
void MainWindow::fileOpen(QString &path) {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < mCodeEditors.size()) {
        QString filename = path;
        if (filename == "") {
            return;
        }

        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            QString buf = stream.readAll();
            mCodeEditors[index]->setPlainText(buf);
            mLabelFilename->setText(filename);
            mCodeEditors[index]->setFileExtension(filename);
            mCodeEditors[index]->updateSyntaxHighlighter();
            mFilenames[index] = filename;

            QString tabName = QFileInfo(filename).fileName();
            mTabWidget->setTabText(index, tabName.length() > 15 ? "..." + tabName.right(15) : tabName);
            mLabelFilename->setText(filename);
        } else {
            QMessageBox mBox;
            mBox.setWindowIcon(QIcon("resources/icons/appIcon.png"));
            mBox.setIcon(QMessageBox::Warning);
            mBox.setText("File opening error!");
            mBox.setButtonText(QMessageBox::Ok, "Ok");
            mBox.exec();
        }
        file.close();
    }
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

// Сохранение файла
void MainWindow::fileSave() {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < mCodeEditors.size()) {
        QFileInfo info(mFilenames[index]);

        if (info.exists()) {
            QFile file(mFilenames[index]);
            if (file.open(QIODevice::WriteOnly)) {
                QString buf = mCodeEditors[index]->toPlainText();
                QTextStream stream(&file);
                stream.setCodec("UTF-8");
                stream << buf;
                file.close();
                mNotification->setNotificationText("Saved");
                mNotification->show();

                mCodeEditors[index]->setNeedSave(false);
            }
        } else {
            fileSaveAs();
        }
    }
}

// Сохранение файла как новый
void MainWindow::fileSaveAs() {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < mCodeEditors.size()) {
        QString filename = QFileDialog::getSaveFileName(this, "Save As", mFilenames[index], "Text files (*.*)");
        if (filename == "") {
            return;
        }

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            QString buf = mCodeEditors[index]->toPlainText();
            stream << buf;
            file.close();
            mNotification->setNotificationText("Saved");
            mNotification->show();

            mCodeEditors[index]->setNeedSave(false);
        }
    }
}

// Закрытие файла
void MainWindow::fileClose() {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < mCodeEditors.size()) {
        int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?\n" + mFilenames[index], QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No, QMessageBox::Save);
        if (ret == QMessageBox::Save) {
            fileSave();
        } else if (ret == QMessageBox::Cancel) {
            return;
        } else {
            mCodeEditors[index]->clear();
            mLabelFilename->setText("");
            mCodeEditors[index]->setFileExtension("");
            mCodeEditors[index]->updateSyntaxHighlighter();
        }
    }
}

// Обработка события закрытия окна
void MainWindow::closeEvent(QCloseEvent* /*event*/) {
    for (int i = 0; i < mCodeEditors.size(); ++i) {
        if (mCodeEditors[i]->needSave()) {
            int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?\n" + mFilenames[i], QMessageBox::Save | QMessageBox::No, QMessageBox::Save);
            if (ret == QMessageBox::Save) {
                fileSave();
            }
        }
    }

    QApplication::closeAllWindows();
}
