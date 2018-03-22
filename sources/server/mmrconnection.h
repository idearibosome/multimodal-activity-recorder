#ifndef MMRCONNECTION_H
#define MMRCONNECTION_H

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QWebSocket>

class MMRWSData;
class MMRFileData;

class MMRConnection : public QObject
{
    Q_OBJECT
public:
    explicit MMRConnection(QObject *parent = nullptr);

    QWebSocket *ws = NULL;

    MMRFileData *fileData = NULL;

    QString storageBasePath;

    QString type;
    QString identifier;

    void log(QString text);

    void handleRequest(MMRWSData *wsData);
    void handleRequestRegister(QString type, QVariantMap data);
    void handleRequestData(QString type, QVariantMap data);

private:
    void prepareFile();
    void closeFile();

signals:

public slots:
};

#endif // MMRCONNECTION_H
