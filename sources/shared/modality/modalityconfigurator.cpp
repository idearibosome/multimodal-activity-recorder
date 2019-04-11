#include "modalityconfigurator.h"

#include "modality.h"
#include "modalitykinect.h"
#include "modalityqtsensor.h"
#include "modalitybitalino.h"
#include "modalityfitbit.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ModalityConfigurator::ModalityConfigurator(QObject *parent) : QObject(parent)
{

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void ModalityConfigurator::registerModules(QQmlApplicationEngine *engine) {
    Q_UNUSED(engine);

    // modules
    qmlRegisterType<Modality>();

#if defined(MMR_MODALITY_KINECT)
    qmlRegisterType<ModalityKinect>();
#endif

#if defined(MMR_MODALITY_QTSENSOR)
    qmlRegisterType<ModalityQtSensor>();
#endif

#if defined(MMR_MODALITY_BITALINO)
    qmlRegisterType<ModalityBITalino>();
#endif

#if defined(MMR_MODALITY_FITBIT)
    qmlRegisterType<ModalityFitbit>();
#endif
}
//------------------------------------------------------------------------------
Modality *ModalityConfigurator::modalityForType(QString type, QObject *parent) {
    Modality *modality = nullptr;

#if defined(MMR_MODALITY_KINECT)
    if (type == "kinect") {
        modality = new ModalityKinect(parent);
    }
#endif
#if defined(MMR_MODALITY_QTSENSOR)
    if (type == "qtsensor") {
        modality = new ModalityQtSensor(parent);
    }
#endif
#if defined(MMR_MODALITY_BITALINO)
    if (type == "bitalino") {
        modality = new ModalityBITalino(parent);
    }
#endif
#if defined(MMR_MODALITY_FITBIT)
    if (type == "fitbit") {
        modality = new ModalityFitbit(parent);
    }
#endif

    if (!modality) return modality;

    modality->type = type;

    return modality;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
