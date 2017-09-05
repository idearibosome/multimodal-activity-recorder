#include "irqmsignalhandler.h"

QMap< QString, QMap< QString, QList< QPair< QObject *, QString > > * > * > IRQMSignalHandler::bindingMap;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
IRQMSignalHandler::IRQMSignalHandler(QObject *parent) : QObject(parent) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IRQMSignalHandler::bindSignal(QString signalObjectName, QString signalName, QObject *slotObject, QString slotName) {
    if (!bindingMap.count(signalObjectName)) {
        bindingMap.insert(signalObjectName, new QMap< QString, QList< QPair< QObject *, QString > > * >());
    }
    QMap< QString, QList< QPair< QObject *, QString > > * > *signalObjectMap = bindingMap.value(signalObjectName);

    if (!signalObjectMap->count(signalName)) {
        signalObjectMap->insert(signalName, new QList< QPair< QObject *, QString > >());
    }
    QList< QPair< QObject *, QString > > *signalSlotList = signalObjectMap->value(signalName);

    signalSlotList->append(QPair< QObject *, QString >(slotObject, slotName));
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::unbindSignal(QString signalObjectName, QString signalName, QObject *slotObject, QString slotName) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    signalSlotList->removeAll(QPair< QObject *, QString >(slotObject, slotName));
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::unbindAllSignalsForSlot(QObject *slotObject) {
    QList< QString > signalObjectNameList = bindingMap.keys();

    foreach (QString signalObjectName, signalObjectNameList) {
        QMap< QString, QList< QPair< QObject *, QString > > * > *signalObjectMap = bindingMap.value(signalObjectName);

        QList< QString > signalNameList = signalObjectMap->keys();

        foreach (QString signalName, signalNameList) {
            QList< QPair< QObject *, QString > > *signalSlotList = signalObjectMap->value(signalName);

            for (int i=0; i<signalSlotList->count(); i++) {
                QPair< QObject *, QString > signalSlot = signalSlotList->at(i);
                if (signalSlot.first == slotObject) {
                    signalSlotList->removeAt(i);
                    i -= 1;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QList< QPair< QObject *, QString > > *IRQMSignalHandler::getSignalSlots(QString signalObjectName, QString signalName) {
    if (!bindingMap.count(signalObjectName)) return NULL;
    QMap< QString, QList< QPair< QObject *, QString > > * > *signalObjectMap = bindingMap.value(signalObjectName);

    if (!signalObjectMap->count(signalName)) return NULL;

    return signalObjectMap->value(signalName);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument());
    }
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName, QVariant arg1) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument(),
            Q_ARG(QVariant, QVariant(arg1)));
    }
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument(),
            Q_ARG(QVariant, QVariant(arg1)), Q_ARG(QVariant, QVariant(arg2)));
    }
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument(),
            Q_ARG(QVariant, QVariant(arg1)), Q_ARG(QVariant, QVariant(arg2)), Q_ARG(QVariant, QVariant(arg3)));
    }
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument(),
            Q_ARG(QVariant, QVariant(arg1)), Q_ARG(QVariant, QVariant(arg2)), Q_ARG(QVariant, QVariant(arg3)),
            Q_ARG(QVariant, QVariant(arg4)));
    }
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument(),
            Q_ARG(QVariant, QVariant(arg1)), Q_ARG(QVariant, QVariant(arg2)), Q_ARG(QVariant, QVariant(arg3)),
            Q_ARG(QVariant, QVariant(arg4)), Q_ARG(QVariant, QVariant(arg5)));
    }
}
//---------------------------------------------------------------------------
void IRQMSignalHandler::sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6) {
    QList< QPair< QObject *, QString > > *signalSlotList = getSignalSlots(signalObjectName, signalName);
    if (!signalSlotList) return;

    QListIterator< QPair< QObject *, QString > > i(*signalSlotList);
    while (i.hasNext()) {
        QPair< QObject *, QString > signalSlot = i.next();
        if (!signalSlot.first) continue;
        QMetaObject::invokeMethod(signalSlot.first, signalSlot.second.toStdString().c_str(),
            QGenericReturnArgument(),
            Q_ARG(QVariant, QVariant(arg1)), Q_ARG(QVariant, QVariant(arg2)), Q_ARG(QVariant, QVariant(arg3)),
            Q_ARG(QVariant, QVariant(arg4)), Q_ARG(QVariant, QVariant(arg5)), Q_ARG(QVariant, QVariant(arg6)));
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
