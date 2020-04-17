QT       += core gui xml svg
QT += network

CONFIG += c++11
CONFIG += static

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sentinel-admin
TEMPLATE = app

win32:RC_ICONS += sentinel.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    ui/dialoglogin.cpp \
    ui/map/qgraphicssvgs.cpp \
    ui/map/mapwidget.cpp \
    ui/map/mapgraphicsview.cpp \
    ui/admin/patroladminwidget.cpp \
    ui/map/patrolgraphicsitem.cpp \
    ui/admin/patrollistviewdelegate.cpp \
    ui/map/zonegraphicsitem.cpp \
    ui/dialogabout.cpp \
    ui/config/dialogconfiguration.cpp \
    ui/config/robotconfiguration.cpp \
    remote/robot.cpp \
    ui/admin/patrolstab.cpp \
    tools/patrolcolor.cpp \
    ui/config/networkconfiguration.cpp \
    sentinel-admin_core/qt_utils.cpp \
    ui/dialognewsensor.cpp \
    ui/map/sensorpointgraphicsitem.cpp \
    evigilante_sdk/src/tcp/ipaddress.cpp \
    evigilante_sdk/src/tcp/listener.cpp \
    evigilante_sdk/src/tcp/logger.cpp \
    evigilante_sdk/src/tcp/packet.cpp \
    evigilante_sdk/src/tcp/socket.cpp \
    evigilante_sdk/src/engine/backend_treatment.cpp \
    evigilante_sdk/src/engine/client.cpp \
    evigilante_sdk/src/engine/client_impl.cpp \
    evigilante_sdk/src/engine/log_interface.cpp \
    evigilante_sdk/src/engine/server_callsystem.cpp \
    evigilante_sdk/src/engine/server.cpp \
    evigilante_sdk/src/engine/server_impl.cpp \
    evigilante_sdk/src/engine/msgs.pb.cc \
    evigilante_configuration/source/pimpl_configuration.cpp \
    evigilante_configuration/source/resource_manager/loaders/zones_loader.cpp \
    evigilante_configuration/source/resource_manager/loaders/map_loader.cpp \
    evigilante_configuration/source/resource_manager/loaders/patrols_loader.cpp \
    evigilante_configuration/source/resource_manager/loaders/settings_loader.cpp \
    evigilante_configuration/source/resource_manager/loaders/sensors_loader.cpp \
    evigilante_configuration/source/resource_manager/loaders/tasksscheduled_loader.cpp \
    evigilante_configuration/source/resource_manager/loader.cpp \
    evigilante_configuration/source/resource_manager/ziparchive.cpp \
    evigilante_configuration/source/resource_manager/resource_manager.cpp \
    evigilante_configuration/source/configuration.cpp \
    evigilante_configuration/source/models/models.pb.cc \
    sentinel-common/core/workspace.cpp \
    sentinel-common/core/helper.cpp \
    sentinel-common/core/eventmanager.cpp \
    sentinel-common/core/appinstance.cpp \
    sentinel-common/users/user.cpp \
    sentinel-common/users/userloader.cpp \
    sentinel-common/helper/map.cpp \
    sentinel-common/core/sentinel_logger.cpp \
    sentinel-common/sftp/sftp.cpp \
    sentinel-common/sftp/transfer.cpp \
    ui/calendar/tasksscheduled.cpp \
    ui/calendar/taskscheduled.cpp \
    ui/calendar/dialogcreateplannification.cpp \
    ui/calendar/calendarwidget.cpp \
    ui/calendar/Graphics/calendarbackgroundscene.cpp \
    ui/calendar/Graphics/calendarpatrolscene.cpp \
    ui/calendar/Graphics/GraphicsItem/backgrounditem.cpp \
    ui/calendar/Graphics/GraphicsItem/daybackgrounditem.cpp \
    ui/calendar/Graphics/GraphicsItem/weekbackgrounditem.cpp \
    ui/calendar/Graphics/GraphicsItem/monthbackgrounditem.cpp \
    ui/calendar/Graphics/GraphicsItem/listbackgrounditem.cpp \
    ui/calendar/Graphics/GraphicsItem/graphicspatrolitem.cpp \
    ui/calendar/minicalendarwidget.cpp \
    ui/config/siaconfiguration.cpp \
    ui/calendar/Graphics/GraphicsItem/linetimeitem.cpp \
    ui/map/editablepixmapitem.cpp \
    ui/map/mapcreationwizard.cpp \
    ui/config/restapiconfiguration.cpp \
    ui/config/useraccountsconfiguration.cpp \
    evigilante_sdk/src/http/network.cpp \


HEADERS  += mainwindow.h \
    ui/dialoglogin.h \
    ui/map/qgraphicssvgs.h \
    ui/map/mapwidget.h \
    ui/map/mapgraphicsview.h \
    ui/admin/patroladminwidget.h \
    ui/map/patrolgraphicsitem.h \
    ui/admin/patrollistviewdelegate.h \
    ui/map/zonegraphicsitem.h \
    ui/dialogabout.h \
    ui/config/dialogconfiguration.h \
    ui/config/robotconfiguration.h \
    remote/robot.h \
    ui/admin/patrolstab.h \
    sentinel-admin_core/constants.h \
    sentinel-admin_core/events.h \
    tools/patrolcolor.h \
    ui/config/networkconfiguration.h \
    sentinel-admin_core/qt_utils.h \
    ui/dialognewsensor.h \
    ui/map/sensorpointgraphicsitem.h \
    evigilante_sdk/include/eos/sdk/client.hpp \
    evigilante_sdk/include/eos/sdk/msgs.pb.h \
    evigilante_sdk/include/eos/sdk/log_interface.hpp \
    evigilante_sdk/src/tcp/ipaddress.hpp \
    evigilante_sdk/src/tcp/listener.hpp \
    evigilante_sdk/src/tcp/logger.hpp \
    evigilante_sdk/src/tcp/packet.hpp \
    evigilante_sdk/src/tcp/singleton.hpp \
    evigilante_sdk/src/tcp/socket.hpp  \
    evigilante_sdk/src/engine/client_impl.hpp \
    evigilante_sdk/src/engine/message_handler.hpp \
    evigilante_sdk/src/engine/server_callsystem.hpp  \
    evigilante_sdk/src/engine/server_impl.hpp \
    evigilante_configuration/include/eos/configuration/configuration.hpp \
    evigilante_configuration/source/resource_manager/loaders/sensors_loader.hpp \
    evigilante_configuration/source/resource_manager/loaders/patrols_loader.hpp \
    evigilante_configuration/source/resource_manager/loaders/settings_loader.hpp \
    evigilante_configuration/source/resource_manager/loaders/map_loader.hpp \
    evigilante_configuration/source/resource_manager/loaders/zones_loader.hpp \
    evigilante_configuration/source/resource_manager/resource.hpp \
    evigilante_configuration/source/resource_manager/file.hpp \
    evigilante_configuration/source/resource_manager/ziparchive.hpp \
    evigilante_configuration/source/resource_manager/loader.hpp \
    evigilante_configuration/source/resource_manager/resource_manager.hpp \
    evigilante_configuration/source/pimpl_configuration.hpp \
    evigilante_configuration/include/eos/configuration/models.pb.h \
    evigilante_configuration/include/eos/configuration/configuration.hpp \
    sentinel-common/core/workspace.h \
    sentinel-common/core/singleton.h \
    sentinel-common/core/simplesignal.h \
    sentinel-common/core/helper.h \
    sentinel-common/core/eventmanager.h \
    sentinel-common/core/constants.h \
    sentinel-common/core/appinstance.h \
    sentinel-common/core/iservices.h \
    sentinel-common/users/user.h \
    sentinel-common/users/userloader.h \
    sentinel-common/helper/map.h \
    sentinel-common/sftp/sftp.h \
    sentinel-common/sftp/transfer.h \
    ui/calendar/tasksscheduled.h \
    ui/calendar/taskscheduled.h \
    ui/calendar/dialogcreateplannification.h \
    ui/calendar/calendarwidget.h \
    ui/calendar/contants_calendar.h \
    ui/calendar/Graphics/calendarbackgroundscene.h \
    ui/calendar/Graphics/calendarpatrolscene.h \
    ui/calendar/Graphics/GraphicsItem/backgrounditem.h \
    ui/calendar/Graphics/GraphicsItem/daybackgrounditem.h \
    ui/calendar/Graphics/GraphicsItem/weekbackgrounditem.h \
    ui/calendar/Graphics/GraphicsItem/monthbackgrounditem.h \
    ui/calendar/Graphics/GraphicsItem/listbackgrounditem.h \
    ui/calendar/Graphics/GraphicsItem/graphicspatrolitem.h \
    ui/calendar/minicalendarwidget.h \
    ui/config/siaconfiguration.h \
    ui/calendar/Graphics/GraphicsItem/linetimeitem.h \
    ui/map/editablepixmapitem.h \
    ui/map/mapcreationwizard.h \
    ui/config/restapiconfiguration.h \
    ui/config/useraccountsconfiguration.h \
    evigilante_sdk/src/http/network.h \

FORMS    += mainwindow.ui \
    ui/dialoglogin.ui \
    ui/admin/patroladminwidget.ui \
    ui/dialogabout.ui \
    ui/config/robotconfiguration.ui \
    ui/config/networkconfiguration.ui \
    ui/dialognewsensor.ui \
    ui/calendar/dialogcreateplannification.ui \
    ui/calendar/calendarwidget.ui \
    ui/config/siaconfiguration.ui \
    ui/map/mapcreationwizard.ui \
    ui/config/restapiconfiguration.ui \
    ui/config/useraccountsconfiguration.ui

TRANSLATIONS = sentinel-admin_fr.ts sentinel-admin_en.ts sentinel-admin_de.ts

OTHER_FILES += \
    git.pri

RESOURCES += \
    sentinel-admin.qrc

include(git.pri)

LIBS += -lprotobuf -ltinyxml -lz -lssh2

INCLUDEPATH += ui/admin/
INCLUDEPATH += sentinel-common
INCLUDEPATH += evigilante_sdk/include
INCLUDEPATH += evigilante_sdk/include/eos/sdk/
INCLUDEPATH += evigilante_sdk/src
INCLUDEPATH += evigilante_sdk/src/tcp
INCLUDEPATH += evigilante_configuration/include
INCLUDEPATH += evigilante_configuration/include/eos/configuration/
INCLUDEPATH += evigilante_configuration/source

DEPENDPATH += ui/admin/
DEPENDPATH += sentinel-common
DEPENDPATH += evigilante_sdk/include
DEPENDPATH += evigilante_sdk/include/eos/sdk/
DEPENDPATH += evigilante_sdk/src
DEPENDPATH += evigilante_sdk/src/tcp
DEPENDPATH += evigilante_configuration/include
DEPENDPATH += evigilante_configuration/include/eos/configuration/
DEPENDPATH += evigilante_configuration/source

win32 {
CONFIG -= qt_windows
QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS


INCLUDEPATH += windows-3rdparts/include
INCLUDEPATH += windows-3rdparts/include/ao
INCLUDEPATH += windows-3rdparts/include/asio
INCLUDEPATH += windows-3rdparts/include/libav
INCLUDEPATH += windows-3rdparts/include/SDL2
INCLUDEPATH += windows-3rdparts/include/ssh2
INCLUDEPATH += windows-3rdparts/include/tinyxml
INCLUDEPATH += windows-3rdparts/include/zip

DEPENDPATH  += windows-3rdparts/include
DEPENDPATH  += windows-3rdparts/include/ao
DEPENDPATH  += windows-3rdparts/include/asio
DEPENDPATH  += windows-3rdparts/include/libav
DEPENDPATH  += windows-3rdparts/include/SDL2
DEPENDPATH  += windows-3rdparts/include/ssh2
DEPENDPATH  += windows-3rdparts/include/tinyxml
DEPENDPATH  += windows-3rdparts/include/zip

# Using "QMAKE_CXXFLAGS += -isystem" to cast out Google ProtoBuf's multiple warnings.
# "-isystem" indicates directories containing system headers. System headers do not produce warnings.
QMAKE_CXXFLAGS += -isystem $$_PRO_FILE_PWD_/windows-3rdparts/include
QMAKE_CXXFLAGS += -DASIO_STANDALONE

MOC_DIR = moc_files
OBJECTS_DIR = obj_files

contains(QT_ARCH, x86_64) {
    LIBS += -L $$_PRO_FILE_PWD_/windows-3rdparts/x64/lib
    FILE_ZIP_DLL  = $$_PRO_FILE_PWD_/windows-3rdparts/x64/bin/libzip-2.dll
    FILE_ZLIB_DLL = $$_PRO_FILE_PWD_/windows-3rdparts/x64/bin/zlib1.dll
} else {
    LIBS += -L $$_PRO_FILE_PWD_/windows-3rdparts/x86/lib
    FILE_ZIP_DLL  = $$_PRO_FILE_PWD_/windows-3rdparts/x86/bin/libzip-2.dll
    FILE_ZLIB_DLL = $$_PRO_FILE_PWD_/windows-3rdparts/x86/bin/zlib1.dll
}

FILE_CONFIG_INI = $$_PRO_FILE_PWD_/config.ini

LIBS += -lws2_32
LIBS += -lBcrypt
LIBS += -lCrypt32
LIBS += -lMSwSock
LIBS += -pthread
LIBS += -lzip
LIBS += -lz

QMAKE_POST_LINK += windeployqt --no-translations --no-angle --no-system-d3d-compiler \$\(DESTDIR_TARGET\)$$escape_expand(\n\t)

QMAKE_POST_LINK += copy $$shell_path($${FILE_ZIP_DLL}) \$\(DESTDIR\)$$escape_expand(\n\t)
QMAKE_POST_LINK += copy $$shell_path($${FILE_ZLIB_DLL}) \$\(DESTDIR\)$$escape_expand(\n\t)
QMAKE_POST_LINK += copy $$shell_path($${FILE_CONFIG_INI}) \$\(DESTDIR\)$$escape_expand(\n\t)

CONFIG(debug, debug|release) {
    QMAKE_POST_LINK += del /f debug\qrc_sentinel-admin.cpp$$escape_expand(\n\t)
    QMAKE_POST_LINK += del /f debug\opengl32sw.dll$$escape_expand(\n\t)
}

CONFIG(release, debug|release) {
    QMAKE_POST_LINK += del /f release\qrc_sentinel-admin.cpp$$escape_expand(\n\t)
    QMAKE_POST_LINK += del /f release\opengl32sw.dll$$escape_expand(\n\t)
    QMAKE_POST_LINK += $$_PRO_FILE_PWD_\iscc.cmd $$_PRO_FILE_PWD_\setup.iss$$escape_expand(\n\t)
}
}

unix {
INCLUDEPATH += $$_PRO_FILE_PWD_/evigilante_configuration/ext/libzip/include/
LIBS += -L$$_PRO_FILE_PWD_/evigilante_configuration/ext/libzip/lib

CONFIG += link_pkgconfig
LIBS += -pthread -lrt -lzip -lz -Llib -L/usr/local/lib/
QMAKE_LFLAGS = -Wl,-rpath,/usr/local/lib/
}

SUBDIRS += \
    tests/automatic/tests.pro

# Copy user manuals to DESTDIR
# (need to add "make install" build step for this to work
CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug
user_manuals.path = $${DESTDIR}/
user_manuals.files = manual_*.pdf
INSTALLS += user_manuals
