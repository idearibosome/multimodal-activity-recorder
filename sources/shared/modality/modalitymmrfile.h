#ifndef MODALITYMMRFILE_H
#define MODALITYMMRFILE_H

#include <QObject>
#include <QTimer>

#include "modality.h"

class MMRFileData;

class ModalityMMRFile : public Modality
{
    Q_OBJECT
public:
    explicit ModalityMMRFile(QObject *parent = nullptr);

    QString filePath;
    QVariantList recordings;

    QVariantList pendingRecordings;
    MMRFileData *fileData = nullptr;
    QTimer *dataTimer = nullptr;

    bool initialize(QVariantMap configuration) override;
    void reset() override;

    bool startAcquisition() override;
    void stopAcquisition() override;

private slots:
    void slotDataTimerTimeout();

};

#endif // MODALITYMMRFILE_H
