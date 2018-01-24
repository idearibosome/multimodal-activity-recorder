# general
HEADERS += $$PWD/sources/client/modality/modality.h
SOURCES += $$PWD/sources/client/modality/modality.cpp


# kinect
contains(MMRModalities, "kinect") {
    HEADERS += $$PWD/sources/client/modality/modalitykinect.h
    SOURCES += $$PWD/sources/client/modality/modalitykinect.cpp

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

    HEADERS += $$PWD/sources/client/modality/modalityqtsensor.h
    SOURCES += $$PWD/sources/client/modality/modalityqtsensor.cpp

    DEFINES += MMR_MODALITY_QTSENSOR
}
