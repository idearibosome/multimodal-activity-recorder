#ifndef MMRCONNECTION_H
#define MMRCONNECTION_H

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QWebSocket>

class MMRWSData;
class MMRFileMetadata;
class MMRFileData;

class MMRConnection : public QObject
{
    Q_OBJECT
public:
    explicit MMRConnection(QObject *parent = nullptr);

    QWebSocket *ws = NULL;

    MMRFileMetadata *fileMetadata = NULL;
    MMRFileData *fileData = NULL;

    QString storageBasePath;

    QString type;
    QString identifier;

    void log(QString text);

    void setWebSocket(QWebSocket *ws);
    void setFileMetadata(MMRFileMetadata *fileMetadata);

    void prepare();
    void start();
    void stop();
    void finalize();
    void close();

    void handleRequest(MMRWSData *wsData);
    void handleRequestRegister(QString type, QVariantMap data);
    void handleRequestData(QString type, QVariantMap data);
    void handleRequestDataList(QString type, QVariantMap data);

    void sendRequest(MMRWSData *wsData);

private:
    void prepareFile();
    void closeFile();

signals:
    void sendBinaryMessage(QWebSocket *ws, QByteArray message);
    void closeWebSocket(QWebSocket *ws);
    void disconnected();

public slots:
    void slotWsBinaryMessageReceived(QByteArray message);
    void slotWsDisconnected();
    void slotPrepare(MMRFileMetadata *fileMetadata);
    void slotStart();
    void slotStop();
    void slotFinalize();

};

#endif // MMRCONNECTION_H
