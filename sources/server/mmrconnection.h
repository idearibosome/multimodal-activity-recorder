#ifndef MMRCONNECTION_H
#define MMRCONNECTION_H

#include <QObject>
#include <QWebSocket>

class MMRWSData;

class MMRConnection : public QObject
{
    Q_OBJECT
public:
    explicit MMRConnection(QObject *parent = nullptr);

    QWebSocket *ws = NULL;

    QString type;
    QString identifier;

    void log(QString text);

    void handleRequest(MMRWSData *wsData);
    void handleRequestRegister(QString type, QVariantMap data);
    void handleRequestData(QString type, QVariantMap data);

signals:

public slots:
};

#endif // MMRCONNECTION_H
