#include "main.h"

#include "mmrserver.h"

#include "../shared/irqm/irqmconfigurator.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
#if defined(Q_OS_WIN)
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
#endif

    IRQMConfigurator::configureRecommendedSettings();

    // app and engine
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;


    // engine contexts
    IRQMConfigurator::registerAllParams(&engine);


    // server
    MMRServer *server = new MMRServer();
    engine.rootContext()->setContextProperty("mServer", server);


    // load main
    engine.load(QUrl(QStringLiteral("qrc:/qml/server/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }


    // run
    int res = app.exec();


    // finalize
    return res;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
