#include "main.h"

#include "../shared/irqm/irqmconfigurator.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    // app and engine
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;


    // engine contexts
    IRQMConfigurator::registerAllParams(&engine);


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
