#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QDir>

#include "notification.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
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
    void selectDirectory();
    void treeMenuOpen(bool);
    void treeMenuDelete(bool);
private:
    void fileOpen(QString& path);
    void fillModel(QDir dir);
    void treeMenu(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    QString mFilename;
    QLabel* mLabelFilename;
    Notification* mNotification;
    QStandardItemModel* mModel;
    QString mPath;
};
