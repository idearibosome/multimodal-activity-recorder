#ifndef MODALITYBITALINOPARSER_H
#define MODALITYBITALINOPARSER_H

#include <QDataStream>
#include <QIODevice>
#include <QObject>
#include <QVariant>

#include "modalityparser.h"

class ModalityBITalinoParser : public ModalityParser
{
    Q_OBJECT
public:
    explicit ModalityBITalinoParser(QObject *parent = nullptr);

    QVariantList parseData(QByteArray data, EModalityParserParseType parseType) override;
};

#endif // MODALITYBITALINOPARSER_H
