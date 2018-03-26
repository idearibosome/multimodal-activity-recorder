#include "irqmpathhelper.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQMPathHelper::IRQMPathHelper(QObject *parent) : QObject(parent) {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QString IRQMPathHelper::concatenate(QString path1, QString path2) {
    QString newPath1 = path1.replace(QRegularExpression("([\\\\/]+)$"), "");
    QString newPath2 = path2.replace(QRegularExpression("^([\\\\/]+)"), "");

    QString path = newPath1 + QDir::separator() + newPath2;

    return path;
}
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
