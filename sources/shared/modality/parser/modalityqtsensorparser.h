#ifndef MODALITYQTSENSORPARSER_H
#define MODALITYQTSENSORPARSER_H

#include <QDataStream>
#include <QIODevice>
#include <QObject>
#include <QVariant>

#include "modalityparser.h"

class ModalityQtSensorParser : public ModalityParser
{
    Q_OBJECT
public:
    explicit ModalityQtSensorParser(QObject *parent = nullptr);

    QVariantList parseData(QByteArray data, EModalityParserParseType parseType) override;
};

#endif // MODALITYQTSENSORPARSER_H
