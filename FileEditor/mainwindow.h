#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QDir>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
class Notification;
class CodeEditor;
class SearchWidget;
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \class MainWindow
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /// Constructor
    MainWindow(QWidget *parent = nullptr);
    /// Destructor
    ~MainWindow();

private slots:
    void fileCreate();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileClose();

    void closeTab(int index);
    void addNewTab();
    void updateCurrentTab(int index);

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void fileOpen(QString& path);
    void fillModel(QDir dir);
    void treeMenu(const QPoint &pos);
    void setPositionSearchWidget();
    void initMenuBar();
    void setSearchWidgetGeometry();
private:
    Ui::MainWindow *ui;
    QTabWidget *mTabWidget;

    QVector<QString> mFilenames;
    QVector<CodeEditor*> mCodeEditors;

    Notification* mNotification;
    QLabel* mLabelFilename;
    SearchWidget* mSearch;
};
