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

TARGET = MMRecorderClient

win32 {
  VERSION = 0.6.0
} else:macx {
  VERSION = 0.6.0

  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
  QMAKE_LFLAGS += -ObjC
} else:android {
  VERSION = 0.6.0
} else:ios {
  VERSION = 0.6.0

  CONFIG -= bitcode

  QMAKE_IOS_DEPLOYMENT_TARGET = 10.0
}
DEFINES += APP_VERSION=$$VERSION
DEFINES += APP_VERSION_STRING='\\\"$$VERSION\\\"'


# shared
include(MultiModalRecorderShared.pri)


# defines, headers, sources, and resources
DEFINES += MMRECORDER_CLIENT

RESOURCES += \
    $$PWD/qml/qml_client.qrc

HEADERS += \
    $$PWD/sources/client/main.h \
    $$PWD/sources/client/mmrclient.h \
    $$PWD/sources/client/quickmain.h

SOURCES += \
    $$PWD/sources/client/main.cpp \
    $$PWD/sources/client/mmrclient.cpp \
    $$PWD/sources/client/quickmain.cpp

QML_IMPORT_PATH =

QML_DESIGNER_IMPORT_PATH =


# external sources
win32 {
} else:macx {
    #OBJECTIVE_SOURCES +=
} else:android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/sources/client/android
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

