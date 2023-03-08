#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

/*!
 * \class Notification
 * \class Класс отвечающий за всплывающие уведомления
 */
class Notification : public QWidget{
    Q_OBJECT
    /// Свойство полупрозрачности
    Q_PROPERTY(float mNotificationOpacity READ getNotificationOpacity WRITE setNotificationOpacity)
    /// Установка значения прозрачности
    void setNotificationOpacity(float opacity);
    /// Получение значения прозрачности
    float getNotificationOpacity() const;
public:
    /// Конструктор
    explicit Notification(QWidget *parent = 0);
protected:
    /// Фон будет отрисовываться через метод перерисовки
    void paintEvent(QPaintEvent *event);
public slots:
    /// Установка текста в уведомление
    void setNotificationText(const QString& text);
    ///  Собственный метод показа виджета
    ///  Необходимо для преварительной настройки анимации
    void show();
private slots:
    /// Слот для запуска анимации скрытия
    void hideAnimation();
    /// По окончании анимации, в данном слоте делается проверка,
    /// виден ли виджет, или его необходимо скрыть
    void hide();
private:
    /// Label с сообщением
    QLabel mLabel;
    /// Размещение для лейбла
    QGridLayout mLayout;
    /// Свойство анимации для всплывающего сообщения
    QPropertyAnimation mAnimation;
    /// Свойства полупрозрачности виджета
    float mNotificationOpacity;
    /// Таймер, по которому виджет будет скрыт
    QTimer *mTimer;
};
