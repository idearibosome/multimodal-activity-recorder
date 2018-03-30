#include "main.h"

#include "../shared/irqm/irqmconfigurator.h"
#include "../shared/modality/modalityconfigurator.h"

#include "../shared/mmrfilemetadata.h"

#include "mmrobject.h"

#include "quickmain.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    IRQMConfigurator::configureRecommendedSettings();

    // app and engine
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;


    // engine contexts
    IRQMConfigurator::registerAllParams(&engine);
    ModalityConfigurator::registerModules(&engine);

    qmlRegisterType<MMRFileMetadata>();
    qmlRegisterType<MMRObject>();


    // objects > quick main
    qMain = new QuickMain();
    engine.rootContext()->setContextProperty("qMain", qMain);
    engine.addImageProvider("object", qMain->objectImageProvider);


    // load main
    engine.load(QUrl(QStringLiteral("qrc:/qml/viewer/main.qml")));
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
