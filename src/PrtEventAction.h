// -----------------------------------------
// PrtEventAction.h
//
// Author  : R.Dzhygadlo at gsi.de
// -----------------------------------------

#ifndef PrtEventAction_h
#define PrtEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4SDManager.hh"
#include "G4VHitsCollection.hh"
#include "G4Exception.hh"

#include "TGraph.h"
#include "TRandom.h"

#include "PrtTaggerPmtHit.h"

#include <vector>

class G4Track;
class PrtEventAction : public G4UserEventAction
{
public:
  PrtEventAction() {;}
  virtual ~PrtEventAction() {;}
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);


private:
   G4int ftaggerHCID[1];
  std::vector<G4double> EnePhoDet; // energy of the photons detected;
  std::vector<G4int> eParentID; // I added this vector to store the parent ID of the detected photons. 


   protected:
  G4EventManager* fpEventManager;
};
#endif
