TEMPLATE = app


# plugins
QT += qml quick widgets network concurrent websockets

win32 {
} else:macx {
} else:android {
  QT += androidextras
} else:ios {
}


# basic config
CONFIG += c++11
CONFIG += resources_big

DEFINES += QT_DEPRECATED_WARNINGS

TARGET = MMRecorderServer

win32 {
  VERSION = 0.5.2
} else:macx {
  VERSION = 0.5.2

  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
  QMAKE_LFLAGS += -ObjC
} else:android {
  VERSION = 0.5.2
} else:ios {
  VERSION = 0.5.2

  CONFIG -= bitcode

  QMAKE_IOS_DEPLOYMENT_TARGET = 10.0
}


# shared
include(MultiModalRecorderShared.pri)


# defines, headers, sources, and resources
DEFINES += MMRECORDER_SERVER

RESOURCES += \
    $$PWD/qml/qml_server.qrc

HEADERS += \
    $$PWD/sources/server/main.h \
    $$PWD/sources/server/mmrserver.h \
    sources/server/mmrmodalityconnection.h \
    sources/server/mmrrecognizerconnection.h

SOURCES += \
    $$PWD/sources/server/main.cpp \
    $$PWD/sources/server/mmrserver.cpp \
    sources/server/mmrmodalityconnection.cpp \
    sources/server/mmrrecognizerconnection.cpp

QML_IMPORT_PATH =

QML_DESIGNER_IMPORT_PATH =


# external sources
win32 {
} else:macx {
    #OBJECTIVE_SOURCES +=
} else:android {
} else:ios {
}


# modalities
MMRModalities += kinect
MMRModalities += qtsensor
MMRModalities += bitalino
MMRModalities += fitbit
include(modalities.pri)


# default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

