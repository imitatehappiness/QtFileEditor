QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    notification.cpp

HEADERS += \
    mainwindow.h \
    notification.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc

DISTFILES += \
    resources/icons/appIcon.png \
    resources/icons/fileIcons/close_file.png \
    resources/icons/fileIcons/delete_file.png \
    resources/icons/fileIcons/new_file.png \
    resources/icons/fileIcons/open_file.png \
    resources/icons/fileIcons/save_as_file.png \
    resources/icons/fileIcons/save_file.png
