#ifndef MMRCLIENT_H
#define MMRCLIENT_H

#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QRegularExpression>
#include <QUuid>
#include <QVariant>
#include <QWebSocket>

class MMRWSData;
class Modality;

class MMRClient : public QObject
{
    Q_OBJECT
public:
    explicit MMRClient(QObject *parent = nullptr);

    QString identifier;
    Modality *modality = NULL;
    QVariantMap configuration;

    QWebSocket *ws = NULL;

    QMutex modalityDataMutex;
    bool hadReceiveModalityData = false;
    bool isWsReadyToReceiveModalityData = false;
    bool hasPendingModalityData = false;
    QVariantList pendingModalityDataList;

    void log(QString text);

    void setConfiguration(QString key, QVariant value);

    void registerModality(Modality *modality);

    void connectServer(QString url);
    void disconnectServer();

    Q_INVOKABLE void requestRegister();

    void handleRequest(MMRWSData *wsData);
    void handleRequestPrepare(QString type, QVariantMap data);
    void handleRequestStart(QString type, QVariantMap data);
    void handleRequestStop(QString type, QVariantMap data);
    void handleRequestFinalize(QString type, QVariantMap data);

    void handleResponse(MMRWSData *wsData);
    void handleResponseRegister(QString type, QVariantMap data);
    void handleResponseData(QString type, QVariantMap data);
    void handleResponseDataList(QString type, QVariantMap data);

private:
    void sendPendingModalityData();

signals:

private slots:
    void slotWsConnected();
    void slotWsDisconnected();
    void slotWsError(QAbstractSocket::SocketError error);
    void slotWsBinaryMessageReceived(QByteArray message);

    void slotModalityAcquired(qint64 timestamp, QByteArray data);

};

#endif // MMRCLIENT_H
