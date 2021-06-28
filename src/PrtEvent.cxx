#include "PrtEvent.h"

ClassImp(PrtEvent)

  PrtEvent::PrtEvent()
  : fPid(0), fTime(0), fTof(0), fTofPi(0), fTofP(0), fMomentum(TVector3(0, 0, 0)),
    fPosition(TVector3(0, 0, 0)), fT1Position(TVector3(0, 0, 0)), fT2Position(TVector3(0, 0, 0)),
    fT3Position(TVector3(0, 0, 0)) {}
