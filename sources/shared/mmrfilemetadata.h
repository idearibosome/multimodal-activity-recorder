#ifndef MMRFILEMETADATA_H
#define MMRFILEMETADATA_H

#include <QDataStream>
#include <QDebug>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

#include "sqlite3.h"

#define MMR_FILE_METADATA_FILENAME "mmr.sqlite"

class MMRFileMetadata : public QObject
{
    Q_OBJECT
public:
    explicit MMRFileMetadata(QObject *parent = nullptr);

    sqlite3 *db = NULL;
    QMutex dbMutex;

    QMap<QString, int> modalityIdentifierToIdxMap;

    static MMRFileMetadata *fromFileDirPath(QString path);

    void clear();
    void close();

    void loadFromFileDirPath(QString path);

    void createToFileDirPath(QString path);
    void addModality(QString type, QString identifier, QVariantMap configuration);
    void addRecording(QString identifier, qint64 dataPos, qint64 timestamp);
    void finalizeWriting();


private:
    void initializeTables();

signals:

public slots:
};

#endif // MMRFILEMETADATA_H
