#include "quickmain.h"

#include "mmrclient.h"

#include "../shared/mmrfilemetadata.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/modality/modality.h"
#include "../shared/modality/modalitykinect.h"
#include "../shared/modality/modalityqtsensor.h"
#include "../shared/modality/modalitybitalino.h"
#include "../shared/modality/modalityfitbit.h"

QuickMain *qMain;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QuickMain::QuickMain(QObject *parent) : QObject(parent) {
    lastConnectedServerUrl = "";
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
#if defined(MMR_MODALITY_BITALINO)
    {
        QVariantMap modality;
        modality.insert("type", "bitalino");
        modality.insert("text", "BITalino");
        modalities.append(modality);
    }
#endif
#if defined(MMR_MODALITY_FITBIT)
    {
        QVariantMap modality;
        modality.insert("type", "fitbit");
        modality.insert("text", "Fitbit");
        modalities.append(modality);
    }
#endif

    return modalities;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void QuickMain::loadMMRData(QString basePath) {
    unloadMMRData();

    fileMetadata = new MMRFileMetadata(this);
    fileMetadata->loadFromFileDirPath(basePath);

    IRQMSignalHandler::sendSignal("main", "mmrDataLoaded");
}
//---------------------------------------------------------------------------
void QuickMain::unloadMMRData() {
    if (fileMetadata) {
        fileMetadata->deleteLater();
        fileMetadata = NULL;
    }

    IRQMSignalHandler::sendSignal("main", "mmrDataUnloaded");
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
#if defined(MMR_MODALITY_BITALINO)
    if (type == "bitalino") {
        client->registerModality(new ModalityBITalino());
    }
#endif
#if defined(MMR_MODALITY_FITBIT)
    if (type == "fitbit") {
        client->registerModality(new ModalityFitbit());
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
    lastConnectedServerUrl = url;
}
//---------------------------------------------------------------------------
void QuickMain::clientDisconnectServer(QString identifier) {
    MMRClient *client = getClient(identifier);
    if (!client) return;

    client->disconnectServer();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QString QuickMain::getLastConnectedServerUrl() {
    return lastConnectedServerUrl;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
