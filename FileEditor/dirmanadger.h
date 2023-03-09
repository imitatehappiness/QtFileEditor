#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QDir>
#include <QVector>
#include <QString>

/*!
 * \brief The DirManadger class
 */
class DirManadger : public QObject {
    Q_OBJECT
public:
    DirManadger(QStandardItemModel& model, QDir dir);
private:
    void setFont(QStandardItem& item);
    void setIcon(QStandardItem& item);
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

