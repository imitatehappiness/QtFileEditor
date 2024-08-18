#include "mainwindow.h"
#include "windowframe.h"

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QFile>
#include <QMessageBox>
#include <QMetaType>
#include <QWidget>
#include <Qt>

int main(int argc, char *argv[]){

    QApplication a(argc, argv);
    a.setStyle(new QProxyStyle(QStyleFactory::create("Fusion")));
    QString stylePath = "resources/style/appstyles.qss";
    QFile styleFile(stylePath);
    styleFile.open(QFile::ReadOnly);
    QString styleQSS = styleFile.readAll();

    if (styleQSS.size() == 0){
        QMessageBox mBox;
        mBox.setIcon(QMessageBox::Warning);
        mBox.setText("Error appstyles.qss reading! \nThe program may not work correctly!");
        mBox.setButtonText(QMessageBox::Ok, "Ok");
        mBox.exec();
    }

    a.setStyleSheet(styleQSS);

    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");

    WindowFrame w(nullptr, new MainWindow());
    w.show();
    return a.exec();
}

