TEMPLATE = app


# plugins
QT += qml quick widgets concurrent websockets

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

TARGET = MMRecorderViewer

win32 {
  VERSION = 0.1.0
} else:macx {
  VERSION = 0.1.0

  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
  QMAKE_LFLAGS += -ObjC
} else:android {
  VERSION = 0.1.0
} else:ios {
  VERSION = 0.1.0

  CONFIG -= bitcode

  QMAKE_IOS_DEPLOYMENT_TARGET = 8.0
}


# shared
include(MultiModalRecorderShared.pri)


# defines, headers, sources, and resources
DEFINES += MMRECORDER_VIEWER

RESOURCES += \
    $$PWD/qml/qml_viewer.qrc

HEADERS += \
    $$PWD/sources/viewer/main.h \
    $$PWD/sources/viewer/quickmain.h \
    $$PWD/sources/viewer/mmrobject.h

SOURCES += \
    $$PWD/sources/viewer/main.cpp \
    $$PWD/sources/viewer/quickmain.cpp \
    $$PWD/sources/viewer/mmrobject.cpp

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

