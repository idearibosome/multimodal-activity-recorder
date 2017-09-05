#include "irqmpathhelper.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQMPathHelper::IRQMPathHelper(QObject *parent) : QObject(parent) {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QString IRQMPathHelper::lastComponent(QString path) {
    QRegularExpression regex("^.*[\\\\/]([^\\\\/?]+)(.*)$");

    QRegularExpressionMatch regexMatch = regex.match(path);
    if (regexMatch.hasMatch()) {
        return regexMatch.captured(1);
    }

    return "";
}
//------------------------------------------------------------------------------
QString IRQMPathHelper::withoutLastComponent(QString path) {
    QRegularExpression regex("^(.*[\\\\/])([^\\\\/?]+)(.*)$");

    QRegularExpressionMatch regexMatch = regex.match(path);
    if (regexMatch.hasMatch()) {
        return regexMatch.captured(1);
    }

    return path;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QString IRQMPathHelper::fileExtension(QString path) {
    QFileInfo fileInfo(path);
    return fileInfo.suffix();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
