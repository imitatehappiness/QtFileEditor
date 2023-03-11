#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QDir>

QT_BEGIN_NAMESPACE
class Notification;
class CodeEditor;
class DirManager;
class SearchWidget;
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
    /// Слот поиска по файлу
    void search(const QString& str);
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
protected:
    /// Переопределенный метод закрытия приложения
    void closeEvent(QCloseEvent* event) override;
    /// Переопределение метода рекомендуемого размера виджета
    void resizeEvent(QResizeEvent* event) override;
    ///
    void keyPressEvent(QKeyEvent* event) override;
private:
    /// Открытие конкретного файла через путь к нему
    void fileOpen(QString& path);
    /// Заполнение модели (запуск потока)
    void fillModel(QDir dir);
    /// Отображение контекстного меню элемента модели
    void treeMenu(const QPoint &pos);
    /// Установка поиска по тексту в нужное положение
    void setPositionSearchWidget();
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
    /// Виджет потска по файлу
    SearchWidget* mSearch = nullptr;
};
