#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QDir>
#include <QTabWidget>
#include <QListWidget>

#include "customtreeview.h"

#include "termwidget.h"

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void fileCreate();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileClose();
    void openDir();
    void closeTab(int index);
    void addNewTab();
    void updateCurrentTab(int index);
    void onTabMoved(int from, int to);
    void clearTerminal();
    void updateCharactersCount(int count);
    void showShortcuts();
public slots:
    void fileOpen(QString& path, bool newTab = false);
    void onOpenPageItemDoubleClicked(QListWidgetItem* item);
protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    void fillModel(QDir dir);
    void treeMenu(const QPoint &pos);
    void initMenuBar();
    void setSearchWidgetGeometry();
    void initLeftPanel();
    void initMainPanel();
    void initStatusBar();
    void updateOpenPagesList();
private:
    Ui::MainWindow *ui;

    QTabWidget *mTabWidget;

    QVector<QString> mFilenames;
    QVector<CodeEditor*> mCodeEditors;

    Notification* mNotification;
    QLabel* mStatusBarFilePathLabel;
    QLabel* mStatusBarCharactersCountLabel;
    SearchWidget* mSearch;
    CustomDirTreeView* mDirTree;
    QListWidget *mOpenPagesList;
    TermWidget* mTermWidget;
};
