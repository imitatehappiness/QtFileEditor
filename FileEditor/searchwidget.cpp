#include "searchwidget.h"
#include "ui_searchwidget.h"

#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>
SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget){

    setWindowIcon(QIcon("resources/icons/search.png"));
    setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    connect(ui->pB_search, &QPushButton::clicked, this, [=](){
        QString str = ui->lE_search->text();
        if(!str.isEmpty()){
            emit search(ui->lE_search->text());
        }
    });
}

SearchWidget::~SearchWidget(){
    delete ui;
}

void SearchWidget::on_pB_close_clicked(){
    close();
}

void SearchWidget::show(){
    if(!isHidden()) {
        showNormal();
        activateWindow();
    } else {
        QWidget::show();
        ui->lE_search->clear();
    }
}
