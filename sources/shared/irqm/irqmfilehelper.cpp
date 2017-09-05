#include "irqmfilehelper.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQMFileHelper::IRQMFileHelper(QObject *parent) : QObject(parent) {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QString IRQMFileHelper::loadFileToString(QString path) {
    QByteArray byteArray = loadFileToByteArray(path);
    return QString(byteArray);
}
//------------------------------------------------------------------------------
QByteArray IRQMFileHelper::loadFileToByteArray(QString path) {
    QFile file(path);

    file.open(QIODevice::ReadOnly);
    if (!file.isOpen()) return QByteArray();

    QByteArray byteArray = file.readAll();

    file.close();

    return byteArray;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IRQMFileHelper::saveString(QString string, QString path) {
    return saveByteArray(string.toUtf8(), path);
}
//------------------------------------------------------------------------------
void IRQMFileHelper::saveByteArray(QByteArray byteArray, QString path) {
    createIntermediateDirectories(path);

    QFile file(path);

    file.open(QIODevice::WriteOnly);
    if (!file.isOpen()) return;

    file.write(byteArray);
    file.close();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void IRQMFileHelper::createIntermediateDirectories(QString path) {
    QFileInfo fileInfo(path);
    if (fileInfo.isDir()) {
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists()) {
            dir.mkpath(".");
        }
    }
    else {
        QDir dir = fileInfo.dir();
        if (!dir.exists()) {
            dir.mkpath(".");
        }
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
