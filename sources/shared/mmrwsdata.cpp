#include "mmrwsdata.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRWSData::MMRWSData(QObject *parent) : QObject(parent) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRWSData::loadFromByteArray(QByteArray byteArray) {
    QDataStream inStream(byteArray);
    inStream.setVersion(QDataStream::Qt_5_9);

    inStream >> requestType;
    inStream >> dataType;
    inStream >> data;
}
//---------------------------------------------------------------------------
QByteArray MMRWSData::toByteArray() {
    QByteArray byteArray;
    QDataStream outStream(&byteArray, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_9);

    outStream << requestType;
    outStream << dataType;
    outStream << data;

    return byteArray;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
