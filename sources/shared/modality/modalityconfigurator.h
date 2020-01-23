#ifndef MODALITYCONFIGURATOR_H
#define MODALITYCONFIGURATOR_H

#include <QObject>
#include <QQmlApplicationEngine>

class Modality;
class ModalityParser;

class ModalityConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit ModalityConfigurator(QObject *parent = nullptr);

    static void registerModules(QQmlApplicationEngine *engine);
    static Modality *modalityForType(QString type, QObject *parent = nullptr);
    static ModalityParser *modalityParserForType(QString type, QObject *parent = nullptr);

signals:

public slots:
};

#endif // MODALITYCONFIGURATOR_H
