#-------------------------------------------------
# QMake Build Script for: openpst/sahara
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 5): error("At least Qt 5.0 is required")

QT       += core gui widgets
CONFIG   += C++11
TARGET   = sahara
TEMPLATE = app

equals(BASE_DIR, ""):  BASE_DIR = $$PWD
equals(BUILD_DIR, ""): BUILD_DIR = $$BASE_DIR/build/linux/$$QT_ARCH

equals(BASE_DIR, ""):  error("Missing BASE_DIR")
equals(BUILD_DIR, ""): error("Missing BUILD_DIR")

INCLUDEPATH +=  $$BASE_DIR/include \
                $$BASE_DIR/lib/libopenpst/include \
                $$BASE_DIR/lib/libopenpst/lib/serial/include \
                $$BASE_DIR/lib/gui-common/include 
DEPENDPATH  += $$BASE_DIR/
VPATH       += $$BASE_DIR/
OBJECTS_DIR += $$BUILD_DIR/obj
MOC_DIR     += $$BUILD_DIR/moc
RCC_DIR     += $$BUILD_DIR/rcc
UI_DIR      += $$BUILD_DIR/ui
DESTDIR     += $$BUILD_DIR

message("------------ $$TARGET ------------ ")
message("ARC         $$QT_ARCH")
message("BASE_DIR    $$BASE_DIR")
message("BUILD_DIR   $$BUILD_DIR")
message("INCLUDEPATH $$INCLUDEPATH")
message("DEPENDPATH  $$DEPENDPATH")
message("VPATH       $$VPATH")
message("OBJECTS_DIR $$OBJECTS_DIR")
message("MOC_DIR     $$MOC_DIR")
message("RCC_DIR     $$RCC_DIR")
message("UI_DIR      $$UI_DIR")
message("DESTDIR     $$DESTDIR")
message("------------ $$TARGET ------------ ")

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

###
## Include gui-common .pro
###
GUICOMMON_DIR = $$BASE_DIR/lib/gui-common
include($$GUICOMMON_DIR/gui-common.pro)

###
## Make libopenpst and link against it
###
QMAKE_EXTRA_TARGETS += libopenpst
PRE_TARGETDEPS 		+= libopenpst
libopenpst.commands = cd $$BASE_DIR/lib/libopenpst && make

LIBS += -L$$BASE_DIR/lib/libopenpst/build/linux/$$QT_ARCH -lopenpst