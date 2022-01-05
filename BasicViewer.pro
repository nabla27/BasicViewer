QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(completer/completer.pri)
include(io/io.pri)
include(numanalysis/numanalysis.pri)
include(qgraph/qgraph.pri)

SOURCES += \
    basicset.cpp \
    cmdline.cpp \
    filetree.cpp \
    gnuplot.cpp \
    main.cpp \
    mainwindow.cpp \
    setwindow.cpp \
    syntaxhighlighter.cpp \
    tablewidget.cpp \
    textbrowser.cpp \
    textedit.cpp

HEADERS += \
    basicset.h \
    cmdline.h \
    filetree.h \
    gnuplot.h \
    mainwindow.h \
    syntaxhighlighter.h \
    tablewidget.h \
    textbrowser.h \
    textedit.h

INCLUDEPATH += E:/boost_1_76_0/boost_1_76_0

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
