#include "PrtEvent.h"
#include <iostream>
ClassImp(PrtEvent)

  PrtEvent::PrtEvent()
  : fPid(0), fTime(0), fTof(0), fTofPi(0), fTofP(0), fMomentum(TVector3(0, 0, 0)),
    fPosition(TVector3(0, 0, 0)), fT1Position(TVector3(0, 0, 0)), fT2Position(TVector3(0, 0, 0)),
    fT3Position(TVector3(0, 0, 0)), det_n_Pho(0), pro_n_Pho(), det_ParentID(), det_Ene_Pho(), 
    pro_Ene_Pho() {}


    // I just wanted to test the variables are read here (CA)
void PrtEvent::Print() const {
    
   // std::cout << "tCerenkovCounter: " << det_n_Pho << std::endl;
   // std::cout << "tEnePho: ";
   // for(const auto& val : det_Ene_Pho) std::cout << val << " ";
   // std::cout << std::endl;

}
