#include "mmrfiledata.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRFileData::MMRFileData(QObject *parent) : QObject(parent) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRFileData *MMRFileData::fromFilePath(QString path) {
    MMRFileData *fileData = new MMRFileData();

    fileData->loadFromFilePath(path);

    return fileData;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileData::clear() {
    close();

    headerMap.clear();
}
//---------------------------------------------------------------------------
void MMRFileData::close() {
    if (fileDataStream) {
        delete fileDataStream;
        fileDataStream = NULL;
    }
    if (file) {
        file->close();
        file->deleteLater();
        file = NULL;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariant MMRFileData::getHeaderInfo(QString key) {
    return headerMap.value(key);
}
//---------------------------------------------------------------------------
void MMRFileData::setHeaderInfo(QString key, QVariant value) {
    headerMap.insert(key, value);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileData::loadFromFilePath(QString path) {
    if (fileDataStream) return;

    clear();

    QString filePath = QDir::cleanPath(path);

    file = new QFile(filePath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        file->deleteLater();
        file = NULL;
        return;
    }

    fileDataStream = new QDataStream(file);
    fileDataStream->setVersion(QDataStream::Qt_5_9);

    // read header
    *fileDataStream >> headerMap;
}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRFileData::prepareWritingToFilePath(QString path) {
    if (fileDataStream) return;

    // prepare properties
    clear();
    isReadingMode = false;
    headerMap.insert("version", MMR_FILE_DATA_VERSION);


    QString filePath = QDir::cleanPath(path);

    file = new QFile(filePath, this);
    if (!file->open(QIODevice::WriteOnly)) {
        file->deleteLater();
        file = NULL;
        return;
    }

    fileDataStream = new QDataStream(file);
    fileDataStream->setVersion(QDataStream::Qt_5_9);

    // write header
    *fileDataStream << headerMap;
}
//---------------------------------------------------------------------------
void MMRFileData::writeData(qint64 timestamp, QByteArray data) {
    if (!fileDataStream) return;

    *fileDataStream << timestamp << data;
}
//---------------------------------------------------------------------------
void MMRFileData::finalizeWriting() {
    if (!file || !fileDataStream) return;

    file->flush();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
qint64 MMRFileData::getCurrentFilePos() {
    if (!file) return -1;

    return file->pos();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
