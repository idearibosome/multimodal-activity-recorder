#include "quickmain.h"

#include "mmrclient.h"

#include "../shared/modality/modality.h"
#include "../shared/modality/modalitykinect.h"
#include "../shared/modality/modalityqtsensor.h"

QuickMain *qMain;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QuickMain::QuickMain(QObject *parent) : QObject(parent) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList QuickMain::getAvailableModalities() {
    QVariantList modalities;

#if defined(MMR_MODALITY_KINECT)
    {
        QVariantMap modality;
        modality.insert("type", "kinect");
        modality.insert("text", "Kinect");
        modalities.append(modality);
    }
#endif
#if defined(MMR_MODALITY_QTSENSOR)
    {
        QVariantMap modality;
        modality.insert("type", "qtsensor");
        modality.insert("text", "Internal sensor (Qt Sensors)");
        modalities.append(modality);
    }
#endif

    return modalities;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QString QuickMain::createClient(QVariantMap modality) {
    QString type = modality.value("type").toString();

    MMRClient *client = new MMRClient(this);

#if defined(MMR_MODALITY_KINECT)
    if (type == "kinect") {
        client->registerModality(new ModalityKinect());
    }
#endif
#if defined(MMR_MODALITY_QTSENSOR)
    if (type == "qtsensor") {
        client->registerModality(new ModalityQtSensor());
    }
#endif

    clientList.insert(client->identifier, client);

    return client->identifier;
}
//---------------------------------------------------------------------------
void QuickMain::destroyClient(QString identifier) {
    MMRClient *client = getClient(identifier);
    if (!client) return;

    client->deleteLater();
    clientList.remove(identifier);
}
//---------------------------------------------------------------------------
MMRClient *QuickMain::getClient(QString identifier) {
    MMRClient *client = clientList.value(identifier, NULL);

    return client;
}
//---------------------------------------------------------------------------
Modality *QuickMain::getClientModality(QString identifier) {
    MMRClient *client = getClient(identifier);
    if (!client) return 0;

    return client->modality;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void QuickMain::clientSetConfiguration(QString identifier, QString key, QVariant value) {
    MMRClient *client = getClient(identifier);
    if (!client) return;

    client->setConfiguration(key, value);
}
//---------------------------------------------------------------------------
void QuickMain::clientConnectServer(QString identifier, QString url) {
    MMRClient *client = getClient(identifier);
    if (!client) return;

    client->connectServer(url);
}
//---------------------------------------------------------------------------
void QuickMain::clientDisconnectServer(QString identifier) {
    MMRClient *client = getClient(identifier);
    if (!client) return;

    client->disconnectServer();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
