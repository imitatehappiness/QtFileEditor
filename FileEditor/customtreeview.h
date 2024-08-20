#pragma once

#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QScreen>
#include <QScroller>
#include <QTreeView>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QContextMenuEvent>
#include <QMessageBox>

class CustomDirTreeView : public QTreeView
{
    Q_OBJECT

public:
    CustomDirTreeView(QWidget *parent = nullptr) : QTreeView(parent) {}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        QModelIndex index = indexAt(event->pos());
        QMenu contextMenu;

        QAction *openAction = contextMenu.addAction("Open");

        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            if (fsModel->isDir(index)) {
                QAction *createFileAction = contextMenu.addAction("Create File");
                QAction *createDirectoryAction = contextMenu.addAction("Create Directory");

                connect(createFileAction, &QAction::triggered, this, [this, index]() { createFile(index); });
                connect(createDirectoryAction, &QAction::triggered, this, [this, index]() { createDirectory(index); });
            }
        }

        QAction *renameAction = contextMenu.addAction("Rename");
        QAction *copyAction = contextMenu.addAction("Copy");
        QAction *pasteAction = contextMenu.addAction("Paste");
        QAction *deleteAction = contextMenu.addAction("Delete");

        connect(openAction, &QAction::triggered, this, [this, index]() { openItem(index); });
        connect(deleteAction, &QAction::triggered, this, [this, index]() { deleteItem(index); });
        connect(renameAction, &QAction::triggered, this, [this, index]() { renameItem(index); });
        connect(copyAction, &QAction::triggered, this, [this, index]() { copyItem(index); });
        connect(pasteAction, &QAction::triggered, this, [this, index]() { pasteItem(index); });

        // Enable or disable the Paste action based on whether mСopyPath is set
        pasteAction->setEnabled(!mCopyPath.isEmpty());

        contextMenu.exec(event->globalPos());
    }
signals:
    void fileOpen(QString& filePath, bool newTab);
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * event) override {
        QModelIndex index = indexAt(event->pos());
        if (index.isValid()) {
            openItem(index);  // Call openItem when an item is double-clicked
        }
    }
private:
    QString mCopyPath;
private:
    bool copyDirectoryRecursively(const QString &sourcePath, const QString &destinationPath){
        QDir sourceDir(sourcePath);
        QDir destDir(destinationPath);

        if (!destDir.exists()) {
            if (!destDir.mkpath(destinationPath)) {
                return false;
            }
        }

        QFileInfoList entries = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QFileInfo &entry, entries) {
            QString srcFilePath = entry.absoluteFilePath();
            QString destFilePath = destDir.filePath(entry.fileName());

            if (entry.isDir()) {
                if (!copyDirectoryRecursively(srcFilePath, destFilePath)) {
                    return false;
                }
            } else {
                if (!QFile::copy(srcFilePath, destFilePath)) {
                    return false;
                }
            }
        }

        return true;
    }

    void createFile(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            QString fileName = QInputDialog::getText(this, "Create File", "Enter file name:");
            if (!fileName.isEmpty()) {
                QString filePath = fsModel->filePath(index) + "/" + fileName;
                QFile file(filePath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.close();
                } else {
                    QMessageBox::warning(this, "Error", "Could not create file.");
                }
            }
        }
    }

    void createDirectory(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            QString dirName = QInputDialog::getText(this, "Create Directory", "Enter directory name:");
            if (!dirName.isEmpty()) {
                QDir dir(fsModel->filePath(index));
                if (!dir.mkdir(dirName)) {
                    QMessageBox::warning(this, "Error", "Could not create directory.");
                }
            }
        }
    }

    void deleteItem(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            QString itemPath = fsModel->filePath(index);
            QFileInfo fileInfo(itemPath);

            QString warningText = fileInfo.isDir() ?
                "Are you sure you want to delete this directory?" :
                "Are you sure you want to delete this file?";

            if (QMessageBox::question(this, "Delete", warningText, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                if (fileInfo.isDir()) {
                    QDir dir(itemPath);
                    if (!dir.removeRecursively()) {
                        QMessageBox::warning(this, "Error", "Could not delete directory.");
                    }
                } else {
                    QFile file(itemPath);
                    if (!file.remove()) {
                        QMessageBox::warning(this, "Error", "Could not delete file.");
                    }
                }
            }
        }
    }

    void renameItem(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            QString oldName = fsModel->fileName(index);
            QString newName = QInputDialog::getText(this, "Rename", "Enter new name:", QLineEdit::Normal, oldName);

            if (!newName.isEmpty() && newName != oldName) {
                QString oldPath = fsModel->filePath(index);
                QString newPath = fsModel->rootDirectory().absoluteFilePath(newName);

                if (!QFile::rename(oldPath, newPath)) {
                    QMessageBox::warning(this, "Error", "Could not rename item.");
                }
            }
        }
    }

    void openItem(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            if (fsModel->isDir(index)) {
                if (!isExpanded(index)) {
                    expand(index);
                }

            } else {
                QString filePath = fsModel->filePath(index);

                emit fileOpen(filePath, true);
            }
        }
    }

    void copyItem(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            QString itemPath = fsModel->filePath(index);
            mCopyPath = itemPath;
        }
    }

    void pasteItem(const QModelIndex &index){
        if (QFileSystemModel *fsModel = qobject_cast<QFileSystemModel*>(model())) {
            if (!mCopyPath.isEmpty()) {
                QString destinationPath = fsModel->filePath(index);
                QFileInfo fileInfo(mCopyPath);
                QString destinationPathFull = destinationPath + "/" + fileInfo.fileName();

                QFileInfo destinationFileInfo(destinationPathFull);

                if (destinationFileInfo.exists()) {
                    QString message = fileInfo.isDir() ?
                        "A directory with the same name already exists. Do you want to replace it?" :
                        "A file with the same name already exists. Do you want to replace it?";

                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(this, "Replace", message,
                                                  QMessageBox::Yes | QMessageBox::No);

                    if (reply == QMessageBox::No) {
                        return;
                    }

                    if (fileInfo.isDir()) {
                        QDir dir(destinationPathFull);
                        if (dir.exists()) {
                            if (!dir.removeRecursively()) {
                                QMessageBox::warning(this, "Error", "Could not delete existing directory.");
                                return;
                            }
                        }
                    } else {
                        if (!QFile::remove(destinationPathFull)) {
                            QMessageBox::warning(this, "Error", "Could not delete existing file.");
                            return;
                        }
                    }
                }

                bool success = false;
                if (fileInfo.isDir()) {
                    success = copyDirectoryRecursively(mCopyPath, destinationPathFull);
                } else {
                    success = QFile::copy(mCopyPath, destinationPathFull);
                }

                if (!success) {
                    QMessageBox::warning(this, "Error", fileInfo.isDir() ?
                        "Could not copy directory." :
                        "Could not copy file.");
                }

                mCopyPath.clear();  // Clear the copied path after pasting
            }
        }
    }
};
