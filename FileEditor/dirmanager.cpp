#include "dirmanager.h"

DirManager::DirManager(QStandardItemModel& model, QDir dir)
    : mModel(model),
      mDir(dir){
    mTypeFile.append({"cpp", "h", "hpp", "md", "c", "pro", "qrc", "ui", "txt", "qss"});
    mTypeFileIgnore.append({"moc", "qrc", "ui"});
}

QStandardItem *DirManager::createFile(const QString &path, const QString &name, const QString &type){
    QStandardItem* item = new QStandardItem(name);
    item->setData(type, Qt::UserRole + 1);
    item->setData(path + "/" + name, Qt::UserRole + 2);
    setFont(*item);
    setIcon(*item);
    return item;
}

void DirManager::fillTree(){
    mModel.clear();
    mModel.setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Name"));

    mDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs );

    QString path = mDir.path();
    QString filename = getFilenameFromPath(path);
    QFileInfo file(path);

    if(file.exists()){
        QString type;
        type = file.isFile() ? "file" : "folder";
        QStandardItem* item = createFile(path, filename, type);
        mModel.appendRow(item);

        findChildrenDir(item, mDir);
    }

    emit finished(true);
}

void DirManager::setDir(const QDir &newDir){
    mDir = newDir;
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

        QString type;
        type = file.isFile() ?  "file" : "folder";
        QStandardItem* childItem = createFile(path, fileList[i], type);
        mModel.appendRow(item);

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

bool DirManager::removeDir(const QString& path){
    QDir dir(path);
    return dir.removeRecursively();
}

bool DirManager::removeFile(const QString &path){
    return QDir().remove(path);
}

QString DirManager::getFilenameFromPath(QString &path){
    QStringList parts = path.split("/");
    return parts.at(parts.size() - 1);
}

QString DirManager::getTypeFile(QString &name){
    QStringList parts = name.split(".");
    return parts.at(parts.size() - 1);
}
