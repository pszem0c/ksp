TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += krpc protobuf

SOURCES += \
        main.cpp \
    DisplayMsg.cpp \
    DisplayThread.cpp \
    LaunchThread.cpp \
    RocketData.cpp \
    RocketMainThread.cpp \
    ThreadInterface.cpp

HEADERS += \
    DisplayMsg.h \
    DisplayThread.h \
    LaunchThread.h \
    RocketData.h \
    RocketMainThread.h \
    ThreadInterface.h
