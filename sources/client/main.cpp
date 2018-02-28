#include "main.h"

#include "quickmain.h"

#include "../shared/irqm/irqmconfigurator.h"
#include "../shared/modality/modalityconfigurator.h"
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
    ModalityConfigurator::registerModules(&engine);


    // objects > quick main
    qMain = new QuickMain();
    engine.rootContext()->setContextProperty("qMain", qMain);


    // load main
    engine.load(QUrl(QStringLiteral("qrc:/qml/client/main.qml")));
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
