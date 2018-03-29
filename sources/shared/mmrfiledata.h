#ifndef MMRFILEDATA_H
#define MMRFILEDATA_H

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QVariant>

#define MMR_FILE_DATA_VERSION 1

class MMRFileData : public QObject
{
    Q_OBJECT
public:
    explicit MMRFileData(QObject *parent = nullptr);

    bool isReadingMode = true;

    QFile *file = NULL;
    QDataStream *fileDataStream = NULL;

    QVariantMap headerMap;

    static MMRFileData *fromFilePath(QString path);

    void clear();
    void close();

    QVariant getHeaderInfo(QString key);
    void setHeaderInfo(QString key, QVariant value);

    void loadFromFilePath(QString path);

    void prepareWritingToFilePath(QString path);
    void writeData(qint64 timestamp, QByteArray data);
    void finalizeWriting();

    qint64 getCurrentFilePos();

signals:

public slots:
};

#endif // MMRFILEDATA_H
