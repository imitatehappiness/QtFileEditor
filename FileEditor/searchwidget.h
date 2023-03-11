#pragma once

#include <QWidget>

namespace Ui {
class SearchWidget;
}

/*!
 * \class SearchWidget
 * \brief Виджет для ввода строки для поиска
 */
class SearchWidget : public QWidget{
    Q_OBJECT
public:
    /// Констурктор
    explicit SearchWidget(QWidget *parent = nullptr);
    /// Деструтор
    ~SearchWidget();
signals:
    /// Сигнал начала поиска
    void search(const QString& str);
public slots:
    /// Переопределенный метотд отображения виджета
    void show();
private slots:
    /// Слот нажатия кпопки "Закрыть"
    void on_pB_close_clicked();
private:
    Ui::SearchWidget *ui;
};


