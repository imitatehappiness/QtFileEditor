#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QDir>
#include <QVector>
#include <QString>

/*!
 * \brief The DirManager class
 */
class DirManager : public QObject {
    Q_OBJECT
public:
    DirManager(QStandardItemModel& model, QDir dir);
    void setFont(QStandardItem& item);
    void setIcon(QStandardItem& item);
private:

    QString getFilenameFromPath(QString& path);
    void findChildrenDir(QStandardItem *item, QDir dir);
    QString getTypeFile(QString &name);
public slots:
    void fillTree();
signals:
    void finished(bool);
private:
    QStandardItemModel& mModel;
    QDir mDir;
    QVector<QString> mTypeFile;
    QVector<QString> mTypeFileIgnore;

};

