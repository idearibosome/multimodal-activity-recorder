# general
HEADERS += $$PWD/sources/shared/modality/modality.h \
    $$PWD/sources/shared/modality/modalityconfigurator.h
SOURCES += $$PWD/sources/shared/modality/modality.cpp \
    $$PWD/sources/shared/modality/modalityconfigurator.cpp


# kinect
contains(MMRModalities, "kinect") {
    HEADERS += $$PWD/sources/shared/modality/modalitykinect.h
    SOURCES += $$PWD/sources/shared/modality/modalitykinect.cpp

    win32 {
        DEFINES += MMR_MODALITY_KINECT
        INCLUDEPATH += "$$(KINECTSDK20_DIR)\\inc"
    }

    win32:contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L"$$(KINECTSDK20_DIR)\\Lib\\x64" -lKinect20
    } else:win32 {
        LIBS += -L"$$(KINECTSDK20_DIR)\\Lib\\x86" -lKinect20
    }
}


# Qt sensor
contains(MMRModalities, "qtsensor") {
    QT += sensors

    HEADERS += $$PWD/sources/shared/modality/modalityqtsensor.h
    SOURCES += $$PWD/sources/shared/modality/modalityqtsensor.cpp

    DEFINES += MMR_MODALITY_QTSENSOR
}
