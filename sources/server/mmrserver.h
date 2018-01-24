#ifndef MMRSERVER_H
#define MMRSERVER_H

#include <QMap>
#include <QObject>
#include <QVariant>
#include <QWebSocket>
#include <QWebSocketServer>

class MMRWSData;
class MMRConnection;

class MMRServer : public QObject
{
    Q_OBJECT
public:
    explicit MMRServer(QObject *parent = nullptr);

    void log(QString text);

    Q_INVOKABLE void startServer(int port);
    Q_INVOKABLE void stopServer();

    Q_INVOKABLE void requestPrepareModalities();
    Q_INVOKABLE void requestStartModalities();
    Q_INVOKABLE void requestStopModalities();

    void sendRequest(MMRWSData *wsData);

private:
    QWebSocketServer *wsServer = 0;
    QMap<QWebSocket *, MMRConnection *> wsMap;

    void initializeWsServer();

private slots:
    void slotWsServerNewConnection();

    void slotWsBinaryMessageReceived(QByteArray message);
    void slotWsDisconnected();

};

#endif // MMRSERVER_H
