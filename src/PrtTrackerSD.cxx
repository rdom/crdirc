#include "PrtTrackerSD.h"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4TransportationManager.hh"
#include <TVector3.h>

#include "PrtEvent.h"

#include "PrtRunAction.h"
#include "PrtManager.h"

PrtTrackerSD::PrtTrackerSD(const G4String &name, const G4String &hitsCollectionName, G4int nofCells) : G4VSensitiveDetector(name) {
  collectionName.insert(hitsCollectionName);
}

PrtTrackerSD::~PrtTrackerSD() {}

void PrtTrackerSD::Initialize(G4HCofThisEvent *hce) {}

bool PrtTrackerSD::ProcessHits(G4Step *step, G4TouchableHistory *hist) {

  G4ThreeVector gpos = step->GetPreStepPoint()->GetPosition();
  TVector3 vgpos = TVector3(gpos.x(),gpos.y(),gpos.z());
  G4Track *track = step->GetTrack();
  G4ThreeVector vmom = track->GetMomentum();
  
  TString aname = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  // TString bname = step->GetPostStepPoint()->GetPhysicalVolume()->GetName();
  // std::cout << aname<<" "<<step->GetPostStepPoint()->GetPhysicalVolume() << " gpos " << gpos << std::endl;

  if (aname == "wTracker1") PrtManager::Instance()->getEvent()->setT1Position(vgpos);
  else if (aname == "wTracker2") {
    PrtManager::Instance()->getEvent()->setT2Position(vgpos);
    PrtManager::Instance()->getEvent()->setMomentum(TVector3(vmom.x(), vmom.y(), vmom.z()));
    double time = step->GetPreStepPoint()->GetGlobalTime();
    PrtManager::Instance()->getEvent()->setTime(time);
  } else if (aname == "wTracker3") {
    PrtManager::Instance()->getEvent()->setT3Position(vgpos);
  }

  return true;
}

void PrtTrackerSD::EndOfEvent(G4HCofThisEvent *) {}
