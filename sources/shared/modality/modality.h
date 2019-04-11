#ifndef MODALITY_H
#define MODALITY_H

#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <QObject>
#include <QRegularExpression>
#include <QUuid>
#include <QVariant>

class Modality : public QObject
{
    Q_OBJECT
public:
    explicit Modality(QObject *parent = nullptr);

    enum EModalityParseType {
        ParseType_Viewer = 0,
        ParseType_Recognizer = 1
    };

    QString type;
    QString identifier;
    QVariantMap configuration;

    qint64 startAcquisitionTimestamp;

    virtual bool initialize(QVariantMap configuration);
    virtual void reset();

    virtual bool startAcquisition() = 0;
    virtual void stopAcquisition() = 0;

    virtual QVariantList parseData(QByteArray data, EModalityParseType parseType = ParseType_Viewer) = 0;
    static QVariant parsedDataItemWithValue(QString name, QVariant value);
    static QVariant parsedDataItemWithImage(QString name, QImage image);

    qint64 getTimestamp();
    void startRecordingAcquisitionTimestamp();
    qint64 getAcquisitionTimestamp(qint64 timestamp=0);

signals:
    void acquired(qint64 timestamp, QByteArray data);

public slots:
};

#endif // MODALITY_H
