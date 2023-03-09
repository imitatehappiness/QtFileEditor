#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QInputDialog>
#include <QDebug>
#include <QModelIndex>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    setWindowIcon(QIcon("resources/icons/appIcon.png"));
    setWindowTitle("File Editor");

    QMenu *menu = menuBar()->addMenu("&Directory");
    auto *selectDir = new QAction("&Select", this);
    selectDir->setIcon(QIcon("resources/icons/root-directory.png"));
    menu->addAction(selectDir);
    connect(selectDir, SIGNAL(triggered()), this, SLOT(selectDirectory()));

    connect(ui->pB_fileCreate, SIGNAL(clicked()), this, SLOT(fileCreate()));
    connect(ui->pB_fileOpen, SIGNAL(clicked()), this, SLOT(fileOpen()));
    connect(ui->pB_fileSave, SIGNAL(clicked()), this, SLOT(fileSave()));
    connect(ui->pB_fileSaveAs, SIGNAL(clicked()), this, SLOT(fileSaveAs()));
    connect(ui->pB_fileClose, SIGNAL(clicked()), this, SLOT(fileClose()));

    mLabelFilename = new QLabel();
    ui->statusbar->addWidget(mLabelFilename);

    mNotification = new Notification(this);

    setWindowOpacity(0.95);

    mModel = new QStandardItemModel(ui->tV_directories);

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
    mFilename = QInputDialog::getText(this, tr("Create file"), tr("Enter the file name:"), QLineEdit::Normal);
    if(mFilename.size() > 0){
        ui->tE_textEditor->clear();
        mLabelFilename->setText(mFilename);
    }
}

void MainWindow::fileOpen(){
    mFilename = QFileDialog::getOpenFileName(this, "Open file", mFilename, "*.txt *.c *.cpp *.h");
    if(mFilename==""){
        return;
    }
    QFile file(mFilename);
    if(file.open(QIODevice::ReadWrite)){
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString buf = stream.readAll();
        ui->tE_textEditor->setText(buf);
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
            QString buf = ui->tE_textEditor->toPlainText();
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
    mFilename = QFileDialog::getSaveFileName(this,"Save As", mFilename);
    if(mFilename==""){
        return;
    }

    QFile file(mFilename);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        QString buf = ui->tE_textEditor->toPlainText();
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
        ui->tE_textEditor->clear();
        mLabelFilename->setText("");
        return;
    }

}

void MainWindow::fillModel(QDir dir){
    mModel->clear();
    mModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Name"));

    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs );

    QString path = dir.path();
    QString filename = getFilenameFromPath(path);
    QFileInfo file(path);

    if(file.exists()){
        QStandardItem* item = new QStandardItem(filename);
        file.isFile() ? item->setData("file", Qt::UserRole + 1) :
                        item->setData("folder", Qt::UserRole + 1);

        item->setData(path, Qt::UserRole + 2);
        setFont(*item);
        setIcon(*item);
        mModel->appendRow(item);
        findChildrenDir(item, dir);
    }
}

void MainWindow::findChildrenDir(QStandardItem* item, QDir dir){
    QString path = dir.path();
    QFileInfo file(path);
    if(file.isFile()){
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs );
    QStringList fileList = dir.entryList();

    for (int i=0; i < fileList.count(); i++){
        QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(fileList.at(i));
        file.setFile(newPath);

        QStandardItem * childItem = new QStandardItem(fileList[i]);
        file.isFile() ? childItem->setData("file", Qt::UserRole + 1) :
                        childItem->setData("folder", Qt::UserRole + 1);

        childItem->setData(newPath, Qt::UserRole + 2);
        setFont(*childItem);
        setIcon(*childItem);

        item->appendRow(childItem);

        if(file.isDir()){
            findChildrenDir(childItem, QDir(newPath));
        }
    }
}

void MainWindow::selectDirectory(){
    mPath = QFileDialog::getExistingDirectory(this, "Select a directory");
    mNotification->setNotificationText("Select: " + mPath);
    mNotification->show();
    fillModel(QDir(mPath));
}

void MainWindow::setFont(QStandardItem &item){
    if(item.data(Qt::UserRole + 1).toString() == "folder"){
        QFont font;
        //font.setPointSize(10);
        font.setBold(true);
        item.setFont(font);
    }
}

void MainWindow::setIcon(QStandardItem &item){
    if(item.data(Qt::UserRole + 1).toString() == "folder"){
        item.setIcon(QIcon("resources/icons/folder.png"));
    }else{
        item.setIcon(QIcon("resources/icons/file.png"));
    }
}

QString MainWindow::getFilenameFromPath(QString &path){
    QStringList parts = path.split("/");
    return parts.at(parts.size() - 1);
}

void MainWindow::treeMenu(const QPoint &pos){
    QMenu menu;
    QModelIndex curIndex = ui->tV_directories->indexAt(pos);
    QModelIndex index = curIndex.sibling(curIndex.row(), 0);
    if (index.isValid()){
        if(mModel->itemFromIndex(index)->data(Qt::UserRole + 1).toString() == "folder"){
            return;
        }
        menu.addAction (QStringLiteral ("Open"), this, SLOT (treeMenuOpen(bool)));
        menu.addAction (QStringLiteral ("Delete"), this, SLOT (treeMenuDelete(bool)));

    }
    menu.exec(QCursor::pos());
}

void MainWindow::treeMenuOpen(bool /*checked*/){
    QModelIndex curIndex = ui->tV_directories->currentIndex();
    QModelIndex index = curIndex.sibling (curIndex.row (), 0);
    if(index.isValid()){
        QStandardItem* item = mModel->itemFromIndex(index);
        QString path = item->data(Qt::UserRole + 2).toString();
        fileOpen(path);
    }
}

void MainWindow::treeMenuDelete(bool){
    int ret = QMessageBox::question(this, "Delete file", "Are you sure?" , QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret == QMessageBox::Yes){
        QModelIndex curIndex = ui->tV_directories->currentIndex();

        QModelIndex index = curIndex.sibling (curIndex.row (), 0);
        if(index.isValid()){
            QStandardItem* item = mModel->itemFromIndex(index);
            QString path = item->data(Qt::UserRole + 2).toString();
            QDir dir;
            dir.remove(path);
            mNotification->setNotificationText("Deleted: " + path);
            mNotification->show();
            mModel->removeRow(index.row(), index.parent());
            ui->tE_textEditor->clear();
        }
    }
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
        ui->tE_textEditor->setText(buf);
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
