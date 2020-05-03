QT += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = img_resizer_gui
TEMPLATE = app

SOURCES += \
        main.cpp \
        widget.cpp

HEADERS += widget.h

include(../../common.pri)

MOC_DIR = $${BUILD_PATH}/gui/moc
OBJECTS_DIR = $${BUILD_PATH}/gui/obj

INCLUDEPATH += $${ROOT_PATH}/src/lib
LIBS += -L$${BIN_PATH} -lresizer$${LIB_SUFFIX}
