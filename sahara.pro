#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += C++11

TARGET = sahara

TEMPLATE = app

INCLUDEPATH += $$PWD/../src $$PWD/../lib/serial/include

DEPENDPATH += $$PWD/../

VPATH += $$PWD/../

SOURCES += \
    src/util/hexdump.cpp \
    src/gui/sahara_window.cpp \
    src/gui/worker/sahara_memory_read_worker.cpp \
    src/gui/worker/sahara_image_transfer_worker.cpp \
    src/gui/application.cpp \
    src/sahara.cpp

HEADERS  += \
    src/include/definitions.h \
    src/util/hexdump.h \
    src/gui/sahara_window.h \
    src/qc/sahara_serial.h \
    src/gui/worker/sahara_memory_read_worker.h \
    src/gui/worker/sahara_image_transfer_worker.h \
    src/gui/application.h


FORMS  += resources/ui/sahara_window.ui

RESOURCES = resources/sahara.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/  -llibopenpst -lserial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug/ -llibopenpst -lserial
else:unix: LIBS += -L$$OUT_PWD/ -llibopenpst -lserial
