#pragma once

#include <QWidget>

#include <types.h>

namespace Ui {
class SearchWidget;
}

/*!
 * \class SearchWidget
 * \brief Widget for entering a search string
 */
class SearchWidget : public QWidget{
    Q_OBJECT
public:
    /// Constructor
    explicit SearchWidget(QWidget *parent = nullptr);
    /// Destructor
    ~SearchWidget();
    void setSearchFocus();
    void setSearchText(QString str);
signals:
    /// Signal to start the search
    void search(const QString& str, forwardTypes forward);
    /// Signal to start the replace
    void replace(const QString& from, const QString& to, replaceTypes replace);

public slots:
    /// Overridden method to display the widget
    void show();
private slots:
    /// Slot for the "Close" button click
    void on_pB_close_clicked();
    void on_lE_search_textChanged(const QString &arg1);
    void on_pB_next_clicked();

    void on_pB_prev_clicked();

    void on_pB_search_clicked();

    void on_pB_replaceCurrent_clicked();

    void on_pB_replaceAll_clicked();

private:
    Ui::SearchWidget *ui;
};
