#ifndef MODALITY_H
#define MODALITY_H

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QRegularExpression>
#include <QUuid>
#include <QVariant>

class Modality : public QObject
{
    Q_OBJECT
public:
    explicit Modality(QObject *parent = nullptr);

    QString type;
    QString identifier;
    QVariantMap configuration;

    qint64 startAcquisitionTimestamp;

    virtual bool initialize(QVariantMap configuration);
    virtual void reset();

    virtual bool startAcquisition() = 0;
    virtual void stopAcquisition() = 0;

    void startRecordingAcquisitionTimestamp();
    qint64 getAcquisitionTimestamp();

signals:
    void acquired(QByteArray data);

public slots:
};

#endif // MODALITY_H
