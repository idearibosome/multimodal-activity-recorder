#ifndef MMROBJECT_H
#define MMROBJECT_H

#include <QDir>
#include <QImage>
#include <QMap>
#include <QObject>
#include <QVariant>

class ModalityParser;
class MMRFileData;

class MMRObject : public QObject
{
    Q_OBJECT
public:
    explicit MMRObject(QObject *parent = nullptr);

    QString identifier;
    ModalityParser *modalityParser = NULL;
    MMRFileData *fileData = NULL;

    qint64 loadedDataPos = -1;
    QVariantList loadedData;
    QMap<QString, QImage> loadedImageData;

    void clear();

    void setModalityInfo(QVariantMap info);
    void loadFileData(QString basePath);

    Q_INVOKABLE QString getObjectName();

    void loadModalityData(qint64 dataPos);
    Q_INVOKABLE QVariantList getModalityDataList();
    QImage getModalityImageData(QString name);
    Q_INVOKABLE void saveModalityImageData(QString name, QString savePath);

signals:

public slots:
};

#endif // MMROBJECT_H
