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
}

SearchWidget::~SearchWidget(){
    delete ui;
}

void SearchWidget::setSearchFocus(){
    ui->lE_search->setFocus();
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

void SearchWidget::on_lE_search_textChanged(const QString &arg1){
    emit search(arg1);
}

