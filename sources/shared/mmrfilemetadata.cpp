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

    basePath = path;

    QString filePath = IRQMPathHelper::concatenate(path, MMR_FILE_METADATA_FILENAME);

    int rc = sqlite3_open_v2(filePath.toUtf8().constData(), &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK) {
        qDebug() << "failed to open sqlite file:" << filePath;
        return;
    }

    updateModalityIdentifierToIdxMap();
}
//---------------------------------------------------------------------------
void MMRFileMetadata::updateModalityIdentifierToIdxMap() {
    modalityIdentifierToIdxMap.clear();

    QVariantList modalities = getModalities();
    for (auto modalityVariant=modalities.begin(); modalityVariant!=modalities.end(); ++modalityVariant) {
        QVariantMap modality = (*modalityVariant).toMap();

        int modalityIdx = modality.value("modality_idx").toInt();
        QString identifier = modality.value("identifier").toString();

        modalityIdentifierToIdxMap.insert(identifier, modalityIdx);
    }
}
//---------------------------------------------------------------------------
QVariantList MMRFileMetadata::getModalities() {
    if (!db) return QVariantList();

    QVariantList modalities;

    QString query = "SELECT * FROM modalities";
    sqlite3_stmt *stmt = IRQMSQLiteHelper::prepare(db, query);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QVariantMap row = IRQMSQLiteHelper::fetchRow(stmt);

        modalities.append(row);
    }

    sqlite3_finalize(stmt);

    return modalities;
}
//---------------------------------------------------------------------------
qint64 MMRFileMetadata::getLength() {
    if (!db) return 0;

    qint64 timestamp = 0;

    QString query = "SELECT timestamp FROM recordings ORDER BY timestamp DESC LIMIT 1";

    sqlite3_stmt *stmt = IRQMSQLiteHelper::prepare(db, query);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        QVariantMap row = IRQMSQLiteHelper::fetchRow(stmt);
        timestamp = row.value("timestamp").toLongLong();
    }
    sqlite3_finalize(stmt);

    return timestamp;
}
//---------------------------------------------------------------------------
qint64 MMRFileMetadata::getModalityDataPos(QString identifier, qint64 timestamp) {
    if (!db) return -1;

    if (!modalityIdentifierToIdxMap.contains(identifier)) return -1;
    int modalityIdx = modalityIdentifierToIdxMap.value(identifier);

    qint64 dataPos = 0;

    QString query = "SELECT * FROM recordings WHERE modality_idx = ? AND timestamp <= ? ORDER BY timestamp DESC LIMIT 1";

    sqlite3_stmt *stmt = IRQMSQLiteHelper::prepare(db, query);
    IRQMSQLiteHelper::bindValue(stmt, 1, modalityIdx);
    IRQMSQLiteHelper::bindValue(stmt, 2, timestamp);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        QVariantMap row = IRQMSQLiteHelper::fetchRow(stmt);
        dataPos = row.value("data_pos").toLongLong();
    }
    sqlite3_finalize(stmt);

    return dataPos;
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
void MMRFileMetadata::beginTransaction() {
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
}
//---------------------------------------------------------------------------
void MMRFileMetadata::commitTransaction() {
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
}
//---------------------------------------------------------------------------
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
