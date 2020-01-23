#include "mmrobject.h"

#include "../shared/modality/modalityconfigurator.h"
#include "../shared/modality/parser/modalityparser.h"

#include "../shared/mmrfiledata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRObject::MMRObject(QObject *parent) : QObject(parent) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRObject::clear() {
    if (fileData) {
        fileData->clear();
        fileData->deleteLater();
        fileData = NULL;
    }
    if (modalityParser) {
        modalityParser->deleteLater();
        modalityParser = NULL;
    }

    loadedData.clear();
    loadedImageData.clear();
    loadedDataPos = -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRObject::setModalityInfo(QVariantMap info) {
    identifier = info.value("identifier").toString();

    QString type = info.value("type").toString();

    modalityParser = ModalityConfigurator::modalityParserForType(type, this);
}
//---------------------------------------------------------------------------
void MMRObject::loadFileData(QString basePath) {
    if (!modalityParser) return;

    QString filePath = basePath + QDir::separator() + modalityParser->type + "_" + identifier + ".mmr";
    filePath = QDir::cleanPath(filePath);

    fileData = new MMRFileData(this);
    fileData->loadFromFilePath(filePath);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QString MMRObject::getObjectName() {
    return modalityParser->type + " (" + identifier + ")";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRObject::loadModalityData(qint64 dataPos) {
    if (!modalityParser || !fileData) return;

    if (loadedDataPos == dataPos) return; // already loaded

    QVariantMap timestampAndData = fileData->getData(dataPos);

    QByteArray dataInFile = timestampAndData.value("data").toByteArray();

    QVariantList parsedData = modalityParser->parseData(dataInFile);
    QMap<QString, QImage> parsedImageData;

    for (int i=0; i<parsedData.count(); i++) {
        QVariantMap data = parsedData.at(i).toMap();
        QString dataName = data.value("name").toString();
        QString dataType = data.value("type").toString();

        if (dataType == "image") {
            parsedImageData.insert(dataName, data.value("image").value<QImage>());
            data.remove("image");
        }

        parsedData.replace(i, data);
    }

    loadedData = parsedData;
    loadedImageData = parsedImageData;
    loadedDataPos = dataPos;
}
//---------------------------------------------------------------------------
QVariantList MMRObject::getModalityDataList() {
    return loadedData;
}
//---------------------------------------------------------------------------
QImage MMRObject::getModalityImageData(QString name) {
    if (loadedImageData.contains(name)) {
        return loadedImageData.value(name);
    }
    return QImage();
}
//---------------------------------------------------------------------------
void MMRObject::saveModalityImageData(QString name, QString savePath) {
    QImage image = getModalityImageData(name);
    image.save(savePath);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
