#include "notification.h"

#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

namespace {
    const int MARGIN_SIZE = 5;
    const int ROUNDED_RECT_RADIUS = 10;
    const int TRANSPARENCY_LEVEL = 180;
    const int SHOW_ANIMATION_DURATION = 150;  // milliseconds
    const int HIDE_ANIMATION_DURATION = 1000; // milliseconds
    const int NOTIFICATION_DISPLAY_DURATION = 3000; // milliseconds
    const int GEOMETRY_OFFSET = 36;
}

Notification::Notification(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint |        // Disable window frame
                   Qt::Tool |                       // Remove window from the taskbar
                   Qt::WindowStaysOnTopHint);       // Set window to stay on top of all other windows
    setAttribute(Qt::WA_TranslucentBackground);     // Set background to be transparent
    setAttribute(Qt::WA_ShowWithoutActivating);     // Ensure the widget does not gain focus when shown

    // Set the target object for the animation
    this->mAnimation.setTargetObject(this);
    // Set the property to be animated
    this->mAnimation.setPropertyName("mNotificationOpacity");
    // Connect the animation's finished signal to the hide slot
    connect(&mAnimation, &QAbstractAnimation::finished, this, &Notification::hide);

    // Configure the notification text
    this->mLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // Set styles
    this->mLabel.setStyleSheet("QLabel { color : white; "
                               "margin-top: 6px;"
                               "margin-bottom: 6px;"
                               "margin-left: 10px;"
                               "margin-right: 10px; }");

    // Place the text label into the layout
    this->mLayout.addWidget(&mLabel, 0, 0);
    // Set the layout to the widget
    setLayout(&mLayout);

    // The timer will trigger the hiding animation if the notification is still visible
    this->mTimer = new QTimer();
    connect(this->mTimer, &QTimer::timeout, this, &Notification::hideAnimation);
}

void Notification::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    // Enable anti-aliasing
    painter.setRenderHint(QPainter::Antialiasing);

    // Prepare the background. rect() returns the internal geometry of the notification widget
    QRect roundedRect;
    roundedRect.setX(rect().x() + MARGIN_SIZE);
    roundedRect.setY(rect().y() + MARGIN_SIZE);
    roundedRect.setWidth(rect().width() - 2 * MARGIN_SIZE);
    roundedRect.setHeight(rect().height() - 2 * MARGIN_SIZE);

    // Set the brush to black with a transparency level
    painter.setBrush(QBrush(QColor(0, 0, 0, TRANSPARENCY_LEVEL)));
    painter.setPen(Qt::NoPen); // The edge of the notification will not be highlighted

    // Draw the background with rounded corners
    painter.drawRoundedRect(roundedRect, ROUNDED_RECT_RADIUS, ROUNDED_RECT_RADIUS);
}

void Notification::setNotificationText(const QString &text) {
    // Set the text in the label
    this->mLabel.setText(text);
    // Adjust the size of the notification to fit the text
    adjustSize();
}

void Notification::show() {
    // Set the initial opacity to zero
    setWindowOpacity(0.0);
    // Configure the animation duration
    this->mAnimation.setDuration(SHOW_ANIMATION_DURATION);
    // Start with the widget fully transparent
    this->mAnimation.setStartValue(0.0);
    // End with the widget fully opaque
    this->mAnimation.setEndValue(1.0);

    // Set the position of the window
    setGeometry(QApplication::desktop()->availableGeometry().width() - GEOMETRY_OFFSET - width() + QApplication::desktop()->availableGeometry().x(),
                QApplication::desktop()->availableGeometry().height() - GEOMETRY_OFFSET - height() + QApplication::desktop()->availableGeometry().y(),
                width(),
                height());
    // Show the widget (which is fully transparent initially)
    QWidget::show();
    // Start the animation
    this->mAnimation.start();
    // Start the timer, which will trigger the hide animation after a delay
    this->mTimer->start(NOTIFICATION_DISPLAY_DURATION);
}

void Notification::hideAnimation() {
    // Stop the timer
    this->mTimer->stop();
    // Configure the animation duration
    this->mAnimation.setDuration(HIDE_ANIMATION_DURATION);
    // Start with the widget fully opaque
    this->mAnimation.setStartValue(1.0);
    // End with the widget fully transparent
    this->mAnimation.setEndValue(0.0);
    // Start the animation
    this->mAnimation.start();
}

void Notification::hide() {
    if(this->getNotificationOpacity() == 0.0) {
        QWidget::hide();
    }
}

void Notification::setNotificationOpacity(float opacity) {
    this->mNotificationOpacity = opacity;
    setWindowOpacity(this->mNotificationOpacity);
}

float Notification::getNotificationOpacity() const {
    return this->mNotificationOpacity;
}
