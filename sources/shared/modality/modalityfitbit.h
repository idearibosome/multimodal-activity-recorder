#ifndef MODALITYFITBIT_H
#define MODALITYFITBIT_H

#if defined(MMR_MODALITY_FITBIT)

#include <QObject>

#include "modality.h"

#include "qhttp/qhttpabstracts.hpp"
#include "qhttp/qhttpserver.hpp"
#include "qhttp/qhttpserverconnection.hpp"
#include "qhttp/qhttpserverrequest.hpp"
#include "qhttp/qhttpserverresponse.hpp"

class ModalityFitbit : public Modality
{
    Q_OBJECT
public:
    explicit ModalityFitbit(QObject *parent = nullptr);

    bool isInitialized;
    bool isInAcquisition;

    bool initialize(QVariantMap configuration) override;
    void reset() override;

    bool startAcquisition() override;
    void stopAcquisition() override;

    QVariantList parseData(QByteArray data, EModalityParseType parseType) override;

    Q_INVOKABLE void startServer(int port);
    Q_INVOKABLE void stopServer();

    void processAcquiredData(QByteArray data);

private:
    qhttp::server::QHttpServer *httpServer;

    qint64 minimumTimestampDiff;
};

#endif

#endif // MODALITYFITBIT_H
