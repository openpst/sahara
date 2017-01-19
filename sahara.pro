#-------------------------------------------------
# QMake Build Script for: openpst/sahara
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 5): error("At least Qt 5.0 is required")

QT       += core gui widgets xml
CONFIG   += C++11
TARGET   = sahara
TEMPLATE = app

equals(BASE_DIR, ""):  BASE_DIR = $$PWD

include($$BASE_DIR/lib/gui-common/gui-common.pri)
include($$BASE_DIR/lib/gui-common/gui-common.pro)
include($$BASE_DIR/lib/libopenpst/libopenpst.pri)

INCLUDEPATH +=  $$BASE_DIR/include \
                $$BASE_DIR/lib/libopenpst/include \
                $$BASE_DIR/lib/libopenpst/lib/serial/include \
                $$BASE_DIR/lib/gui-common/include 

SOURCES += \
    $$BASE_DIR/src/sahara_window.cpp \
    $$BASE_DIR/src/task/sahara_image_transfer_task.cpp \
    $$BASE_DIR/src/task/sahara_memory_read_task.cpp \
    $$BASE_DIR/src/main.cpp

HEADERS  += \
    $$BASE_DIR/include/sahara_window.h \
    $$BASE_DIR/include/task/sahara_image_transfer_task.h \
    $$BASE_DIR/include/task/sahara_memory_read_task.h

FORMS  += $$BASE_DIR/resources/ui/sahara_window.ui

RESOURCES = $$BASE_DIR/resources/sahara.qrc

