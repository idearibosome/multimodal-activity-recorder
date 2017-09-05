#include "main.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/server/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
