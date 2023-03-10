#include "dirmanager.h"

DirManager::DirManager(QStandardItemModel& model, QDir dir)
    : mModel(model),
      mDir(dir){
    mTypeFile.append({"cpp", "h", "hpp", "md", "c", "pro", "qrc", "ui", "txt", "qss"});
    mTypeFileIgnore.append({"moc", "qrc", "ui"});
}


void DirManager::fillTree(){
    mModel.clear();
    mModel.setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Name"));

    mDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs );

    QString path = mDir.path();
    QString filename = getFilenameFromPath(path);
    QFileInfo file(path);

    if(file.exists()){
        QStandardItem* item = new QStandardItem(filename);
        file.isFile() ? item->setData("file", Qt::UserRole + 1) :
                        item->setData("folder", Qt::UserRole + 1);

        item->setData(path, Qt::UserRole + 2);

        setFont(*item);
        setIcon(*item);
        mModel.appendRow(item);

        findChildrenDir(item, mDir);
    }

    emit finished(true);
}

void DirManager::findChildrenDir(QStandardItem* item, QDir dir){
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
        if(file.isFile()){
            QString type = getTypeFile(fileList[i]);
            if(!mTypeFile.contains(type)){
                continue;
            }
            if(mTypeFileIgnore.contains(fileList[i].split("_").at(0))){
                continue;
            }
        }else{
            if(fileList[i][0] == "."){
                continue;
            }
        }
        QStandardItem* childItem = new QStandardItem(fileList[i]);
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

    if(!item->hasChildren()){
        mModel.removeRow(item->index().row(), item->index().parent());
    }
}

void DirManager::setFont(QStandardItem &item){
    if(item.data(Qt::UserRole + 1).toString() == "folder"){
        QFont font;
        font.setBold(true);
        item.setFont(font);
    }
}

void DirManager::setIcon(QStandardItem &item){
    if(item.data(Qt::UserRole + 1).toString() == "folder"){
        item.setIcon(QIcon("resources/icons/folder.png"));
    }else{
        item.setIcon(QIcon("resources/icons/file.png"));
    }
}

QString DirManager::getFilenameFromPath(QString &path){
    QStringList parts = path.split("/");
    return parts.at(parts.size() - 1);
}

QString DirManager::getTypeFile(QString &name){
    QStringList parts = name.split(".");
    return parts.at(parts.size() - 1);
}
