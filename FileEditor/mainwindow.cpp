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
#include <QSplitter>
#include <QHeaderView>

#include "codeeditor.h"
#include "notification.h"
#include "searchwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setWindowIcon(QIcon(":/resources/icons/icon.png"));

    initDirTree();
    initTabWidget();

    this->statusBar()->setSizeGripEnabled(false);
    this->initMenuBar();

    QPushButton *addTabButton = new QPushButton(this);
    addTabButton->setIcon(QIcon(":resources/icons/plus.png"));
    addTabButton->setStyleSheet("background-color: rgb(24,24,24); border: 1px solid rgb(24,24,24); margin-left: 10px; padding-right: 10px; padding-bottom: 1px;");

    mTabWidget->setCornerWidget(addTabButton, Qt::TopRightCorner);
    connect(addTabButton, &QPushButton::clicked, this, &MainWindow::addNewTab);
    connect(mTabWidget->tabBar(), &QTabBar::tabMoved, this, &MainWindow::onTabMoved);

    mNotification = new Notification(this);
    mSearch = new SearchWidget(this);
    mSearch->hide();
    mStatusBarLabel = new QLabel();
    this->mStatusBarLabel->setStyleSheet("color: #eee; font-style: italic;");
    ui->statusbar->addWidget(mStatusBarLabel);

    addNewTab();
    ui->frameTree->setMinimumWidth(200);
    ui->splitter->setSizes(QList<int>() << 200 << 700);
}

MainWindow::~MainWindow() {
    qDeleteAll(mCodeEditors);
    delete ui;
}

void MainWindow::addNewTab() {
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

    mStatusBarLabel->setText(filename);

    mTabWidget->tabBar()->setExpanding(true);
}

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
            mTabWidget->setFocus();
        }

        if (mCodeEditors.size() > 0){
            index = mTabWidget->currentIndex();
            mStatusBarLabel->setText(mFilenames[index]);
        }
    }

    if (index==0 && mCodeEditors.size() == 0){
        addNewTab();
    }
}

void MainWindow::updateCurrentTab(int index) {
    if (index >= 0){
        mStatusBarLabel->setText(mFilenames[index]);
    }else if(mFilenames.size() >= 1){
        mStatusBarLabel->setText(mFilenames[0]);
    }

}

void MainWindow::onTabMoved(int from, int to){
    qSwap(this->mFilenames[to], this->mFilenames[from]);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    this->setSearchWidgetGeometry();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    int index = mTabWidget->currentIndex();

    if (mCodeEditors.size() > 0 && event->modifiers() == Qt::ControlModifier && key == Qt::Key_F) {

        if (mSearch->isHidden()){
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
        }else{
            mSearch->hide();
        }
    }

    if (event->modifiers() == Qt::ControlModifier && key == Qt::Key_S) {
        fileSave();
    }
    if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && key == Qt::Key_S) {
        fileSaveAs();
    }
    if ((event->modifiers() & Qt::ControlModifier) && key == Qt::Key_R) {
        addNewTab();
    }
    if ((event->modifiers() & Qt::ControlModifier) && key == Qt::Key_E) {
        int index = mTabWidget->currentIndex();
        closeTab(index);
    }
    if ((event->modifiers() & Qt::ControlModifier) && key == Qt::Key_Q) {
        if (ui->frameTree->isHidden()){
            ui->frameTree->show();
        }else{
            ui->frameTree->hide();
        }
    }
}

void MainWindow::setSearchWidgetGeometry() {

    if (mCodeEditors.size() > 0){
        int searchWidth = mSearch->width();
        int searchHeight = mSearch->height();

        mSearch->setGeometry(ui->centralwidget->width() - searchWidth - 50, 83, searchWidth, searchHeight);
        mSearch->raise();
    }
}

void MainWindow::initDirTree() {
    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath("");

    this->mTree = new CustomTreeView(this);
    this->mTree->setStyleSheet("border: 0px;");
    this->mTree->setModel(model);

    this->mTree->setAnimated(false);
    this->mTree->setIndentation(20);
    this->mTree->setSortingEnabled(true);

    this->mTree->setColumnWidth(0, 100);

    QScroller::grabGesture(this->mTree, QScroller::TouchGesture);

    QVBoxLayout *layout = new QVBoxLayout(ui->frameTree);
    layout->addWidget(this->mTree);
    ui->frameTree->setLayout(layout);

    for (int i = 1; i < model->columnCount(); ++i) {
        this->mTree->setColumnHidden(i, true);
    }

    const QString rootPath = "/";
    if (!rootPath.isEmpty()) {
        const QModelIndex rootIndex = model->index(QDir::cleanPath(rootPath));
        if (rootIndex.isValid())
            this->mTree->setRootIndex(rootIndex);
    }

    this->mTree->header()->setFixedHeight(0);
    model->sort(0, Qt::AscendingOrder);
    connect(this->mTree, SIGNAL(fileOpen(QString&)), this, SLOT(fileOpen(QString&)));
}

void MainWindow::initTabWidget(){
    mTabWidget = new QTabWidget();
    mTabWidget->setContentsMargins(0, 0, 0, 0);

    mTabWidget->setTabsClosable(true);
    mTabWidget->setMovable(true);

    connect(mTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(mTabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentTab);

    QVBoxLayout *layout = new QVBoxLayout(ui->frameEditor);
    layout->addWidget(mTabWidget);
    ui->frameTree->setLayout(layout);
}

void MainWindow::fileCreate() {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < mCodeEditors.size()) {
        mCodeEditors[index]->setReadOnly(false);
        QString filename = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
        if (filename.size() > 0) {
            mCodeEditors[index]->clear();
            mStatusBarLabel->setText(filename);
            mCodeEditors[index]->setFileExtension(filename);
            mCodeEditors[index]->updateSyntaxHighlighter();

            QString tabName = QFileInfo(filename).fileName();
            mTabWidget->setTabText(index, tabName);

            mStatusBarLabel->setText(filename);
        }
    }
}

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
            mStatusBarLabel->setText(filename);
            mCodeEditors[index]->setFileExtension(filename);
            mCodeEditors[index]->updateSyntaxHighlighter();
            mFilenames[index] = filename;

            QString tabName = QFileInfo(filename).fileName();
            mTabWidget->setTabText(index, tabName.length() > 15 ? "..." + tabName.right(15) : tabName);
            mStatusBarLabel->setText(filename);
        } else {
            QMessageBox mBox;
            mBox.setWindowIcon(QIcon(":/resources/icons/icon.png"));
            mBox.setIcon(QMessageBox::Warning);
            mBox.setText("File opening error!");
            mBox.setButtonText(QMessageBox::Ok, "Ok");
            mBox.exec();
        }
        file.close();
    }
}

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
            mCodeEditors[index]->setSourceText(buf);
            mCodeEditors[index]->setPlainText(buf);
            mStatusBarLabel->setText(filename);
            mCodeEditors[index]->setFileExtension(filename);
            mCodeEditors[index]->updateSyntaxHighlighter();
            mFilenames[index] = filename;

            QString tabName = QFileInfo(filename).fileName();
            mTabWidget->setTabText(index, tabName.length() > 15 ? "..." + tabName.right(15) : tabName);
            mStatusBarLabel->setText(filename);
        } else {
            QMessageBox mBox;
            mBox.setWindowIcon(QIcon(":/resources/icons/icon.png"));
            mBox.setIcon(QMessageBox::Warning);
            mBox.setText("File opening error!");
            mBox.setButtonText(QMessageBox::Ok, "Ok");
            mBox.exec();
        }
        file.close();

        mCodeEditors[index]->setNeedSave(false);
    }
}

void MainWindow::initMenuBar(){
    QMenu *menuFile = menuBar()->addMenu("&File");
    auto *openDir = new QAction("&Open dir", this);
    auto *openFile = new QAction("&Open file", this);
    auto *createFile = new QAction("&Create new file", this);
    auto *saveFile = new QAction("&Save", this);
    auto *saveAsFile = new QAction("&Save as", this);
    auto *closeFile = new QAction("&Close", this);

    menuFile->addAction(openDir);
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
    connect(openDir, SIGNAL(triggered()), this, SLOT(openDir()));
}

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
            mStatusBarLabel->setText("");
            mCodeEditors[index]->setFileExtension("");
            mCodeEditors[index]->updateSyntaxHighlighter();
        }
    }
}

void MainWindow::openDir(){
    QString directory = QFileDialog::getExistingDirectory(
        this,
        tr("Select Directory"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!directory.isEmpty()) {
        QFileSystemModel *model = qobject_cast<QFileSystemModel*>(this->mTree->model());
        if (model) {
            const QModelIndex rootIndex = model->index(QDir::cleanPath(directory));
            if (rootIndex.isValid()) {
                this->mTree->setRootIndex(rootIndex);

                model->sort(0, Qt::AscendingOrder);
            } else {
                qDebug() << "Invalid directory index.";
            }
        } else {
            qDebug() << "Model is not valid.";
        }
    } else {
        qDebug() << "No directory selected.";
    }
}

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
