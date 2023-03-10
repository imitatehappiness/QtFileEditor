#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QDir>

#include "notification.h"
#include "codeeditor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*!
 * \class MainWindow
 */
class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    /// Конструктор
    MainWindow(QWidget *parent = nullptr);
    /// Деструктор
    ~MainWindow();
private slots:
    /// Слот создания файла
    void fileCreate();
    /// Слот открытия файла
    void fileOpen();
    /// Слот сохранения файла
    void fileSave();
    /// Слот сохранения файла с конкретным расширением
    void fileSaveAs();
    /// Слот закрытия файла
    void fileClose();
    /// Слот выбора директории для заполнения модели
    void selectDirectory();
    /// Слот нажатия контекстного меню элемента модели "Открыть"
    void treeMenuOpen(bool);
    /// Слот нажатия контекстного меню элемента модели "Удалить"
    void treeMenuDelete(bool);
private:
    /// Открытие конкретного файла через путь к нему
    void fileOpen(QString& path);
    /// Заполнение модели (запуск потока)
    void fillModel(QDir dir);
    /// Отображение контекстного меню элемента модели
    void treeMenu(const QPoint &pos);
    /// Удаление конкретной папки
    bool removeDir(const QString &path);

private:
    Ui::MainWindow *ui;
    /// Имя текущего файла
    QString mFilename;
    /// Лейбл статус бара
    QLabel* mLabelFilename;
    /// Объект отвечающий за всплывающие уведомления
    Notification* mNotification;
    /// Модель директорий
    QStandardItemModel* mModel;
    /// Путь до текущего файла
    QString mPath;
    /// Редактор файлов
    CodeEditor* mCodeEditor;
};
