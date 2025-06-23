#ifndef TaggerPMTSD_h
#define TaggerPMTSD_h 1

#include "G4VSensitiveDetector.hh"

#include "PrtTaggerPmtHit.h"

#include <vector>

class G4Step;
class G4HCofThisEvent;


class TaggerPMTSD : public G4VSensitiveDetector
{
public:
  TaggerPMTSD(G4String name);
  ~TaggerPMTSD() override = default;
  
  void Initialize(G4HCofThisEvent*HCE) override;
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
  void   EndOfEvent(G4HCofThisEvent* hitCollection) override;

private:
  PrtTaggerPmtHitsCollection* fHitsCollection = nullptr;
  G4int fHCID = -1;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

