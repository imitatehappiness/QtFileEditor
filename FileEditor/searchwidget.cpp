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

void SearchWidget::setSearchText(QString str){
    ui->lE_search->setText(str);
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
    emit search(arg1, forwardTypes::all);
}


void SearchWidget::on_pB_next_clicked(){
    emit search(ui->lE_search->text(), forwardTypes::next);
}


void SearchWidget::on_pB_prev_clicked(){
    emit search(ui->lE_search->text(), forwardTypes::prev);
}


void SearchWidget::on_pB_search_clicked(){
    emit search(ui->lE_search->text(), forwardTypes::all);
}

void SearchWidget::on_pB_replaceCurrent_clicked(){
    emit replace(ui->lE_search->text(), ui->lE_replace->text(), replaceTypes::replaceCurrent);
}


void SearchWidget::on_pB_replaceAll_clicked(){
    emit replace(ui->lE_search->text(), ui->lE_replace->text(), replaceTypes::replaceAll);
}

