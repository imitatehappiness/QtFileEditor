QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basesyntaxhighlighter.cpp \
    codeeditor.cpp \
    cppsyntaxhighlighter.cpp \
    csssyntaxhighlighter.cpp \
    htmlsyntaxhighlighter.cpp \
    jsonsyntaxhighlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    notification.cpp \
    pythonsyntaxhighlighter.cpp \
    searchwidget.cpp \
    terminalsyntaxhighlighter.cpp \
    termwidget.cpp \
    windowframe.cpp \
    xmlsyntaxhighlighter.cpp \
    xsltsyntaxhighlighter.cpp

HEADERS += \
    basesyntaxhighlighter.h \
    codeeditor.h \
    cppsyntaxhighlighter.h \
    csssyntaxhighlighter.h \
    customtreeview.h \
    htmlsyntaxhighlighter.h \
    jsonsyntaxhighlighter.h \
    mainwindow.h \
    notification.h \
    pythonsyntaxhighlighter.h \
    searchwidget.h \
    terminalsyntaxhighlighter.h \
    termwidget.h \
    types.h \
    windowframe.h \
    xmlsyntaxhighlighter.h \
    xsltsyntaxhighlighter.h


FORMS += \
    mainwindow.ui \
    searchwidget.ui \
    windowframe.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc
