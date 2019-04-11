#ifndef MMRRECOGNIZERCONNECTION_H
#define MMRRECOGNIZERCONNECTION_H

#include <QObject>
#include <QWebSocket>

class MMRWSData;

class MMRRecognizerConnection : public QObject
{
    Q_OBJECT
public:
    explicit MMRRecognizerConnection(QObject *parent = nullptr);

    MMRServer *server = nullptr;

    QWebSocket *ws = NULL;

    QString name;
    QString identifier;
    QStringList registeredModalityList;

    void log(QString text);

    void setWebSocket(QWebSocket *ws);

    void prepare();
    void start();
    void stop();
    void finalize();
    void close();

    void handleRequest(MMRWSData *wsData);
    void handleRequestRegister(QString type, QVariantMap data);

    void sendRequest(MMRWSData *wsData);

signals:
    void sendBinaryMessage(QWebSocket *ws, QByteArray message);
    void closeWebSocket(QWebSocket *ws);
    void disconnected();

public slots:
    void slotWsBinaryMessageReceived(QByteArray message);
    void slotWsDisconnected();
    void slotPrepare();
    void slotStart();
    void slotStop();
    void slotFinalize();
};

#endif // MMRRECOGNIZERCONNECTION_H
