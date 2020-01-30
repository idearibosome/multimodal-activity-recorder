#ifndef QUICKMAIN_H
#define QUICKMAIN_H

#include <QObject>
#include <QVariant>

class MMRClient;
class MMRFileMetadata;
class Modality;

class QuickMain : public QObject
{
    Q_OBJECT
public:
    explicit QuickMain(QObject *parent = nullptr);

    QMap<QString, MMRClient *> clientList;

    MMRFileMetadata *fileMetadata = NULL;

    QString lastConnectedServerUrl;

    Q_INVOKABLE QString getAppVersionString() { return APP_VERSION_STRING; }

    Q_INVOKABLE QVariantList getAvailableModalities();

    Q_INVOKABLE void loadMMRData(QString basePath);
    Q_INVOKABLE void unloadMMRData();
    Q_INVOKABLE QVariantList getMMRModalities();

    Q_INVOKABLE QString createClient(QVariantMap modality);
    Q_INVOKABLE QString createMMRDataClient(QVariantMap modalityInfo);
    Q_INVOKABLE void destroyClient(QString identifier);
    MMRClient *getClient(QString identifier);
    Q_INVOKABLE Modality *getClientModality(QString identifier);

    Q_INVOKABLE void clientSetConfiguration(QString identifier, QString key, QVariant value);
    Q_INVOKABLE void clientConnectServer(QString identifier, QString url);
    Q_INVOKABLE void clientDisconnectServer(QString identifier);

    Q_INVOKABLE QString getLastConnectedServerUrl();

signals:

public slots:
};

extern QuickMain *qMain;

#endif // QUICKMAIN_H
