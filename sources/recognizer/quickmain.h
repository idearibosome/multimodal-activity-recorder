#ifndef QUICKMAIN_H
#define QUICKMAIN_H

#include <QObject>
#include <QVariant>

class MMRClient;
class Modality;

class QuickMain : public QObject
{
    Q_OBJECT
public:
    explicit QuickMain(QObject *parent = nullptr);

    MMRClient *client;

signals:

public slots:
};

extern QuickMain *qMain;

#endif // QUICKMAIN_H
