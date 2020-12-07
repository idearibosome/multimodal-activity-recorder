#include "irqmconfigurator.h"

#define IRQM_CONFIGURATOR_SINGLETON_PROVIDER(className) \
    static QObject *singletonForIRQM##className(QQmlEngine *engine, QJSEngine *scriptEngine) { \
        Q_UNUSED(engine) \
        Q_UNUSED(scriptEngine) \
        return new IRQM##className(); \
    }

#define IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(className) \
    qmlRegisterSingletonType<IRQM##className>( \
        "IRQtModules", \
        1, 0, \
        #className, \
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
    Q_UNUSED(engine);

    // singletons
    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(JSONHelper);
    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(PathHelper);
    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(RandomHelper);

    IRQM_CONFIGURATOR_QML_REGISTER_SINGLETON(SignalHandler);
}
//------------------------------------------------------------------------------
void IRQMConfigurator::registerProperties(QQmlApplicationEngine *engine) {
    Q_UNUSED(engine);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IRQMConfigurator::configureRecommendedSettings() {
    // Enable automatic high DPI scaling
#if defined(Q_OS_WIN) || defined(Q_OS_OSX)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
