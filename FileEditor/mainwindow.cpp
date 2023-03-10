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

#include "dirmanager.h"
#include "codeeditor.h"
#include "notification.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    setWindowIcon(QIcon("resources/icons/appIcon.png"));
    setWindowTitle("File Editor");
    setWindowOpacity(0.95);

    QMenu *menu = menuBar()->addMenu("&Directory");
    auto *selectDir = new QAction("&Select", this);
    auto *clearDir = new QAction("&Clear", this);

    mCodeEditor = new CodeEditor(this);
    QGridLayout* grid = new QGridLayout();
    grid->addWidget(mCodeEditor);
    ui->frame_2->setLayout(grid);
    selectDir->setIcon(QIcon("resources/icons/root-directory.png"));
    clearDir->setIcon(QIcon("resources/icons/clear.png"));

    menu->addAction(selectDir);
    menu->addAction(clearDir);

    connect(selectDir, SIGNAL(triggered()), this, SLOT(selectDirectory()));
    connect(clearDir, &QAction::triggered, this, [=](){
        mModel->clear();
    });

    connect(ui->pB_fileCreate, SIGNAL(clicked()), this, SLOT(fileCreate()));
    connect(ui->pB_fileOpen, SIGNAL(clicked()), this, SLOT(fileOpen()));
    connect(ui->pB_fileSave, SIGNAL(clicked()), this, SLOT(fileSave()));
    connect(ui->pB_fileSaveAs, SIGNAL(clicked()), this, SLOT(fileSaveAs()));
    connect(ui->pB_fileClose, SIGNAL(clicked()), this, SLOT(fileClose()));

    mLabelFilename = new QLabel();
    ui->statusbar->addWidget(mLabelFilename);

    mNotification = new Notification(this);
    mModel = new QStandardItemModel(ui->tV_directories);
    mDirManager = new DirManager(*mModel, QDir(mPath));

    ui->tV_directories->setEditTriggers(QTreeView::NoEditTriggers);
    ui->tV_directories->setSelectionBehavior(QTreeView::SelectRows);
    ui->tV_directories->setSelectionMode(QTreeView::SingleSelection);
    mModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Name"));
    ui->tV_directories->setModel(mModel);

    ui->tV_directories->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tV_directories, &QTreeView::customContextMenuRequested, this, &MainWindow::treeMenu);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::fileCreate(){
    mCodeEditor->setReadOnly(false);
    mFilename = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
    if(mFilename.size() > 0){
        mCodeEditor->clear();
        mLabelFilename->setText(mFilename);
    }
}

void MainWindow::fileOpen(){
    mCodeEditor->setReadOnly(false);
    mFilename = QFileDialog::getOpenFileName(this, "Open file", mFilename, "*.txt *.c *.cpp *.h *.qrc *.qss *.hpp *.pro *.c *.md");
    if(mFilename==""){
        return;
    }
    QFile file(mFilename);
    if(file.open(QIODevice::ReadWrite)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buf = stream.readAll();
        mCodeEditor->appendPlainText(buf);
        mLabelFilename->setText(mFilename);
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
    mFilename = path;
    if (mFilename == ""){
        return;
    }

    QFile file(mFilename);
    if(file.open(QIODevice::ReadWrite)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buf = stream.readAll();
        mCodeEditor->setPlainText(buf);
        mLabelFilename->setText(mFilename);
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

void MainWindow::fileSave(){
    QFileInfo info(mFilename);

    if(info.exists()){
        QFile file(mFilename);
        if(file.open(QIODevice::WriteOnly)){
            QString buf = mCodeEditor->toPlainText();
            QTextStream stream(&file);
            stream << buf;
            file.close();
            mNotification->setNotificationText("The file has been saved");
            mNotification->show();
        }
    }
    else{
        fileSaveAs();
    }
}

void MainWindow::fileSaveAs(){
    mFilename = QFileDialog::getSaveFileName(this,"Save As", mFilename, "Text files (*.txt);;Text files (*.cpp);;Text files (*.h);;Text files (*.hpp);;Text files (*.md);;Text files (*.qss)");
    if(mFilename==""){
        return;
    }

    QFile file(mFilename);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        QString buf = mCodeEditor->toPlainText();
        stream << buf;
        file.close();
        mNotification->setNotificationText("The file has been saved");
        mNotification->show();
        fillModel(QDir(mPath));
    }
}

void MainWindow::fileClose(){
    int ret = QMessageBox::question(this, "Close file", "Do you want to save the file?" , QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No, QMessageBox::Save);
    if(ret == QMessageBox::Save){
        mLabelFilename->setText("");
        QFile file(mFilename);
        if(!(file.open(QIODevice::WriteOnly | QIODevice::Text))){
            fileSave();
        }
        else{
            fileSaveAs();
        }
        return;
    }
    else if(ret == QMessageBox::Cancel){
        return;
    }
    else{
        mFilename = "";
        mCodeEditor->clear();
        mLabelFilename->setText("");
        return;
    }

}

void MainWindow::fillModel(QDir dir){
    if(dir.path() == "" || dir.path() == "."){
        return;
    }

    DirManager* dm = new DirManager(*mModel, mPath);
    QThread* thread = new QThread();

    dm->moveToThread(thread);
    // При запуске потока запускаем выполнение метода class::process()
    connect(thread, &QThread::started, dm, &DirManager::fillTree);
    // Также, по сигналу finished отправляем команду на завершение потока
    connect(dm, &DirManager::finished, thread, &QThread::quit);
    // А потом удаляем экземпляр обработчика
    connect(dm, &DirManager::finished, dm, &QObject::deleteLater);
    // И наконец, когда закончит работу поток, удаляем и его
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void MainWindow::selectDirectory(){
    mPath = QFileDialog::getExistingDirectory(this, "Select a directory");
    fillModel(QDir(mPath));
}

void MainWindow::treeMenu(const QPoint &pos){
    QMenu menu;
    QModelIndex curIndex = ui->tV_directories->indexAt(pos);
    QModelIndex index = curIndex.sibling(curIndex.row(), 0);
    if (index.isValid()){

        if(mModel->itemFromIndex(index)->data(Qt::UserRole + 1).toString() == "file"){
            menu.addAction (QIcon("resources/icons/display.png"), QStringLiteral("Display"), this, SLOT (treeMenuDisplay(bool)));
            menu.addAction (QIcon("resources/icons/fileIcons/open_file.png"), QStringLiteral("Open"), this, SLOT (treeMenuOpen(bool)));
            menu.addAction (QIcon("resources/icons/fileIcons/delete_file.png"), QStringLiteral("Delete"), this, SLOT (treeMenuDelete(bool)));
        }
        if(mModel->itemFromIndex(index)->data(Qt::UserRole + 1).toString() == "folder"){
            menu.addAction (QIcon("resources/icons/folder_black.png"), QStringLiteral("Create folder"), this, SLOT (treeMenuCreateFolder(bool)));
            menu.addAction (QIcon("resources/icons/file_black.png"), QStringLiteral("Create file"), this, SLOT (treeMenuCreateFile(bool)));
            menu.addAction (QIcon("resources/icons/fileIcons/delete_file.png"), QStringLiteral("Delete"), this, SLOT (treeMenuDelete(bool)));
        }
    }
    menu.exec(QCursor::pos());
}

void MainWindow::treeMenuOpen(bool /*checked*/){
    mCodeEditor->setReadOnly(false);
    QModelIndex curIndex = ui->tV_directories->currentIndex();
    QModelIndex index = curIndex.sibling (curIndex.row (), 0);
    if(index.isValid()){
        QStandardItem* item = mModel->itemFromIndex(index);
        QString path = item->data(Qt::UserRole + 2).toString();
        fileOpen(path);
    }
}

void MainWindow::treeMenuDelete(bool){
    int ret = QMessageBox::question(this, "Delete", "Are you sure?" , QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret == QMessageBox::Yes){
        QModelIndex curIndex = ui->tV_directories->currentIndex();

        QModelIndex index = curIndex.sibling (curIndex.row (), 0);
        if(index.isValid()){
            QStandardItem* item = mModel->itemFromIndex(index);
            QString path = item->data(Qt::UserRole + 2).toString();
            bool res = QFileInfo(path).isDir() ? mDirManager->removeDir(path) : mDirManager->removeFile(path);
            res ?  mNotification->setNotificationText("Deleted: " + path) :
                   mNotification->setNotificationText("Deletion Error: " + path) ;

            mNotification->show();
            mModel->removeRow(index.row(), index.parent());
            mCodeEditor->clear();
        }
    }
}

void MainWindow::treeMenuCreateFolder(bool){
    QString name = QInputDialog::getText(this, tr("Create folder"), tr("Enter the folder name:"), QLineEdit::Normal);
    QModelIndex curIndex = ui->tV_directories->currentIndex();
    QModelIndex index = curIndex.sibling (curIndex.row (), 0);
    if(index.isValid() && name.size() > 0){
        QStandardItem* item = mModel->itemFromIndex(index);
        QString path = item->data(Qt::UserRole + 2).toString();
        QDir(path).mkdir(name);
        QStandardItem* newFolder = mDirManager->createFile(path, name, "folder");
        item->appendRow(newFolder);
    }
}

void MainWindow::treeMenuCreateFile(bool){
    QString name = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
    QModelIndex curIndex = ui->tV_directories->currentIndex();
    QModelIndex index = curIndex.sibling (curIndex.row (), 0);
    if(index.isValid() && name.size() > 0){
        QStandardItem* item = mModel->itemFromIndex(index);
        QString path = item->data(Qt::UserRole + 2).toString();
        QFile file(path + "/" + name);
        if(file.open(QIODevice::WriteOnly)){
            QStandardItem* newFile = mDirManager->createFile(path, name, "file");
            item->appendRow(newFile);
        }
    }
}

void MainWindow::treeMenuDisplay(bool){
    mCodeEditor->setReadOnly(true);
    QModelIndex curIndex = ui->tV_directories->currentIndex();
    QModelIndex index = curIndex.sibling (curIndex.row (), 0);
    if(index.isValid()){
        QStandardItem* item = mModel->itemFromIndex(index);
        QString path = item->data(Qt::UserRole + 2).toString();
        fileOpen(path);
    }
}
