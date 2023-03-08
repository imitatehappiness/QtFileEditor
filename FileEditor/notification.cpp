#include "notification.h"

#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

Notification::Notification(QWidget *parent) : QWidget(parent){
    setWindowFlags(Qt::FramelessWindowHint |        /// Отключаем оформление окна
                   Qt::Tool |                       /// Отменяем показ в качестве отдельного окна
                   Qt::WindowStaysOnTopHint);       /// Устанавливаем поверх всех окон
    setAttribute(Qt::WA_TranslucentBackground);     /// Указываем, что фон будет прозрачным
    setAttribute(Qt::WA_ShowWithoutActivating);     /// При показе, виджет не получается фокуса автоматически

    /// Устанавливаем целевой объект анимации
    mAnimation.setTargetObject(this);
    /// Устанавилваем анимируемое свойство
    mAnimation.setPropertyName("mNotificationOpacity");
    /// Подключаем сигнал окончания анимации к слоты скрытия
    connect(&mAnimation, &QAbstractAnimation::finished, this, &Notification::hide);

    /// Настройка текста уведомления
    mLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    /// Настройка стилей
    mLabel.setStyleSheet("QLabel { color : white; "
                        "margin-top: 6px;"
                        "margin-bottom: 6px;"
                        "margin-left: 10px;"
                        "margin-right: 10px; }");

    /// Производим установку текста в размещение
    mLayout.addWidget(&mLabel, 0, 0);
    /// которое помещаем в виджет
    setLayout(&mLayout);

    /// По сигналу таймера будет произведено скрытие уведомления, если оно видимо
    mTimer = new QTimer();
    connect(mTimer, &QTimer::timeout, this, &Notification::hideAnimation);
}

void Notification::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)

    QPainter painter(this);
    /// Включаем сглаживание
    painter.setRenderHint(QPainter::Antialiasing);

    /// Подготавливаем фон. rect() возвращает внутреннюю геометрию виджета уведомления, по содержимому
    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    /// Кисть настраиваем на чёрный цвет в режиме полупрозрачности 180 из 255
    painter.setBrush(QBrush(QColor(0,0,0,180)));
    painter.setPen(Qt::NoPen); /// Край уведомления не будет выделен

    /// Отрисовываем фон с закруглением краёв в 10px
    painter.drawRoundedRect(roundedRect, 10, 10);
}

void Notification::setNotificationText(const QString &text){
    /// Устанавилваем текст в Label
    mLabel.setText(text);
    /// С пересчётом размеров уведомления
    adjustSize();
}

void Notification::show(){
    /// Устанавливаем прозрачность в ноль
    setWindowOpacity(0.0);
    /// Настравиваем длительность анимации
    mAnimation.setDuration(150);
    /// Стартовое значение будет 0 (полностью прозрачный виджет)
    mAnimation.setStartValue(0.0);
    /// Конечное - полностью непрозрачный виджет
    mAnimation.setEndValue(1.0);

    /// Установка позиции окна
    setGeometry(QApplication::desktop()->availableGeometry().width() - 36 - width() + QApplication::desktop() -> availableGeometry().x(),
                QApplication::desktop()->availableGeometry().height() - 36 - height() + QApplication::desktop() -> availableGeometry().y(),
                width(),
                height());
    /// Отображаем виджет, который полностью прозрачен
    QWidget::show();
    /// И запускаем анимацию
    mAnimation.start();
    /// А также стартуем таймер, который запустит скрытие уведомления через 3 секунды
    mTimer->start(3000);
}

void Notification::hideAnimation(){
    /// Останавлвиваем таймер
    mTimer->stop();
    /// Настравиваем длительность анимации
    mAnimation.setDuration(1000);
    /// Стартовое значение будет 1 (полностью непрозрачный виджет)
    mAnimation.setStartValue(1.0);
    /// Конечное - полностью прозрачный виджет
    mAnimation.setEndValue(0.0);
    /// И запускаем анимацию
    mAnimation.start();
}

void Notification::hide(){
    if(getNotificationOpacity() == 0.0){
        QWidget::hide();
    }
}

void Notification::setNotificationOpacity(float opacity){
    mNotificationOpacity = opacity;
    setWindowOpacity(mNotificationOpacity);
}

float Notification::getNotificationOpacity() const{
    return mNotificationOpacity;
}

