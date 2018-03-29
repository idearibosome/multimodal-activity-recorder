#include "mmrfilemetadata.h"

#include "irqm/irqmpathhelper.h"
#include "irqm/irqmsqlitehelper.h"

#include "sqlite3.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRFileMetadata::MMRFileMetadata(QObject *parent) : QObject(parent) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRFileMetadata *MMRFileMetadata::fromFileDirPath(QString path) {
    MMRFileMetadata *fileMetadata = new MMRFileMetadata();

    fileMetadata->loadFromFileDirPath(path);

    return fileMetadata;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileMetadata::clear() {
    close();

    modalityIdentifierToIdxMap.clear();
}
//---------------------------------------------------------------------------
void MMRFileMetadata::close() {
    {
        QMutexLocker dbMutexLocker(&dbMutex);
        if (db) {
            sqlite3_close_v2(db);
            db = NULL;
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileMetadata::loadFromFileDirPath(QString path) {
    if (db) return;

    QString filePath = IRQMPathHelper::concatenate(path, MMR_FILE_METADATA_FILENAME);

    int rc = sqlite3_open_v2(filePath.toUtf8().constData(), &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK) {
        qDebug() << "failed to open sqlite file:" << filePath;
        return;
    }

    // TODO
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileMetadata::createToFileDirPath(QString path) {
    if (db) return;

    QString filePath = IRQMPathHelper::concatenate(path, MMR_FILE_METADATA_FILENAME);

    int rc = sqlite3_open_v2(filePath.toUtf8().constData(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) {
        qDebug() << "failed to open or create sqlite file:" << filePath;
        return;
    }

    initializeTables();
}
//---------------------------------------------------------------------------
void MMRFileMetadata::addModality(QString type, QString identifier, QVariantMap configuration) {

    // configuration to blob
    QByteArray configurationByteArray;
    QDataStream configurationDataStream(&configurationByteArray, QIODevice::WriteOnly);
    configurationDataStream << configuration;

    // sqlite3
    int modalityIdx = 0;
    {
        QMutexLocker dbMutexLocker(&dbMutex);

        QString query = "INSERT INTO modalities (type, identifier, configuration) VALUES (?, ?, ?)";
        sqlite3_stmt *stmt = IRQMSQLiteHelper::prepare(db, query);
        IRQMSQLiteHelper::bindValue(stmt, 1, type);
        IRQMSQLiteHelper::bindValue(stmt, 2, identifier);
        IRQMSQLiteHelper::bindValue(stmt, 3, configurationByteArray);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            modalityIdx = sqlite3_last_insert_rowid(db);
        }
        sqlite3_finalize(stmt);
    }

    // modalityIdentifierToIdxMap
    modalityIdentifierToIdxMap.insert(identifier, modalityIdx);
}
//---------------------------------------------------------------------------
void MMRFileMetadata::addRecording(QString identifier, qint64 dataPos, qint64 timestamp) {

    // modalityIdx
    if (!modalityIdentifierToIdxMap.contains(identifier)) return;
    int modalityIdx = modalityIdentifierToIdxMap.value(identifier);

    // sqlite3
    {
        QMutexLocker dbMutexLocker(&dbMutex);

        QString query = "INSERT INTO recordings (modality_idx, data_pos, timestamp) VALUES (?, ?, ?)";
        sqlite3_stmt *stmt = IRQMSQLiteHelper::prepare(db, query);
        IRQMSQLiteHelper::bindValue(stmt, 1, modalityIdx);
        IRQMSQLiteHelper::bindValue(stmt, 2, dataPos);
        IRQMSQLiteHelper::bindValue(stmt, 3, timestamp);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
//---------------------------------------------------------------------------
void MMRFileMetadata::finalizeWriting() {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileMetadata::initializeTables() {
    if (!db) return;

    QString queryString;


    // table creation
    queryString = "CREATE TABLE IF NOT EXISTS db_info ( "
            "key TEXT NOT NULL, "
            "value BLOB "
            ")";
    sqlite3_exec(db, queryString.toUtf8().constData(), 0, 0, NULL);

    queryString = "CREATE TABLE IF NOT EXISTS modalities ( "
            "modality_idx INTEGER PRIMARY KEY AUTOINCREMENT, "
            "type TEXT, "
            "identifier TEXT, "
            "configuration BLOB "
            ")";
    sqlite3_exec(db, queryString.toUtf8().constData(), 0, 0, NULL);

    queryString = "CREATE TABLE IF NOT EXISTS recordings ( "
            "recording_idx INTEGER PRIMARY KEY AUTOINCREMENT, "
            "modality_idx INTEGER NOT NULL, "
            "data_pos INTEGER, "
            "timestamp INTEGER "
            ")";
    sqlite3_exec(db, queryString.toUtf8().constData(), 0, 0, NULL);


    // index creation
    queryString = "CREATE INDEX IF NOT EXISTS "
            "recordings_modality_index_timestamp ON recordings "
            "(modality_idx, timestamp)";
    sqlite3_exec(db, queryString.toUtf8().constData(), 0, 0, NULL);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
