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
    $$PWD/sources/shared/mmrwsdata.h \
    $$PWD/sources/shared/mmrfiledata.h

SOURCES += \
    $$PWD/sources/shared/irqm/irqmfilehelper.cpp \
    $$PWD/sources/shared/irqm/irqmjsonhelper.cpp \
    $$PWD/sources/shared/irqm/irqmpathhelper.cpp \
    $$PWD/sources/shared/irqm/irqmrandomhelper.cpp \
    $$PWD/sources/shared/irqm/irqmsignalhandler.cpp \
    $$PWD/sources/shared/irqm/irqmconfigurator.cpp \
    $$PWD/sources/shared/mmrwsdata.cpp \
    $$PWD/sources/shared/mmrfiledata.cpp

INCLUDEPATH += $$PWD/sources
