QT += concurrent core

CONFIG += c++11
CONFIG += shared

TEMPLATE = lib

DEFINES += RESIZER_LIB

SOURCES += \
        resizer.cpp \
        resizerwork.cpp

HEADERS += \
        resizer.h \
        resizerwork.h

include(../../common.pri)

MOC_DIR = $${BUILD_PATH}/resizer/moc
OBJECTS_DIR = $${BUILD_PATH}/resizer/obj

TARGET = resizer$${LIB_SUFFIX}
