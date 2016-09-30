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

###
## Add in used gui-common stuff for sahara
###

SOURCES += \
    $$PWD/lib/gui-common/src/task/task_runner.cpp \
    $$PWD/lib/gui-common/src/widget/log_widget.cpp \
    $$PWD/lib/gui-common/src/widget/progress_group_widget.cpp \
    $$PWD/lib/gui-common/src/about_dialog.cpp \
    $$PWD/lib/gui-common/src/application.cpp

HEADERS  += \
    $$PWD/lib/gui-common/include/task/task.h \
    $$PWD/lib/gui-common/include/task/task_runner.h \
    $$PWD/lib/gui-common/include/widget/log_widget.h \
    $$PWD/lib/gui-common/include/widget/progress_group_widget.h \
    $$PWD/lib/gui-common/include/about_dialog.h \
    $$PWD/lib/gui-common/include/application.h

FORMS  += $$PWD/lib/gui-common/resources/ui/about_dialog.ui
FORMS  += $$PWD/lib/gui-common/resources/ui/log_widget.ui
FORMS  += $$PWD/lib/gui-common/resources/ui/progress_group_widget.ui

###
## Make libopenpst and link against it
###

QMAKE_EXTRA_TARGETS += libopenpst
PRE_TARGETDEPS += libopenpst
libopenpst.commands = cd ./../lib/libopenpst/ && make qmake

LIBS += -L./../lib/libopenpst/build -lopenpst


