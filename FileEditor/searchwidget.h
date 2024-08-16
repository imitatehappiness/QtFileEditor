#pragma once

#include <QWidget>

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
signals:
    /// Signal to start the search
    void search(const QString& str);
public slots:
    /// Overridden method to display the widget
    void show();
private slots:
    /// Slot for the "Close" button click
    void on_pB_close_clicked();
    void on_lE_search_textChanged(const QString &arg1);
private:
    Ui::SearchWidget *ui;
};
