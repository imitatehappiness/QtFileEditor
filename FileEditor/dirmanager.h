#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QDir>
#include <QVector>
#include <QString>

/*!
 * \class DirManager
 * \class Класс отвечающий за заполнение модели директориями
 */
class DirManager : public QObject {
    Q_OBJECT
public:
    /// Конструктор с параметрами
    DirManager(QStandardItemModel& model, QDir dir);
    /// Установка шрифта для элемента модели
    void setFont(QStandardItem& item);
    /// Установка иконки для элемента модели
    void setIcon(QStandardItem& item);
private:
    /// Получение имени из пути
    QString getFilenameFromPath(QString& path);
    /// Поиск потомков элемента
    void findChildrenDir(QStandardItem *item, QDir dir);
    /// Получение типа файла из имени
    QString getTypeFile(QString &name);
public slots:
    /// Слот заполнения модели
    void fillTree();
signals:
    /// Сигнал остановки потока
    void finished(bool);
private:
    /// Модель TreeView
    QStandardItemModel& mModel;
    /// Директория
    QDir mDir;
    /// Список допустимых расширений файлов для модели
    QVector<QString> mTypeFile;
    /// Список недопустимых файлов среди допустимых
    QVector<QString> mTypeFileIgnore;
};

