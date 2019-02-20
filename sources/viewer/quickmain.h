#ifndef QUICKMAIN_H
#define QUICKMAIN_H

#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQuickImageProvider>
#include <QVariant>

class MMRObject;
class MMRFileMetadata;
class Modality;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class QuickMainObjectImageProvider : public QQuickImageProvider
{
public:
    QuickMainObjectImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class QuickMain : public QObject
{
    Q_OBJECT
public:
    explicit QuickMain(QObject *parent = nullptr);

    QuickMainObjectImageProvider *objectImageProvider;

    MMRFileMetadata *fileMetadata = NULL;
    QList<MMRObject *> objectList;

    QMutex loadObjectDataMutex;

    Q_INVOKABLE void loadMMRData(QString basePath);
    Q_INVOKABLE void unloadMMRData();

    Q_INVOKABLE MMRFileMetadata *getFileMetadata();

    Q_INVOKABLE int getNumObjects();
    Q_INVOKABLE MMRObject *getObjectAt(int index);
    Q_INVOKABLE void loadObjectData(qint64 timestamp);

    Q_INVOKABLE qint64 getCurrentTimestamp();

    Q_INVOKABLE void exportMMRData(QString exportPath);

private:
    void createMMRObjects();
    void destroyMMRObjects();

signals:

public slots:
};

extern QuickMain *qMain;

#endif // QUICKMAIN_H
