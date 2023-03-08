#pragma once

#include <QMainWindow>
#include <QLabel>

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
private:
    Ui::MainWindow *ui;
    QString mFilename;
    QLabel* mLabelFilename;
    Notification* mNotification;
};
