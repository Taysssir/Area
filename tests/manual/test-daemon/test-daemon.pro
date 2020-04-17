#-------------------------------------------------
#
# Project created by QtCreator 2015-02-02T08:53:11
#
#-------------------------------------------------

QT       += core gui xml svg

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test-daemon
TEMPLATE = app


INCLUDEPATH += $$PWD/../../../libskynet/include/
INCLUDEPATH += $$PWD/../../../libskynet/src/
INCLUDEPATH += $$PWD/../../../libskynet/src/platform/ubuntu64_14/

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../../libskynet/src/internal/ipaddress.cpp \
    ../../../libskynet/src/internal/listener.cpp \
    ../../../libskynet/src/internal/packet.cpp \
    ../../../libskynet/src/internal/socket.cpp \
    ../../../libskynet/src/engine/robot_impl.cpp \
    ../../../libskynet/src/engine/robot.cpp \
    ../../../libskynet/src/engine/server_impl.cpp \
    ../../../libskynet/src/engine/server.cpp \
    ../../../libskynet/src/platform/ubuntu64_14/skynet.pb.cpp

HEADERS  += mainwindow.h \
    ../../../libskynet/include/constants.h \
    ../../../libskynet/include/robot.h \
    ../../../libskynet/include/server.h \
    ../../../libskynet/src/internal/ipaddress.h \
    ../../../libskynet/src/internal/listener.h \
    ../../../libskynet/src/internal/packet.h \
    ../../../libskynet/src/internal/socket.h \
    ../../../libskynet/src/internal/utils.h \
    ../../../libskynet/src/engine/message_handler.h \
    ../../../libskynet/src/engine/robot_impl.h \
    ../../../libskynet/src/engine/server_impl.h \
    ../../../libskynet/src/platform/ubuntu64_14/skynet.pb.h

FORMS    += mainwindow.ui
    win32 {
CONFIG -= qt_windows
QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS
INCLUDEPATH += C:/3rdparts/include
DEPENDPATH +=  C:/3rdparts/include
LIBS += -L"C:\3rdparts\lib"
LIBS += -lws2_32

LIBS += -pthread -lzip
}

LIBS += -lprotobuf
