QT += qml quick widgets network concurrent websockets

RESOURCES += \
    $$PWD/qml/qml_shared.qrc

HEADERS += \
    $$PWD/sources/shared/irqm/irqmfilehelper.h \
    $$PWD/sources/shared/irqm/irqmjsonhelper.h \
    $$PWD/sources/shared/irqm/irqmpathhelper.h \
    $$PWD/sources/shared/irqm/irqmrandomhelper.h \
    $$PWD/sources/shared/irqm/irqmsignalhandler.h \
    $$PWD/sources/shared/irqm/irqmconfigurator.h \
    $$PWD/sources/shared/irqm/irqmsqlitehelper.h \
    $$PWD/sources/shared/mmrwsdata.h \
    $$PWD/sources/shared/mmrfiledata.h \
    $$PWD/sources/shared/mmrfilemetadata.h

SOURCES += \
    $$PWD/sources/shared/irqm/irqmfilehelper.cpp \
    $$PWD/sources/shared/irqm/irqmjsonhelper.cpp \
    $$PWD/sources/shared/irqm/irqmpathhelper.cpp \
    $$PWD/sources/shared/irqm/irqmrandomhelper.cpp \
    $$PWD/sources/shared/irqm/irqmsignalhandler.cpp \
    $$PWD/sources/shared/irqm/irqmconfigurator.cpp \
    $$PWD/sources/shared/irqm/irqmsqlitehelper.cpp \
    $$PWD/sources/shared/mmrwsdata.cpp \
    $$PWD/sources/shared/mmrfiledata.cpp \
    $$PWD/sources/shared/mmrfilemetadata.cpp

INCLUDEPATH += $$PWD/sources
INCLUDEPATH += $$PWD/include/common

# platform specific
win32:contains(QMAKE_HOST.arch, x86_64) {
  LIBS += -L$$PWD/lib/win/x64
} else:win32 {
  LIBS += -L$$PWD/lib/win/x86
} else:macx {
} else:android {
  equals(ANDROID_TARGET_ARCH, armeabi-v7a) {
    LIBS += -L$$PWD/lib/android/armeabi-v7a
    ANDROID_EXTRA_LIBS += \
      $$PWD/lib/android/armeabi-v7a/libsqlite3.so \
      $$PWD/lib/android/armeabi-v7a/libqhttp.so
  }
  equals(ANDROID_TARGET_ARCH, x86) {
    LIBS += -L$$PWD/lib/android/x86
    ANDROID_EXTRA_LIBS += \
      $$PWD/lib/android/x86/libsqlite3.so \
      $$PWD/lib/android/x86/libqhttp.so
  }
} else:ios {
}

# common libraries
LIBS += -lsqlite3
LIBS += -lqhttp


