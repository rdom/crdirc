#include "G4TrajectoryContainer.hh"
#include "G4Event.hh"
#include "G4Trajectory.hh"

#include "PrtEventAction.h"

#include <fstream>


#include "PrtManager.h"
#include "PrtEvent.h"



// Don't ask!! no clue how this piece of code works.
// I would like to modify it to make it more readable
namespace {

  G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
      auto hce = event->GetHCofThisEvent();
      if (!hce) {
          G4ExceptionDescription msg;
          msg << "No hits collection of this event found." << G4endl;
          G4Exception("EventAction::EndOfEventAction()",
                      "Code001", JustWarning, msg);
          return nullptr;
      }

  
      auto hc = hce->GetHC(collId);
      if (!hc) {
          G4ExceptionDescription msg;
          msg << "Hits collection " << collId << " of this event not found." << G4endl;
          G4Exception("EventAction::EndOfEventAction()",
                      "Code001", JustWarning, msg);
      }
      return hc;
  }
  
  } // End of namespace



void PrtEventAction::BeginOfEventAction(const G4Event* event){
 // tagger Analysis data
 auto sdManager = G4SDManager::GetSDMpointer();

 // I want to modify these lines in order to have explicit names of the variables
 G4String dHCName = { "TaggerPMT/TaggerPMTHitsCollection" };
 ftaggerHCID[0] = sdManager->GetCollectionID(dHCName);

}




void PrtEventAction::EndOfEventAction(const G4Event* event){


// I just copy the method I used for the tagger
// I guess it is not the best way to do it, but it works
// should consult Roman about this (CA)


auto hc = GetHC(event, ftaggerHCID[0]);
if ( ! hc ) { G4cout<<"NO HC"<<G4endl; return;}
 
int nhit = hc->GetSize();

for (unsigned long i = 0; i < nhit; ++i)
  {
   PrtTaggerPmtHit *hit = static_cast<PrtTaggerPmtHit*>(hc->GetHit(i));
    
   EnePhoDet.push_back(hit -> GetEdep());

    if(hit -> GetHitParentID()>1)
//    G4cout << "ParentID: " << hit -> GetHitParentID() << G4endl;
      eParentID.push_back(hit -> GetHitParentID()); // this is the parent ID of the photon DETECTED
    // This will tell us which particle produced the photon 
  }

PrtEvent* prtevent = PrtManager::Instance()->getEvent();
prtevent -> setParentIDDet(eParentID); // set the parent ID of the detected photons
prtevent -> setTPhoDet(nhit); // set the number of detected photons
prtevent -> setTPhoEneDet(EnePhoDet); // set the energy of the detected photons 


nhit = 0; // not always there are hits in the tagger 
EnePhoDet.clear();
eParentID.clear();


//prtevent->Print();

// export trajectories
  // G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
  // G4int ntr = 0;
  // if (tc) ntr = tc->entries();
  
  // if(ntr>0){
  //   std::ofstream  file;
  //   file.open ("trackst.json");
  //   file<< "{\"event\": [";
  //   for (G4int i=0; i<ntr; i++){

  //     G4Trajectory* trj = (G4Trajectory*)((*tc)[i]);
  //     file<<"{\"energy\": "<<trj->GetInitialMomentum().mag() <<" }, \n";
  //     file<<"{\"path\": [";
  //     G4int np = trj->GetPointEntries();
  //     for(G4int p=0; p<np; p=p+2){
  // 	//     G4VTrajectoryPoint* tp = trj->GetPoint(p);
  // 	G4ThreeVector pos = trj->GetPoint(p)->GetPosition();
  // 	if(p<np-2) file<< "{\"v\": ["<< pos.x()<<","<<pos.y()<<","<<pos.z()<<"]},";
  // 	else file<< "{\"v\": ["<< pos.x()<<","<<pos.y()<<","<<pos.z()<<"]}";
  //     }
  //     if(i<ntr-1) file<< "]}, \n";
  //     else file<< "]} \n";
  //   }
  //   file<< "]} \n\n";

  //   file.close();
  // }
}

