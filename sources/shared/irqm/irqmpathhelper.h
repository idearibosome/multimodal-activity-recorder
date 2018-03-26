#ifndef IRQMPATHHELPER_H
#define IRQMPATHHELPER_H

#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QRegularExpression>

class IRQMPathHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRQMPathHelper(QObject *parent = 0);

    Q_INVOKABLE static QString concatenate(QString path1, QString path2);
    Q_INVOKABLE static QString lastComponent(QString path);
    Q_INVOKABLE static QString withoutLastComponent(QString path);

    Q_INVOKABLE static QString fileExtension(QString path);

signals:

public slots:
};

#endif // IRQMPATHHELPER_H
