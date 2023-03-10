#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QDir>

QT_BEGIN_NAMESPACE
class Notification;
class CodeEditor;
class DirManager;
namespace Ui {
    class MainWindow;
}
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
    /// Слот нажатия контекстного меню элемента модели "Открыть Файл"
    void treeMenuOpen(bool);
    /// Слот нажатия контекстного меню элемента модели "Удалить Файл/Папку"
    void treeMenuDelete(bool);
    /// Слот нажатия контекстного меню элемента модели "Создать папку"
    void treeMenuCreateFolder(bool);
    /// Слот нажатия контекстного меню элемента модели "Создать файл"
    void treeMenuCreateFile(bool);
    /// Слот нажатия контекстного меню элемента модели "Отобразить файл"
    void treeMenuDisplay(bool);
private:
    /// Открытие конкретного файла через путь к нему
    void fileOpen(QString& path);
    /// Заполнение модели (запуск потока)
    void fillModel(QDir dir);
    /// Отображение контекстного меню элемента модели
    void treeMenu(const QPoint &pos);
private:
    Ui::MainWindow *ui;
    /// Имя текущего файла
    QString mFilename;
    /// Лейбл статус бара
    QLabel* mLabelFilename = nullptr;
    /// Объект отвечающий за всплывающие уведомления
    Notification* mNotification = nullptr;
    /// Модель директорий
    QStandardItemModel* mModel = nullptr;
    /// Путь до текущего файла
    QString mPath;
    /// Редактор файлов
    CodeEditor* mCodeEditor = nullptr;
    /// Менеджер директорий
    DirManager* mDirManager = nullptr;
};
