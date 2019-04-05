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
    QList<Modality *> modalities;
    QVariantMap configuration;

    QWebSocket *ws = nullptr;

    void log(QString text);

    void connectServer(QString url);
    void disconnectServer();

    Q_INVOKABLE void requestRegister();

    void handleRequest(MMRWSData *wsData);
    void handleRequestPrepare(QString type, QVariantMap data);
    void handleRequestStart(QString type, QVariantMap data);
    void handleRequestRecognize(QString type, QVariantMap data);
    void handleRequestStop(QString type, QVariantMap data);
    void handleRequestFinalize(QString type, QVariantMap data);

    void handleResponse(MMRWSData *wsData);
    void handleResponseRegister(QString type, QVariantMap data);
    void handleResponseResult(QString type, QVariantMap data);

signals:

private slots:
    void slotWsConnected();
    void slotWsDisconnected();
    void slotWsError(QAbstractSocket::SocketError error);
    void slotWsBinaryMessageReceived(QByteArray message);

};

#endif // MMRCLIENT_H
