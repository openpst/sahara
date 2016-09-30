#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 5): error("At least Qt 5.0 is required")

QT += core gui widgets

CONFIG += C++11

TARGET = sahara

TEMPLATE = app

INCLUDEPATH += \
	$$PWD/include \
	$$PWD/lib/libopenpst/include \
	$$PWD/lib/libopenpst/lib/serial/include \
	$$PWD/lib/gui-common/include

DEPENDPATH += $$PWD/

VPATH += $$PWD/

SOURCES += \
    src/sahara_window.cpp \
    src/task/sahara_image_transfer_task.cpp \
    src/task/sahara_memory_read_task.cpp \
    src/main.cpp

HEADERS  += \
    include/sahara_window.h \
    include/task/sahara_image_transfer_task.h \
    include/task/sahara_memory_read_task.h


FORMS  += resources/ui/sahara_window.ui

RESOURCES = resources/sahara.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/  -llibopenpst -lserial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug/ -llibopenpst -lserial
else:unix: LIBS += -L$$OUT_PWD/ -llibopenpst -lserial
