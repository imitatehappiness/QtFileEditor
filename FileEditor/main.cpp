#include "mainwindow.h"

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QFile>

#include "notification.h"

int main(int argc, char *argv[]){

    QApplication a(argc, argv);
    a.setStyle(new QProxyStyle(QStyleFactory::create("Fusion")));
    QString stylePath = "resources/style/appstyles.qss";
    QFile styleFile(stylePath);
    styleFile.open(QFile::ReadOnly);
    QString styleQSS = styleFile.readAll();

    if (styleQSS.length() == 0){
        Notification notification;
        notification.setNotificationText("Error appstyles.qss reading");
        notification.show();
    }

    a.setStyleSheet(styleQSS);

    MainWindow w;
    w.show();
    return a.exec();
}

