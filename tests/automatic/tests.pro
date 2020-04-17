QT += testlib
QT += core gui widgets
QT += network
QT += xml

CONFIG += c++11                                                                                                                                   
LIBS += -pthread -lrt -lzip -lz -Llib -L/usr/local/lib/  

QT -= gui -lzip

TARGET = Tests
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    testresourcemanager.cpp \
    testworkspace.cpp \
    testpatrol.cpp \
    ../../core/appinstance.cpp \
    ../../core/helper.cpp \
    ../../core/workspace.cpp \
    ../../models/configurationsettings.cpp \
    ../../models/costmap.cpp \
    ../../models/map.cpp \
    ../../models/patrol.cpp \
    ../../models/sensor.cpp \
    ../../models/user.cpp \
    ../../models/zone.cpp \
    ../../resource_manager/loaders/configurationsettingsloader.cpp \
    ../../resource_manager/loaders/maploader.cpp \
    ../../resource_manager/loaders/patrol_loader.cpp \
    ../../resource_manager/loaders/sensorloader.cpp \
    ../../resource_manager/loader.cpp \
    ../../resource_manager/resource_manager.cpp \
    ../../resource_manager/ziparchive.cpp \
    ../../resource_manager/loaders/zoneloader.cpp \
    testzone.cpp


HEADERS += \
    autorun.h \
    testresourcemanager.h \ 
    testworkspace.h \
    testpatrol.h \
    ../../core/appinstance.h \
    ../../core/constants.h \
    ../../core/helper.h \
    ../../core/singleton.h \
    ../../core/workspace.h \
    ../../models/configurationsettings.h \
    ../../models/costmap.h \
    ../../models/map.h \
    ../../models/patrol.h \
    ../../models/sensor.h \
    ../../models/user.h \
    ../../models/zone.h \
    ../../resource_manager/loaders/configurationsettingsloader.h \
    ../../resource_manager/loaders/maploader.h \
    ../../resource_manager/loaders/patrol_loader.h \
    ../../resource_manager/loaders/sensorloader.h \
    ../../resource_manager/loader.h \
    ../../resource_manager/resource_manager.h \
    ../../resource_manager/resource.h \
    ../../resource_manager/ziparchive.h \
    ../../resource_manager/loaders/zoneloader.h \
    testzone.h

INCLUDEPATH += ../..
