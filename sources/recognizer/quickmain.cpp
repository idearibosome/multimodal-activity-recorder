#include "quickmain.h"

#include "mmrclient.h"

#include "../shared/modality/modality.h"
#include "../shared/modality/modalitykinect.h"
#include "../shared/modality/modalityqtsensor.h"
#include "../shared/modality/modalitybitalino.h"
#include "../shared/modality/modalityfitbit.h"

QuickMain *qMain;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QuickMain::QuickMain(QObject *parent) : QObject(parent) {
    client = new MMRClient(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
