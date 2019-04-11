#include "mmrobject.h"

#include "../shared/modality/modality.h"
#include "../shared/modality/modalityconfigurator.h"

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
    if (modality) {
        modality->deleteLater();
        modality = NULL;
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

    modality = ModalityConfigurator::modalityForType(type, this);
    modality->identifier = identifier;
}
//---------------------------------------------------------------------------
void MMRObject::loadFileData(QString basePath) {
    if (!modality) return;

    QString filePath = basePath + QDir::separator() + modality->type + "_" + modality->identifier + ".mmr";
    filePath = QDir::cleanPath(filePath);

    fileData = new MMRFileData(this);
    fileData->loadFromFilePath(filePath);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QString MMRObject::getObjectName() {
    return modality->type + " (" + identifier + ")";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRObject::loadModalityData(qint64 dataPos) {
    if (!modality || !fileData) return;

    if (loadedDataPos == dataPos) return; // already loaded

    QVariantMap timestampAndData = fileData->getData(dataPos);

    QByteArray dataInFile = timestampAndData.value("data").toByteArray();

    QVariantList parsedData = modality->parseData(dataInFile);
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
