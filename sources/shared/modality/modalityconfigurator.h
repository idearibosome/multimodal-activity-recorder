#ifndef MODALITYCONFIGURATOR_H
#define MODALITYCONFIGURATOR_H

#include <QObject>
#include <QQmlApplicationEngine>

class ModalityConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit ModalityConfigurator(QObject *parent = nullptr);

    static void registerModules(QQmlApplicationEngine *engine);

signals:

public slots:
};

#endif // MODALITYCONFIGURATOR_H
