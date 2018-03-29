#ifndef MMRSERVER_H
#define MMRSERVER_H

#include <QMap>
#include <QObject>
#include <QVariant>
#include <QWebSocket>
#include <QWebSocketServer>

class MMRWSData;
class MMRFileMetadata;
class MMRConnection;

class MMRServer : public QObject
{
    Q_OBJECT
public:
    explicit MMRServer(QObject *parent = nullptr);

    void log(QString text);

    Q_INVOKABLE void startServer(int port);
    Q_INVOKABLE void stopServer();

    Q_INVOKABLE void setStorageBasePath(QString path);

    Q_INVOKABLE void prepareModalities();
    Q_INVOKABLE void startModalityAcquisition();
    Q_INVOKABLE void stopModalityAcquisition();
    Q_INVOKABLE void finalizeModalities();

    void sendRequest(MMRWSData *wsData);

private:
    QWebSocketServer *wsServer = 0;
    QList<MMRConnection *> connections;

    MMRFileMetadata *fileMetadata = NULL;

    QString storageBasePath;

    void initializeWsServer();

    void prepareFileMetadata();
    void finalizeFileMetadata();

private slots:
    void slotWsServerNewConnection();
    void slotConnectionDisconnected();

};

#endif // MMRSERVER_H
