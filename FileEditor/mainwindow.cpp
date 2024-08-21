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
    , ui(new Ui::MainWindow)
    , mNotification(new Notification(this))
    , mSearch(new SearchWidget(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resources/icons/icon.png"));

    this->initLeftPanel();
    this->initMainPanel();
    this->initStatusBar();
    this->initMenuBar();

    this->addNewTab();
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

    this->mTabWidget->addTab(newTab, filename);
    this->mTabWidget->setCurrentWidget(newTab);

    ui->statusbar->addWidget(newLabel);

    connect(mSearch, &SearchWidget::search, newEditor, &CodeEditor::search);
    connect(mSearch, &SearchWidget::replace, newEditor, &CodeEditor::replace);
    connect(this->mCodeEditors.last(), &CodeEditor::editorUpdateText, this, &MainWindow::updateCharactersCount);

    this->mStatusBarFilePathLabel->setText(filename);

    this->mTabWidget->tabBar()->setExpanding(true);

    this->updateOpenPagesList();
}

void MainWindow::closeTab(int index) {
    if (index >= 0 && index < this->mTabWidget->count()) {

        QWidget *widget = this->mTabWidget->widget(index);
        if (widget) {
            if (this->mCodeEditors[index]->needSave()){
                fileClose();
            }

            this->mTabWidget->removeTab(index);
            delete widget;
        }

        if (index < mCodeEditors.size()) {

            this->mCodeEditors.remove(index);
            this->mFilenames.remove(index);
            this->mTabWidget->setFocus();
        }

        if (mCodeEditors.size() > 0){
            index = this->mTabWidget->currentIndex();
            this->mStatusBarFilePathLabel->setText(mFilenames[index]);
        }
    }

    if (index==0 && mCodeEditors.size() == 0){
        addNewTab();
    }

    this->updateOpenPagesList();
}

void MainWindow::updateCurrentTab(int index) {
    if (index >= 0){
        this->mStatusBarFilePathLabel->setText(mFilenames[index]);
        this->mStatusBarCharactersCountLabel->setText("Characters: " + QString::number(this->mCodeEditors[index]->getCharactersCount()));
    }
}

void MainWindow::onTabMoved(int from, int to){
    qSwap(this->mFilenames[to], this->mFilenames[from]);
    qSwap(this->mCodeEditors[to], this->mCodeEditors[from]);
    this->updateOpenPagesList();
}

void MainWindow::clearTerminal(){
    this->mTermWidget->clear();
}

void MainWindow::updateCharactersCount(int count){
    this->mStatusBarCharactersCountLabel->setText("Characters: " + QString::number(count));
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    this->setSearchWidgetGeometry();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    int index = this->mTabWidget->currentIndex();

    if (this->mCodeEditors.size() > 0 && event->modifiers() == Qt::ControlModifier && key == Qt::Key_F) {

        if (mSearch->isHidden()){
            this->setSearchWidgetGeometry();
            this->mSearch->setSearchFocus();
            this->mSearch->show();

            QTextCursor cursor = mCodeEditors[index]->textCursor();
            if (cursor.hasSelection()) {
                QString selectedText = cursor.selectedText();
                if (selectedText.size() > 0) {
                    this->mSearch->setSearchText(selectedText);
                }
            }
        }else{
            QTextCursor cursor = mCodeEditors[index]->textCursor();
            if (cursor.hasSelection()) {
                QString selectedText = cursor.selectedText();
                if (selectedText.size() > 0) {
                    this->mSearch->setSearchText(selectedText);
                }
            }else{
               this->mSearch->hide();
            }
        }
    }

    if (event->modifiers() == Qt::ControlModifier && key == Qt::Key_S) {
        this->fileSave();
    }
    if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier) && key == Qt::Key_S) {
        this->fileSaveAs();
    }
    if ((event->modifiers() & Qt::ControlModifier) && key == Qt::Key_R) {
        this->addNewTab();
    }
    if ((event->modifiers() & Qt::ControlModifier) && key == Qt::Key_E) {
        int index = this->mTabWidget->currentIndex();
        this->closeTab(index);
    }
    if ((event->modifiers() & Qt::ControlModifier) && key == Qt::Key_Q) {
        ui->frameTree->isHidden() ? ui->frameTree->show() : ui->frameTree->hide();
    }
}

void MainWindow::setSearchWidgetGeometry() {
    if (mCodeEditors.size() > 0){
        int searchWidth = mSearch->width();
        int searchHeight = mSearch->height();

        this->mSearch->setGeometry(ui->centralwidget->width() - searchWidth - 40, 73, searchWidth, searchHeight);
        this->mSearch->raise();
    }
}

void MainWindow::initLeftPanel() {
    ui->frameTree->setMinimumWidth(200);

    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath("");
    this->mDirTree = new CustomDirTreeView(this);
    this->mDirTree->setStyleSheet("border: 0px;");
    this->mDirTree->setModel(model);

    this->mDirTree->setAnimated(false);
    this->mDirTree->setIndentation(20);
    this->mDirTree->setSortingEnabled(true);
    this->mDirTree->setColumnWidth(0, 100);
    QScroller::grabGesture(this->mDirTree, QScroller::TouchGesture);

    this->mOpenPagesList = new QListWidget();
    for (const QString &filename : mFilenames) {
        this->mOpenPagesList->addItem(filename);
    }

    QSplitter *splitter = new QSplitter(Qt::Vertical, ui->frameTree);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(this->mDirTree);
    splitter->addWidget(mOpenPagesList);

    this->mDirTree->setMinimumHeight(100);
    this->mOpenPagesList->setMinimumHeight(100);

    QVBoxLayout *layout = new QVBoxLayout(ui->frameTree);
    layout->setContentsMargins(9, 9, 9, 9);
    layout->addWidget(splitter);

    ui->frameTree->setLayout(layout);

    for (int i = 1; i < model->columnCount(); ++i) {
        this->mDirTree->setColumnHidden(i, true);
    }

    const QString rootPath = "/";
    if (!rootPath.isEmpty()) {
        const QModelIndex rootIndex = model->index(QDir::cleanPath(rootPath));
        if (rootIndex.isValid())
            this->mDirTree->setRootIndex(rootIndex);
    }

    this->mDirTree->header()->setFixedHeight(0);

    model->sort(0, Qt::AscendingOrder);

    connect(this->mDirTree, SIGNAL(fileOpen(QString&, bool)), this, SLOT(fileOpen(QString&, bool)));
    connect(this->mOpenPagesList, &QListWidget::itemDoubleClicked, this, &MainWindow::onOpenPageItemDoubleClicked);
}

void MainWindow::initMainPanel(){
    this->mTabWidget = new QTabWidget();
    this->mTabWidget->setContentsMargins(9, 0, 0, 0);

    this->mTabWidget->setTabsClosable(true);
    this->mTabWidget->setMovable(true);

    connect(this->mTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(this->mTabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentTab);

    QVBoxLayout *layout = new QVBoxLayout(ui->frameEditor);
    layout->addWidget(mTabWidget);
    layout->setContentsMargins(9, 9, 9, 0);

    ui->frameTree->setLayout(layout);

    ui->splitterEditorAndTerminal->setSizes(QList<int>() << 300 << 100);

    // Terminal
    this->mTermWidget = new TermWidget(this);
    this->mTermWidget->setContentsMargins(9, 0, 0, 0);
    this->mTermWidget->setStyleSheet("font: 12px;");

    QVBoxLayout *layoutTerm = new QVBoxLayout(ui->frameEditor);
    layoutTerm->setContentsMargins(0, 0, 0, 0);
    layoutTerm->addWidget(this->mTermWidget);
    ui->frameTerminal->setLayout(layoutTerm);

    // addTabButton
    QPushButton *addTabButton = new QPushButton(this);
    addTabButton->setIcon(QIcon(":resources/icons/plus.png"));

    addTabButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #eee;"
        "    border: 1px solid #eee;"
        "    border-radius: 4px;"
        "    padding-right: 0px;"
        "    margin-left: 1px;"
        "    height: 22px;"
        "    width: 22px;"
        "}"

        "QPushButton:hover {"
        "   background: gray;"
        "   border: 1px solid gray;"
        "   border-radius: 4px;"
        "}"
    );

    this->mTabWidget->setCornerWidget(addTabButton, Qt::TopRightCorner);
    connect(addTabButton, &QPushButton::clicked, this, &MainWindow::addNewTab);
    connect(mTabWidget->tabBar(), &QTabBar::tabMoved, this, &MainWindow::onTabMoved);

    ui->splitterLeftAndMain->setSizes(QList<int>() << 200 << 700);
}

void MainWindow::initStatusBar(){
    this->mStatusBarFilePathLabel = new QLabel();
    this->mStatusBarFilePathLabel->setStyleSheet("color: #007cad;");
    ui->statusbar->addWidget(mStatusBarFilePathLabel, 1);

    this->mStatusBarCharactersCountLabel = new QLabel();
    this->mStatusBarCharactersCountLabel->setFixedWidth(250);
    this->mStatusBarCharactersCountLabel->setAlignment(Qt::AlignRight);
    this->mStatusBarCharactersCountLabel->setStyleSheet("color: #007cad; padding-right: 10px");

    ui->statusbar->addPermanentWidget(mStatusBarCharactersCountLabel);

    this->statusBar()->setSizeGripEnabled(false);
}

void MainWindow::updateOpenPagesList(){
    this->mOpenPagesList->clear();
    for (const QString &filename : mFilenames) {
        QFileInfo file(filename);
        this->mOpenPagesList->addItem(file.fileName());
    }
}

void MainWindow::fileCreate() {
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < this->mCodeEditors.size()) {
        this->mCodeEditors[index]->setReadOnly(false);
        QString filename = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
        if (filename.size() > 0) {
            this->mCodeEditors[index]->clear();
            this->mStatusBarFilePathLabel->setText(filename);
            this->mCodeEditors[index]->setFileExtension(filename);
            this->mCodeEditors[index]->updateSyntaxHighlighter();
            this->mCodeEditors[index]->setNeedSave(true);

            QString tabName = QFileInfo(filename).fileName();
            this->mTabWidget->setTabText(index, tabName);

            this->mStatusBarFilePathLabel->setText(filename);
            this->mFilenames[index] = filename;

            this->updateOpenPagesList();
        }
    }
}

void MainWindow::fileOpen() {
    int index = this->mTabWidget->currentIndex();

    if (index >= 0 && index < this->mCodeEditors.size()) {
        this->mCodeEditors[index]->clear();
        this->mCodeEditors[index]->setReadOnly(false);
        QString filename = QFileDialog::getOpenFileName(this, "Open file", "", "Text files (*.*)");

        if (filename == "") {
            return;
        }
        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            QString buf = stream.readAll();
            this->mCodeEditors[index]->setSourceText(buf);
            this->mCodeEditors[index]->appendPlainText(buf);
            this->mStatusBarFilePathLabel->setText(filename);
            this->mCodeEditors[index]->setFileExtension(filename);
            this->mCodeEditors[index]->updateSyntaxHighlighter();
            this->mFilenames[index] = filename;

            QString tabName = QFileInfo(filename).fileName();
            this->mTabWidget->setTabText(index, tabName.length() > 15 ? "..." + tabName.right(15) : tabName);
            this->mStatusBarFilePathLabel->setText(filename);

            updateOpenPagesList();
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

void MainWindow::fileOpen(QString &path, bool newTab) {
    if (newTab){
       this->addNewTab();
    }
    int index = mTabWidget->currentIndex();
    if (index >= 0 && index < this->mCodeEditors.size()) {

        QString filename = path;
        if (filename == "") {
            return;
        }

        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            QString buf = stream.readAll();
            this->mCodeEditors[index]->setSourceText(buf);
            this->mCodeEditors[index]->setPlainText(buf);
            this->mStatusBarFilePathLabel->setText(filename);
            this->mCodeEditors[index]->setFileExtension(filename);
            this->mCodeEditors[index]->updateSyntaxHighlighter();
            mFilenames[index] = filename;

            QString tabName = QFileInfo(filename).fileName();
            this->mTabWidget->setTabText(index, tabName.length() > 15 ? "..." + tabName.right(15) : tabName);
            this->mStatusBarFilePathLabel->setText(filename);

            this->updateOpenPagesList();
        } else {
            QMessageBox mBox;
            mBox.setWindowIcon(QIcon(":/resources/icons/icon.png"));
            mBox.setIcon(QMessageBox::Warning);
            mBox.setText("File opening error!");
            mBox.setButtonText(QMessageBox::Ok, "Ok");
            mBox.exec();
        }
        file.close();

        this->mCodeEditors[index]->setNeedSave(false);
    }
}

void MainWindow::onOpenPageItemDoubleClicked(QListWidgetItem *item){
    if (item) {
        int index = this->mOpenPagesList->row(item);
        this->mTabWidget->setCurrentIndex(index);
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


    QMenu *menuTerminal = menuBar()->addMenu("&Terminal");
    auto *clearTerminal = new QAction("&Clear", this);
    menuTerminal->addAction(clearTerminal);
    connect(clearTerminal, SIGNAL(triggered()), this, SLOT(clearTerminal()));
}

void MainWindow::fileSave() {
    int index = this->mTabWidget->currentIndex();
    if (index >= 0 && index < this->mCodeEditors.size()) {
        QFileInfo info(this->mFilenames[index]);

        if (info.exists()) {
            QFile file(this->mFilenames[index]);
            if (file.open(QIODevice::WriteOnly)) {
                QString buf = this->mCodeEditors[index]->toPlainText();
                QTextStream stream(&file);
                stream.setCodec("UTF-8");
                stream << buf;
                file.close();
                this->mNotification->setNotificationText("Saved");
                this->mNotification->show();

                this->mCodeEditors[index]->setNeedSave(false);

                this->updateOpenPagesList();
            }
        } else {
            this->fileSaveAs();
        }
    }
}

void MainWindow::fileSaveAs() {
    int index = this->mTabWidget->currentIndex();
    if (index >= 0 && index < this->mCodeEditors.size()) {
        QString filename = QFileDialog::getSaveFileName(this, "Save As", this->mFilenames[index], "Text files (*.*)");
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
            this->mNotification->setNotificationText("Saved");
            this->mNotification->show();

            this->mCodeEditors[index]->setNeedSave(false);

            this->updateOpenPagesList();
        }
    }
}

void MainWindow::fileClose() {
    int index = this->mTabWidget->currentIndex();
    if (index >= 0 && index < this->mCodeEditors.size()) {
        int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?\n" + this->mFilenames[index], QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No, QMessageBox::Save);
        if (ret == QMessageBox::Save) {
            this->fileSave();
        } else if (ret == QMessageBox::Cancel) {
            return;
        } else {
            this->mCodeEditors[index]->clear();
            this->mStatusBarFilePathLabel->setText("");
            this->mCodeEditors[index]->setFileExtension("");
            this->mCodeEditors[index]->updateSyntaxHighlighter();

            this->updateOpenPagesList();
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

    QString messageError = "";
    if (!directory.isEmpty()) {
        QFileSystemModel *model = qobject_cast<QFileSystemModel*>(this->mDirTree->model());
        if (model) {
            const QModelIndex rootIndex = model->index(QDir::cleanPath(directory));
            if (rootIndex.isValid()) {
                this->mDirTree->setRootIndex(rootIndex);

                model->sort(0, Qt::AscendingOrder);
            } else {
                messageError = "Invalid directory index.";
            }
        } else {
            messageError = "Model is not valid.";
        }
    } else {
        messageError = "No directory selected.";
    }

    if (messageError.size() > 0){
        QMessageBox mBox;
        mBox.setWindowIcon(QIcon(":/resources/icons/icon.png"));
        mBox.setIcon(QMessageBox::Warning);
        mBox.setText(messageError);
        mBox.setButtonText(QMessageBox::Ok, "Ok");
        mBox.exec();
    }

}

void MainWindow::closeEvent(QCloseEvent* /*event*/) {
    for (int i = 0; i < this->mCodeEditors.size(); ++i) {
        if (this->mCodeEditors[i]->needSave()) {
            int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?\n" + this->mFilenames[i], QMessageBox::Save | QMessageBox::No, QMessageBox::Save);
            if (ret == QMessageBox::Save) {
                this->fileSave();
            }
        }
    }

    QApplication::closeAllWindows();
}
