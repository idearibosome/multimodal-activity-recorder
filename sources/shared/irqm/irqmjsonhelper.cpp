#include "irqmjsonhelper.h"

#include "irqmfilehelper.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQMJSONHelper::IRQMJSONHelper(QObject *parent) : QObject(parent) {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QVariant IRQMJSONHelper::fileToJSONObject(QString path) {
    QByteArray byteArray = IRQMFileHelper::loadFileToByteArray(path);
    return byteArrayToJSONObject(byteArray);
}
//------------------------------------------------------------------------------
QVariant IRQMJSONHelper::ioDeviceToJSONObject(QIODevice *ioDevice) {
    if (!ioDevice) return QVariant();

    if (!ioDevice->isOpen()) ioDevice->open(QIODevice::ReadOnly);
    if (!ioDevice->isOpen()) return QVariant();

    QByteArray jsonByteArray = ioDevice->readAll();

    return byteArrayToJSONObject(jsonByteArray);
}
//------------------------------------------------------------------------------
QVariant IRQMJSONHelper::stringToJSONObject(QString jsonString) {
    return byteArrayToJSONObject(jsonString.toUtf8());
}
//------------------------------------------------------------------------------
QVariant IRQMJSONHelper::byteArrayToJSONObject(QByteArray jsonByteArray) {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonByteArray);

    if (jsonDocument.isArray()) {
        return jsonDocument.array().toVariantList();
    }
    else if (jsonDocument.isObject()) {
        return jsonDocument.object().toVariantMap();
    }

    return QVariant();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IRQMJSONHelper::saveJSONObjectToFile(QVariant jsonObject, QString path) {
    QByteArray jsonByteArray = JSONObjectToByteArray(jsonObject);
    IRQMFileHelper::saveByteArray(jsonByteArray, path);
}
//------------------------------------------------------------------------------
QString IRQMJSONHelper::JSONObjectToString(QVariant jsonObject) {
    QByteArray jsonByteArray = JSONObjectToByteArray(jsonObject);
    return QString(jsonByteArray);
}
//------------------------------------------------------------------------------
QByteArray IRQMJSONHelper::JSONObjectToByteArray(QVariant jsonObject) {
    QByteArray jsonByteArray = QJsonDocument::fromVariant(jsonObject).toJson(QJsonDocument::Compact);
    return jsonByteArray;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
