#ifndef IRQMSIGNALHANDLER_H
#define IRQMSIGNALHANDLER_H

#include <QObject>
#include <QVariant>

class IRQMSignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit IRQMSignalHandler(QObject *parent = 0);

    static QMap< QString, QMap< QString, QList< QPair< QObject *, QString > > * > * > bindingMap;

    Q_INVOKABLE static void bindSignal(QString signalObjectName, QString signalName, QObject *slotObject, QString slotName);
    Q_INVOKABLE static void unbindSignal(QString signalObjectName, QString signalName, QObject *slotObject, QString slotName);
    Q_INVOKABLE static void unbindAllSignalsForSlot(QObject *slotObject);

    static QList< QPair< QObject *, QString > > *getSignalSlots(QString signalObjectName, QString signalName);

    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName);
    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName, QVariant arg1);
    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2);
    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3);
    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4);
    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5);
    Q_INVOKABLE static void sendSignal(QString signalObjectName, QString signalName, QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6);

signals:

public slots:
};

#endif // IRQMSIGNALHANDLER_H
