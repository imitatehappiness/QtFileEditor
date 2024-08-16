#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

/*!
 * \class Notification
 * \brief The class responsible for popup notifications
 */
class Notification : public QWidget{
    Q_OBJECT
    /// Property for opacity
    Q_PROPERTY(float mNotificationOpacity READ getNotificationOpacity WRITE setNotificationOpacity)
    /// Set the opacity value
    void setNotificationOpacity(float opacity);
    /// Get the opacity value
    float getNotificationOpacity() const;
public:
    /// Constructor
    explicit Notification(QWidget *parent = 0);
protected:
    /// Background will be drawn via the paint event method
    void paintEvent(QPaintEvent *event);
public slots:
    /// Set the text of the notification
    void setNotificationText(const QString& text);
    /// Custom method to show the widget
    /// Necessary for pre-setting the animation
    void show();
private slots:
    /// Slot to start the hide animation
    void hideAnimation();
    /// After the animation ends, this slot checks
    /// if the widget is visible or needs to be hidden
    void hide();
private:
    /// Label with the message
    QLabel mLabel;
    /// Layout for the label
    QGridLayout mLayout;
    /// Property animation for the popup message
    QPropertyAnimation mAnimation;
    /// Opacity property of the widget
    float mNotificationOpacity;
    /// Timer to hide the widget
    QTimer *mTimer;
};
