TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lkrpc -lprotobuf -pthread -lpthread

SOURCES += \
        main.cpp \
    DisplayMsg.cpp \
    DisplayThread.cpp \
    LaunchThread.cpp \
    RocketData.cpp \
    RocketMainThread.cpp \
    ThreadInterface.cpp \
    HoverThread.cpp \
    regulatorpd.cpp \
    regulatorpi.cpp \
    regulatorpid.cpp \
    signalgenerator.cpp

HEADERS += \
    DisplayMsg.h \
    DisplayThread.h \
    LaunchThread.h \
    RocketData.h \
    RocketMainThread.h \
    ThreadInterface.h \
    StreamQueue.h \
    HoverThread.h \
    regulator.h \
    regulatorpd.h \
    regulatorpi.h \
    regulatorpid.h \
    SISOSystem.h \
    signalgenerator.h \
    signalgeneratorinterface.h
