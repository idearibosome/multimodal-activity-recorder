#ifndef IRQMRANDOMHELPER_H
#define IRQMRANDOMHELPER_H

#include <QObject>

class IRQMRandomHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRQMRandomHelper(QObject *parent = 0);

    Q_INVOKABLE static int intWithinBounds(int lowerBound, int upperBound);
    Q_INVOKABLE static int positiveInt();

signals:

public slots:
};

#endif // IRQMRANDOMHELPER_H
