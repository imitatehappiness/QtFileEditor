#include "windowframe.h"
#include "ui_windowframe.h"

#include <windows.h>
#include <windowsx.h>

#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>

const QString TITLE = "";

const QString headerDefaultStyle = QStringLiteral(
    "#header {"
    "    background-color: rgb(20, 20, 20);"
    "    border: 1px solid rgb(20, 20, 20);"
    "    border-top-left-radius: 10px;"
    "    border-top-right-radius: 10px;"
    "}"
);

const QString headerCollapseStyle = QStringLiteral(
    "#header {"
    "    background-color: rgb(20, 20, 20);"
    "    border: 2px solid rgb(20, 20, 20);"
    "    border-top-left-radius: 10px;"
    "    border-top-right-radius: 10px;"
    "    border-bottom-left-radius: 10px;"
    "    border-bottom-right-radius: 10px;"
    "}"
);

const QString headerMaximizeStyle = QStringLiteral(
    "#header {"
    "    background-color: rgb(20, 20, 20);"
    "    border: 1px solid rgb(20, 20, 20);"
    "    border-top-left-radius: 0px;"
    "    border-top-right-radius: 0px;"
    "}"
);

const QString headerIcon        = "";
const QString closeIcon         = ":/resources/icons/close_light.png";
const QString collapseHideIcon  = ":/resources/icons/collapse_hide_light.png";
const QString collapseShowIcon  = ":/resources/icons/collapse_show_light.png";
const QString maximizeIcon      = ":/resources/icons/maximize_light.png";
const QString minimizeIcon      = ":/resources/icons/minimize_light.png";
const QString defaultSizeIcon   = ":/resources/icons/default_size_light.png";

/// @brief Constructor for the WindowFrame class.
/// @param parent The parent widget.
/// @param child The child widget to be added to the window (optional).
WindowFrame::WindowFrame(QWidget *parent, QWidget *child)
    : QFrame(parent), ui(new Ui::WindowFrame), mChild(child){

    ui->setupUi(this);
    this->mBorderSize = 5;

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.97);

    ui->bodyFrame->setGraphicsEffect(opacityEffect);

    setWindowOpacity(1.0);

    initIcons();

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground);

    if(child != nullptr) {
        ui->body->layout()->addWidget(child);
        mMainBody = child;
        mMainBody->installEventFilter(this);
        resize(child->size());
    }
    this->mIsCollapse = false;
}

/// @brief Destructor for the WindowFrame class.
WindowFrame::~WindowFrame(){
    delete ui;
}

/// @brief Init frame icons.
void WindowFrame::initIcons(){
    ui->collapse->setIcon(QIcon(collapseHideIcon));
    ui->close->setIcon(QIcon(closeIcon));
    ui->maximum->setIcon(QIcon(maximizeIcon));
    ui->minimum->setIcon(QIcon(minimizeIcon));
}

/// @brief Show header menu.
/// @param pos position mouse click.
void WindowFrame::showHeaderContextMenu(const QPoint &pos){
    QMenu contextMenu(this);

    QAction *exitAction = contextMenu.addAction(tr("&Exit"));
    connect(exitAction, &QAction::triggered, this, &WindowFrame::close);

    contextMenu.addAction(exitAction);
    contextMenu.exec(mapToGlobal(pos));
}

/// @brief Handler for the "Close" button click signal.
void WindowFrame::on_close_clicked(){
    mChild->close();
}

/// @brief Handler for the "Maximize/Restore" button click signal.
void WindowFrame::on_maximum_clicked(){
    if(isMaximized()) {
        ui->maximum->setIcon(QIcon(maximizeIcon));
        this->mIsCollapse ? ui->header->setStyleSheet(headerCollapseStyle) : ui->header->setStyleSheet(headerDefaultStyle);
        showNormal();
    } else {
        ui->maximum->setIcon(QIcon(defaultSizeIcon));
        ui->header->setStyleSheet(headerMaximizeStyle);
        showMaximized();
    }
}

/// @brief Handler for the "Minimize" button click signal.
void WindowFrame::on_minimum_clicked(){
    showMinimized();
}

/// @brief Handler for the "Collapse" button click signal.
void WindowFrame::on_collapse_clicked() {
    if (this->mIsCollapse) {
        ui->body->setVisible(true);
        this->mIsCollapse = false;
        ui->collapse->setIcon(QIcon(collapseHideIcon));
        isMaximized() ? ui->header->setStyleSheet(headerMaximizeStyle) : ui->header->setStyleSheet(headerDefaultStyle);
    } else {
        ui->body->setVisible(false);
        this->mIsCollapse = true;
        ui->collapse->setIcon(QIcon(collapseShowIcon));
        isMaximized() ? ui->header->setStyleSheet(headerMaximizeStyle) : ui->header->setStyleSheet(headerCollapseStyle);
    }
}


/// @brief Handler for the mouse press event.
/// @param event Pointer to the QMouseEvent object containing event information.
void WindowFrame::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        QWidget* widget = childAt(event->x(), event->y());
        if(widget == ui->LHeader) {
            this->mPosition.setX(event->x());
            this->mPosition.setY(event->y());
        }
    }
    if (event->button() == Qt::RightButton ) {
        QWidget* widget = childAt(event->x(), event->y());
        if (widget == ui->LHeader){
            showHeaderContextMenu(event->pos());
        }
    }
}

/// @brief Handler for the mouse move event within the window.
/// @param event Pointer to the mouse move event object (QMouseEvent).
/// @return No return value.
void WindowFrame::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        if (this->mPosition.x() != 0 || this->mPosition.y() != 0) {
            move(event->globalX() - this->mPosition.x(), event->globalY() - this->mPosition.y());
        }
    }
}

/// @brief Handler for the mouse release event within the window.
/// @param event Pointer to the mouse release event object (QMouseEvent).
void WindowFrame::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    this->mPosition.setX(0);
    this->mPosition.setY(0);
}

/// @brief Handler for the mouse double-click event within the window.
/// @param event Pointer to the mouse double-click event object (QMouseEvent).
void WindowFrame::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        QWidget* widget = childAt(event->x(), event->y());
        if(widget == ui->LHeader) {
            if(isMaximized()) {
                ui->maximum->setIcon(QIcon(maximizeIcon));
                ui->header->setStyleSheet(headerDefaultStyle);
                showNormal();
            } else {
                ui->maximum->setIcon(QIcon(defaultSizeIcon));
                ui->header->setStyleSheet(headerMaximizeStyle);
                showMaximized();
            }
        }
    }
}

/// @brief Handler for the native window event.
/// @param eventType The type of event, as a byte array (QByteArray).
/// @param message Pointer to a structure containing event information (void*).
/// @param result Pointer to a variable for returning the result (long*).
/// @return The return value, true if the event was handled, otherwise false.
bool WindowFrame::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(eventType)
    MSG *param = static_cast<MSG *>(message);

    if (param->message == WM_NCHITTEST) {
        QPoint globalPos(GET_X_LPARAM(param->lParam), GET_Y_LPARAM(param->lParam));
        QPoint localPos = mapFromGlobal(globalPos);

        int nX = localPos.x();
        int nY = localPos.y();

        if (nX >= 0 && nX < this->mBorderSize) {
            if (nY >= 0 && nY < this->mBorderSize) {
                *result = HTTOPLEFT;
            } else if (nY >= height() - this->mBorderSize) {
                *result = HTBOTTOMLEFT;
            } else {
                *result = HTLEFT;
            }
        } else if (nX >= width() - this->mBorderSize) {
            if (nY >= 0 && nY < this->mBorderSize) {
                *result = HTTOPRIGHT;
            } else if (nY >= height() - this->mBorderSize) {
                *result = HTBOTTOMRIGHT;
            } else {
                *result = HTRIGHT;
            }
        } else if (nY >= 0 && nY < this->mBorderSize) {
            *result = HTTOP;
        } else if (nY >= height() - this->mBorderSize) {
            *result = HTBOTTOM;
        } else {
            return QWidget::nativeEvent(eventType, message, result);
        }

        return true;
    }

    return QWidget::nativeEvent(eventType, message, result);
}

/// @brief Show or hide the window minimization button.
/// @param enable If true, the button will be shown; if false, it will be hidden.
void WindowFrame::enableMinimum(bool enable) {
    !enable ? ui->minimum->hide() : ui->minimum->show();
}

/// @brief Show or hide the window maximization button.
/// @param enable If true, the button will be shown; if false, it will be hidden.
void WindowFrame::enableMaximum(bool enable) {
    !enable ? ui->maximum->hide() : ui->maximum->show();
}

/// @brief Show or hide the window close button.
/// @param enable If true, the button will be shown; if false, it will be hidden.
void WindowFrame::enableClose(bool enable) {
    !enable ? ui->close->hide() : ui->close->show();
}

void WindowFrame::closeEvent(QCloseEvent *event){
    mChild->close();
}

/// @brief Override event filtering function for the WindowFrame class.
/// @param obj Pointer to the object for which the event was generated.
/// @param event Pointer to the QEvent object representing the event.
/// @return `true` if the event was handled, otherwise `false`.
bool WindowFrame::eventFilter(QObject *obj, QEvent *event) {
    if(obj == mMainBody) {
        if(event->type() == QEvent::HideToParent) {
            hide();
            return true;
        }
        if(event->type() == QEvent::ShowToParent) {
            show();
            return true;
        }
        return QObject::eventFilter(obj, event);
    } else {
        return QFrame::eventFilter(obj, event);
    }
    return false;
}


