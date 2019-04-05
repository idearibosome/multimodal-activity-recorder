#ifndef MMRSERVER_H
#define MMRSERVER_H

#include <QMap>
#include <QObject>
#include <QThread>
#include <QVariant>
#include <QWebSocket>
#include <QWebSocketServer>

class MMRWSData;
class MMRFileMetadata;
class MMRModalityConnection;
class MMRRecognizerConnection;

class MMRServer : public QObject
{
    Q_OBJECT
public:
    explicit MMRServer(QObject *parent = nullptr);

    void log(QString text);

    Q_INVOKABLE void startServer(int modalityPort, int recognizerPort);
    Q_INVOKABLE void stopServer();

    Q_INVOKABLE void setStorageBasePath(QString path);

    Q_INVOKABLE void prepareModalities();
    Q_INVOKABLE void startModalityAcquisition();
    Q_INVOKABLE void stopModalityAcquisition();
    Q_INVOKABLE void finalizeModalities();

    void sendModalityRequest(MMRWSData *wsData);

private:
    QWebSocketServer *wsModalityServer = nullptr;
    QWebSocketServer *wsRecognizerServer = nullptr;
    QList<MMRModalityConnection *> modalityConnections;
    QList<MMRRecognizerConnection *> recognizerConnections;

    MMRFileMetadata *fileMetadata = NULL;

    QString storageBasePath;

    void initializeWsModalityServer();
    void initializeWsRecognizerServer();

    void prepareFileMetadata();
    void finalizeFileMetadata();

signals:
    void modalityPrepare(MMRFileMetadata *fileMetadata);
    void modalityStart();
    void modalityStop();
    void modalityFinalize();
    void recognizerPrepare();
    void recognizerStart();
    void recognizerStop();
    void recognizerFinalize();

private slots:
    void slotWsModalityServerNewConnection();
    void slotWsRecognizerServerNewConnection();
    void slotModalityConnectionDisconnected();
    void slotRecognizerConnectionDisconnected();

    void slotModalityConnectionSendBinaryMessage(QWebSocket *ws, QByteArray message);
    void slotModalityConnectionCloseWebSocket(QWebSocket *ws);
    void slotRecognizerConnectionSendBinaryMessage(QWebSocket *ws, QByteArray message);
    void slotRecognizerConnectionCloseWebSocket(QWebSocket *ws);

};

#endif // MMRSERVER_H
