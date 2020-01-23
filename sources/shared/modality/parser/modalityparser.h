#ifndef MODALITYPARSER_H
#define MODALITYPARSER_H

#include <QImage>
#include <QObject>
#include <QVariant>

class ModalityParser : public QObject
{
    Q_OBJECT
public:
    explicit ModalityParser(QObject *parent = nullptr);

    enum EModalityParserParseType {
        ParseType_Viewer = 0,
        ParseType_Recognizer = 1
    };

    QString type;

    virtual QVariantList parseData(QByteArray data, EModalityParserParseType parseType = ParseType_Viewer) = 0;
    static QVariant parsedDataItemWithValue(QString name, QVariant value);
    static QVariant parsedDataItemWithByteArray(QString name, QByteArray byteArray);
    static QVariant parsedDataItemWithImage(QString name, QImage image);

public slots:
};

#endif // MODALITYPARSER_H
