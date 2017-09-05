#include "irqmconfigurator.h"

#define IRQM_CONFIGURATOR_SINGLETON_PROVIDER(className) \
    static QObject *singletonForIRQM##className(QQmlEngine *engine, QJSEngine *scriptEngine) { \
        Q_UNUSED(engine) \
        Q_UNUSED(scriptEngine) \
        return new IRQM##className(); \
    }

#define IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(className) \
    qmlRegisterSingletonType<IRQM##className>( \
        (QString("IRQM.") + #className).toUtf8().constData(), \
        1, 0, \
        (QString("IRQM") + #className).toUtf8().constData(), \
        singletonForIRQM##className);

#include "irqmjsonhelper.h"
#include "irqmpathhelper.h"
#include "irqmrandomhelper.h"

#include "irqmsignalhandler.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQM_CONFIGURATOR_SINGLETON_PROVIDER(JSONHelper)
IRQM_CONFIGURATOR_SINGLETON_PROVIDER(PathHelper)
IRQM_CONFIGURATOR_SINGLETON_PROVIDER(RandomHelper)

IRQM_CONFIGURATOR_SINGLETON_PROVIDER(SignalHandler)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQMConfigurator::IRQMConfigurator(QObject *parent) : QObject(parent) {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IRQMConfigurator::registerAllParams(QQmlApplicationEngine *engine) {
    registerModules(engine);
    registerProperties(engine);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IRQMConfigurator::registerModules(QQmlApplicationEngine *engine) {
    // singletons
    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(JSONHelper);
    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(PathHelper);
    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(RandomHelper);

    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(SignalHandler);
}
//------------------------------------------------------------------------------
void IRQMConfigurator::registerProperties(QQmlApplicationEngine *engine) {
    // Scaling ratio
    qreal scale;
#if defined(Q_OS_WIN)
    scale = qApp->primaryScreen()->logicalDotsPerInch() / 96;
#else
    scale = qApp->primaryScreen()->logicalDotsPerInch() / 72;
#endif
    engine->rootContext()->setContextProperty("IRQMScale", scale);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
