#ifndef MMRFILEMETADATA_H
#define MMRFILEMETADATA_H

#include <QDataStream>
#include <QDebug>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

#include "sqlite/sqlite3.h"

#define MMR_FILE_METADATA_FILENAME "mmr.sqlite"

class MMRFileMetadata : public QObject
{
    Q_OBJECT
public:
    explicit MMRFileMetadata(QObject *parent = nullptr);

    QString basePath;

    sqlite3 *db = NULL;
    QMutex dbMutex, dbTransactionMutex;

    bool isInMemoryDb = false;
    QString dbFilePath;

    QMap<QString, int> modalityIdentifierToIdxMap;

    void clear();
    void close();

    void loadFromFileDirPath(QString path);
    void updateModalityIdentifierToIdxMap();
    Q_INVOKABLE QVariantList getModalities();
    Q_INVOKABLE qint64 getLength();
    Q_INVOKABLE qint64 getModalityDataPos(QString identifier, qint64 timestamp);
    QVariantList getModalityRecordings(QString identifier);

    void createToFileDirPath(QString path, bool inMemoryMode);
    void addModality(QString type, QString identifier, QVariantMap configuration);
    void addRecording(QString identifier, qint64 dataPos, qint64 timestamp);
    void beginTransaction();
    void commitTransaction();
    void finalizeWriting();


private:
    void initializeTables();

signals:

public slots:
};

#endif // MMRFILEMETADATA_H
