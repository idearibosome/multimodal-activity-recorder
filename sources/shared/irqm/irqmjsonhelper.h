#ifndef IRQMJSONHELPER_H
#define IRQMJSONHELPER_H

#include <QFile>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QVariant>

class IRQMJSONHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRQMJSONHelper(QObject *parent = nullptr);

    Q_INVOKABLE static QVariant fileToJSONObject(QString path);
    Q_INVOKABLE static QVariant ioDeviceToJSONObject(QIODevice *ioDevice);
    Q_INVOKABLE static QVariant stringToJSONObject(QString jsonString);
    Q_INVOKABLE static QVariant byteArrayToJSONObject(QByteArray jsonByteArray);

    Q_INVOKABLE static void saveJSONObjectToFile(QVariant jsonObject, QString path);
    Q_INVOKABLE static QString JSONObjectToString(QVariant jsonObject);
    Q_INVOKABLE static QByteArray JSONObjectToByteArray(QVariant jsonObject);

signals:

public slots:
};

#endif // IRQMJSONHELPER_H
