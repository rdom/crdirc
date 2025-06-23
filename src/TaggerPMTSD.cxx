#include "TaggerPMTSD.h"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "TRandom.h"

#include "PrtManager.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


TaggerPMTSD::TaggerPMTSD(G4String name)
: G4VSensitiveDetector(name)
{
  G4cout << "<TaggerPMTSD> Info" << G4endl; 
  collectionName.insert("TaggerPMTHitsCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TaggerPMTSD::Initialize(G4HCofThisEvent* hce)
{
 // G4cout << "<TaggerPMTSD> Initialize" << G4endl;


  fHitsCollection
    = new TaggerPMTHitsCollection(SensitiveDetectorName,collectionName[0]);

  if (fHCID<0) {
     fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
  }
  hce->AddHitsCollection(fHCID,fHitsCollection);

}




G4bool TaggerPMTSD::ProcessHits(G4Step* step,
                                     G4TouchableHistory*)
{
 
 // G4cout << "<TaggerPMTSD> ProcessHits" << G4endl;

  // From Example B5
  // This is a driftchamber SD
  auto charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
 // if (charge==0.) return true;

  auto preStepPoint = step->GetPreStepPoint();

  auto touchable = step->GetPreStepPoint()->GetTouchable();
  auto motherPhysical = touchable->GetVolume(1); // mother
  auto copyNo = motherPhysical->GetCopyNo();

  
  auto worldPos = preStepPoint->GetPosition();
  auto localPos
    = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);

  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();


  G4int ParticleLevel = step->GetTrack()-> GetTrackID();
  G4int PDGID = step->GetTrack()->GetDefinition()-> GetPDGEncoding();
  
  auto hit = new TaggerPMTHit(copyNo);

  G4int iParentID = step->GetTrack()-> GetParentID();  

  // this is similar to the SteppingAction analysis
  // But I don't know how this behave with multiple
  // interactions in a given event
  
  hit->SetWorldPos(worldPos);
  hit->SetLocalPos(localPos);
  hit->SetTime(preStepPoint->GetGlobalTime());

  hit->Add(edep);

  hit->SetPDG(PDGID);
  hit->SetLevel(ParticleLevel);
  hit->SetHitParentID(iParentID);
 

  // THIS IS MY EFFICIENCY DEFINITION
 G4double ra = gRandom->Uniform(0., 1.);
 if (ra > 0.5) // 50% chance to save the hit
 {
          return false; // maybe false?
 }
 // not sure
fHitsCollection->insert(hit);

//just to see it is copied
 return true;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TaggerPMTSD::EndOfEvent(G4HCofThisEvent*)
{
 // G4cout << "<TaggerPMTSD> EndOfEvent" << G4endl; 

  
  //  if ( verboseLevel>1 )
    {
     auto nofHits = fHitsCollection->entries();
 //    G4cout
 //      << G4endl
 //      << "-------->Hits Collection: in this event they are " << nofHits
 //      << " hits in the PMT. " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


