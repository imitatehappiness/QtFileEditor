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
    /// Constructor
    MainWindow(QWidget *parent = nullptr);
    /// Destructor
    ~MainWindow();

private slots:
    /// Slot for creating a file
    void fileCreate();
    /// Slot for opening a file
    void fileOpen();
    /// Slot for saving a file
    void fileSave();
    /// Slot for saving a file with a specific extension
    void fileSaveAs();
    /// Slot for closing a file
    void fileClose();
protected:
    /// Overridden method for closing the application
    void closeEvent(QCloseEvent* event) override;
    /// Overridden method for the recommended size of the widget
    void resizeEvent(QResizeEvent* event) override;
    /// Overridden method for key press events
    void keyPressEvent(QKeyEvent* event) override;
private:
    /// Open a specific file using its path
    void fileOpen(QString& path);
    /// Populate the model (start the thread)
    void fillModel(QDir dir);
    /// Display the context menu for the model element
    void treeMenu(const QPoint &pos);
    /// Set the text search widget to the correct position
    void setPositionSearchWidget();

    void initMenuBar();
    void setSearchWidgetGeometry();
private:
    Ui::MainWindow *ui;
    /// Name of the current file
    QString mFilename;
    /// Status bar label
    QLabel* mLabelFilename = nullptr;
    /// Object responsible for notifications
    Notification* mNotification = nullptr;
    /// Directory model
    QStandardItemModel* mModel = nullptr;
    /// Path to the current file
    QString mPath;
    /// File editor
    CodeEditor* mCodeEditor = nullptr;
    /// Directory manager
    DirManager* mDirManager = nullptr;
    /// Widget for file search
    SearchWidget* mSearch = nullptr;
};
