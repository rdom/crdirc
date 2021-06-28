// -----------------------------------------
// PrtTrackerSD.h
//
// author  : r.dzhygadlo at gsi.de
// -----------------------------------------

#ifndef PrtTrackerSD_h
#define PrtTrackerSD_h 1

#include <vector>
#include "G4VSensitiveDetector.hh"

#include "PrtEvent.h"

class G4Step;
class G4HCofThisEvent;

class PrtTrackerSD : public G4VSensitiveDetector {
 public:
  PrtTrackerSD(const G4String &name, const G4String &hitsCollectionName, G4int nofCells);
  virtual ~PrtTrackerSD();

  // methods from base class
  virtual void Initialize(G4HCofThisEvent *hitCollection);
  virtual bool ProcessHits(G4Step *step, G4TouchableHistory *history);
  virtual void EndOfEvent(G4HCofThisEvent *hitCollection);
};

#endif
