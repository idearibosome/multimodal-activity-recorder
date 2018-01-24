#ifndef MMRWSDATA_H
#define MMRWSDATA_H

#include <QDataStream>
#include <QObject>
#include <QVariant>

class MMRWSData : public QObject
{
    Q_OBJECT
public:
    explicit MMRWSData(QObject *parent = nullptr);

    QString requestType;
    QString dataType; // "request", "response"
    QVariantMap data;

    void loadFromByteArray(QByteArray byteArray);
    QByteArray toByteArray();

signals:

public slots:
};

#endif // MMRWSDATA_H
