#ifndef IRQMFILEHELPER_H
#define IRQMFILEHELPER_H

#include <QDir>
#include <QFile>
#include <QObject>

class IRQMFileHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRQMFileHelper(QObject *parent = nullptr);

    Q_INVOKABLE static QString loadFileToString(QString path);
    Q_INVOKABLE static QByteArray loadFileToByteArray(QString path);

    Q_INVOKABLE static void saveString(QString string, QString path);
    Q_INVOKABLE static void saveByteArray(QByteArray byteArray, QString path);

    Q_INVOKABLE static void createIntermediateDirectories(QString path);

signals:

public slots:
};

#endif // IRQMFILEHELPER_H
