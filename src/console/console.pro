QT = core

CONFIG += c++11
CONFIG += console
CONFIG -= gui

TARGET = img_resizer_cli
TEMPLATE = app

SOURCES += main.cpp \
    resizerconsole.cpp

include(../../common.pri)

MOC_DIR = $${BUILD_PATH}/console/moc
OBJECTS_DIR = $${BUILD_PATH}/console/obj

INCLUDEPATH += $${ROOT_PATH}/src/lib
LIBS += -L$${BIN_PATH} -lresizer$${LIB_SUFFIX}

HEADERS += \
    resizerconsole.h
