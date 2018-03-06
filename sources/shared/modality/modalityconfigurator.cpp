#include "modalityconfigurator.h"

#include "modality.h"
#include "modalitykinect.h"
#include "modalityqtsensor.h"
#include "modalitybitalino.h"
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
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
