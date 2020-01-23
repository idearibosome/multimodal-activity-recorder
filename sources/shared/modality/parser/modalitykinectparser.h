#ifndef MODALITYKINECTPARSER_H
#define MODALITYKINECTPARSER_H

#include <QDataStream>
#include <QImage>
#include <QIODevice>
#include <QObject>
#include <QVariant>

#include "modalityparser.h"

class ModalityKinectParser : public ModalityParser
{
    Q_OBJECT
public:
    explicit ModalityKinectParser(QObject *parent = nullptr);

    QVariantList parseData(QByteArray data, EModalityParserParseType parseType) override;
};

#endif // MODALITYKINECTPARSER_H
