#ifndef MODALITYFITBITPARSER_H
#define MODALITYFITBITPARSER_H

#include <QDataStream>
#include <QIODevice>
#include <QObject>
#include <QVariant>

#include "modalityparser.h"

class ModalityFitbitParser : public ModalityParser
{
    Q_OBJECT
public:
    explicit ModalityFitbitParser(QObject *parent = nullptr);

    QVariantList parseData(QByteArray data, EModalityParserParseType parseType) override;
};

#endif // MODALITYFITBITPARSER_H
