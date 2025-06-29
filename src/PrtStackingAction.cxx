#include "PrtStackingAction.h"

#include "G4VProcess.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"

#include "PrtManager.h"

PrtStackingAction::PrtStackingAction() : G4UserStackingAction(), fScintillationCounter(0), fCerenkovCounter(0) {

  fRunType = PrtManager::Instance()->getRun()->getRunType();
  int layout = PrtManager::Instance()->getRun()->getPmtLayout();
  fQEtype = 0;
  
  if (layout == 4) fQEtype = 0;
  if (layout == 12) fQEtype = 2; // panda mcp-pmts
  if (layout == 2030) fQEtype = 1;
  
  // create a detector efficiency function:
  {
    // quantum efficiency data from Alex Britting, Jan 25, 2011
    // unit is percent
    // first value is at 200 nm, last at 700 nm
    // credible range start around 250nm, >= 280nm to be safe
    double eff_std[502] = {
      0,     231.84, 615.36, 657.4, 258.78, 9839.92, 44.67, 67.87, 51.01, 41.49, 5.36,  49.4,
      2.13,  35.49,  8.66,   5.03,  7.51,   13.27,   18.71, 3.92,  3.66,  8.2,   0.56,  7.68,
      2.87,  10.06,  3.47,   3.39,  6.99,   6.01,    4.92,  6.25,  5.97,  6.92,  8.29,  10.45,
      8.68,  8.6,    9.79,   11.76, 9.53,   10.98,   9.9,   10.97, 11.31, 10.88, 10.78, 12.16,
      12.38, 12.37,  13.04,  12.36, 13.18,  13.7,    13.85, 13.66, 13.98, 14.55, 14.93, 14.82,
      14.97, 14.98,  15.14,  15.35, 15.37,  15.43,   15.49, 15.59, 15.84, 15.84, 15.92, 16.01,
      16.22, 16.41,  16.42,  16.52, 16.86,  17.1,    17.17, 17.22, 17.46, 17.79, 17.99, 18.13,
      18.33, 18.34,  18.53,  18.72, 18.95,  19.02,   19.15, 19.28, 19.45, 19.66, 19.69, 19.77,
      19.73, 19.95,  19.98,  20.17, 20.29,  20.33,   20.37, 20.47, 20.48, 20.57, 20.75, 20.8,
      20.84, 20.86,  20.88,  21.0,  21.06,  21.0,    21.06, 21.06, 21.04, 21.1,  21.14, 21.08,
      21.17, 21.3,   21.38,  21.49, 21.58,  21.69,   21.77, 21.87, 22.02, 22.13, 22.29, 22.35,
      22.45, 22.53,  22.55,  22.64, 22.67,  22.73,   22.74, 22.71, 22.79, 22.76, 22.77, 22.76,
      22.75, 22.78,  22.7,   22.68, 22.72,  22.66,   22.64, 22.7,  22.67, 22.71, 22.67, 22.75,
      22.77, 22.83,  22.84,  22.93, 22.97,  23.0,    23.08, 23.16, 23.27, 23.25, 23.37, 23.44,
      23.49, 23.55,  23.52,  23.58, 23.64,  23.63,   23.58, 23.64, 23.63, 23.62, 23.64, 23.63,
      23.66, 23.59,  23.59,  23.56, 23.58,  23.63,   23.57, 23.66, 23.62, 23.67, 23.64, 23.54,
      23.57, 23.51,  23.53,  23.45, 23.3,   23.41,   23.25, 23.21, 23.08, 23.01, 22.92, 22.9,
      22.76, 22.76,  22.61,  22.53, 22.48,  22.39,   22.29, 22.24, 22.2,  22.12, 22.07, 21.96,
      21.89, 21.87,  21.76,  21.74, 21.58,  21.49,   21.48, 21.37, 21.29, 21.2,  21.17, 21.03,
      20.98, 20.92,  20.85,  20.76, 20.69,  20.58,   20.56, 20.47, 20.37, 20.32, 20.24, 20.13,
      20.08, 19.9,   19.84,  19.77, 19.69,  19.63,   19.51, 19.41, 19.27, 19.06, 19.01, 18.87,
      18.7,  18.49,  18.41,  18.17, 17.98,  17.84,   17.69, 17.5,  17.25, 17.15, 16.98, 16.79,
      16.66, 16.48,  16.32,  16.19, 16.02,  15.88,   15.77, 15.67, 15.5,  15.39, 15.23, 15.09,
      15.04, 14.92,  14.75,  14.7,  14.5,   14.45,   14.34, 14.25, 14.16, 14.13, 14.0,  13.92,
      13.84, 13.76,  13.73,  13.61, 13.54,  13.52,   13.45, 13.41, 13.39, 13.31, 13.22, 13.17,
      13.13, 13.06,  13.2,   13.09, 12.97,  12.92,   12.73, 12.65, 12.4,  12.22, 12.02, 11.79,
      11.59, 11.33,  11.03,  10.68, 10.46,  10.14,   9.88,  9.62,  9.36,  9.14,  8.87,  8.63,
      8.51,  8.24,   8.07,   7.88,  7.77,   7.65,    7.52,  7.35,  7.27,  7.21,  7.1,   6.92,
      6.89,  6.79,   6.74,   6.56,  6.54,   6.5,     6.39,  6.33,  6.25,  6.27,  6.14,  6.06,
      6.04,  6.01,   5.91,   5.89,  5.79,   5.75,    5.75,  5.67,  5.61,  5.51,  5.52,  5.43,
      5.43,  5.34,   5.31,   5.35,  5.23,   5.2,     5.14,  5.11,  5.11,  5.01,  4.98,  4.93,
      4.99,  4.89,   4.82,   4.87,  4.8,    4.7,     4.65,  4.65,  4.61,  4.49,  4.56,  4.44,
      4.42,  4.44,   4.35,   4.35,  4.27,   4.29,    4.19,  4.13,  4.08,  4.02,  4.07,  3.92,
      3.95,  3.88,   3.82,   3.86,  3.74,   3.71,    3.66,  3.72,  3.62,  3.55,  3.56,  3.57,
      3.45,  3.38,   3.36,   3.36,  3.28,   3.25,    3.19,  3.26,  3.13,  3.17,  3.15,  3.04,
      2.98,  2.93,   2.98,   2.9,   2.89,   2.9,     2.81,  2.74,  2.81,  2.68,  2.73,  2.7,
      2.57,  2.58,   2.55,   2.55,  2.37,   2.39,    2.39,  2.44,  2.37,  2.26,  2.27,  2.27,
      2.23,  2.26,   2.14,   2.08,  2.15,   2.06,    2.09,  2.04,  2.0,   1.95,  2.02,  1.87,
      1.9,   1.8,    1.87,   1.85,  1.87,   1.81,    1.86,  1.74,  1.74,  1.63,  1.59,  1.5,
      1.5,   1.44,   1.47,   1.32,  1.24,   1.28,    1.19,  1.21,  1.21,  1.1,   1.1,   1.05,
      1.06,  0.94,   0.92,   0.87,  0.92,   0.81,    0.86,  0.78,  0.77,  0.8,   0.67,  0.7,
      0.81,  0.61,   0.64,   0.71,  0.66,   0.67,    0.68,  0.69,  0.68,  0.73};

    double eff_400[251] = {
      0,    0,    14.0, 14.8, 14.5, 14.9, 14.4, 14.2, 13.9, 14.6, 15.2, 15.7, 16.4, 16.9, 17.5,
      17.7, 18.1, 18.8, 19.3, 19.8, 20.6, 21.4, 22.4, 23.1, 23.6, 24.1, 24.2, 24.6, 24.8, 25.2,
      25.7, 26.5, 27.1, 28.2, 29.0, 29.9, 30.8, 31.1, 31.7, 31.8, 31.6, 31.5, 31.5, 31.3, 31.0,
      30.8, 30.8, 30.4, 30.2, 30.3, 30.2, 30.1, 30.1, 30.1, 29.8, 29.9, 29.8, 29.7, 29.7, 29.7,
      29.8, 29.8, 29.9, 29.9, 29.8, 29.9, 29.8, 29.9, 29.8, 29.7, 29.8, 29.7, 29.8, 29.6, 29.5,
      29.7, 29.7, 29.8, 30.1, 30.4, 31.0, 31.3, 31.5, 31.8, 31.8, 31.9, 32.0, 32.0, 32.0, 32.0,
      32.2, 32.2, 32.1, 31.8, 31.8, 31.8, 31.7, 31.6, 31.6, 31.7, 31.5, 31.5, 31.4, 31.3, 31.3,
      31.2, 30.8, 30.7, 30.5, 30.3, 29.9, 29.5, 29.3, 29.2, 28.6, 28.2, 27.9, 27.8, 27.3, 27.0,
      26.6, 26.1, 25.9, 25.5, 25.0, 24.6, 24.2, 23.8, 23.4, 23.0, 22.7, 22.4, 21.9, 21.4, 21.2,
      20.7, 20.3, 19.8, 19.6, 19.3, 18.9, 18.7, 18.3, 17.9, 17.8, 17.8, 16.7, 16.5, 16.4, 16.0,
      15.6, 15.6, 15.2, 14.9, 14.6, 14.4, 14.1, 13.8, 13.6, 13.3, 13.0, 12.8, 12.6, 12.3, 12.0,
      11.9, 11.7, 11.5, 11.2, 11.1, 10.9, 10.7, 10.4, 10.3, 9.9,  9.8,  9.6,  9.3,  9.1,  9.0,
      8.8,  8.5,  8.3,  8.3,  8.2,  7.9,  7.8,  7.7,  7.5,  7.3,  7.1,  6.9,  6.7,  6.6,  6.3,
      6.2,  6.0,  5.8,  5.7,  5.6,  5.4,  5.2,  5.1,  4.9,  4.8,  4.6,  4.5,  4.4,  4.2,  4.1,
      4.0,  3.8,  3.7,  3.5,  3.3,  3.2,  3.1,  3.0,  2.9,  2.5,  2.4,  2.4,  2.3,  2.3,  2.1,
      1.8,  1.6,  1.5,  1.5,  1.6,  1.8,  1.9,  1.4,  0.8,  0.9,  0.8,  0.7,  0.6,  0.3,  0.3,
      0.5,  0.3,  0.4,  0.3,  0.1,  0.2,  0.1,  0.2,  0.3,  0.0};

    // [200,850] nm with 2 nm step
    double eff_9002224[326] = {
      2.21, -24.6, -27,   -10.8, -6.2,  -8.2, -14.8, -19.9, -14.1, -14,  -22.4, -21.9, -13.8, -16.1,
      -7.3, -9.26, -7.43, -4.04, -0.49, 2.86, 4.55,  5.34,  10.1,  8.51, 8.35,  11,    11.6,  12.9,
      12.6, 13.1,  13.8,  13.7,  13.8,  13.7, 13.9,  13.8,  14.1,  14.6, 15,    15.4,  15.8,  16.2,
      16.4, 16.8,  17.1,  17.3,  17.6,  17.6, 17.9,  18,    18,    18.1, 18.2,  18.3,  18.3,  18.4,
      18.4, 18.4,  18.4,  18.5,  18.7,  18.7, 18.9,  19.1,  19.3,  19.5, 19.6,  19.7,  19.7,  19.8,
      19.8, 19.8,  19.6,  19.6,  19.6,  19.6, 19.5,  19.5,  19.5,  19.6, 19.7,  19.6,  19.8,  19.9,
      20.1, 20.3,  20.3,  20.6,  20.7,  20.8, 20.9,  21,    21,    21,   21,    20.9,  20.9,  20.9,
      20.8, 20.6,  20.4,  20.4,  20.2,  20.1, 19.9,  19.7,  19.8,  19.6, 19.4,  19.4,  19.2,  19,
      18.8, 18.6,  18.5,  18.3,  18.1,  18.1, 17.9,  17.7,  17.5,  17.4, 17.2,  17.1,  16.9,  16.7,
      16.5, 16.4,  15.9,  15.7,  15.1,  14.6, 14.7,  14.4,  14.3,  14,   13.6,  13.3,  13.1,  12.9,
      12.8, 12.5,  12.2,  12,    11.9,  11.7, 11.5,  11.3,  11.2,  11.1, 10.9,  10.8,  10.7,  10.5,
      10.3, 10,    9.73,  9.33,  8.86,  8.34, 7.86,  7.27,  6.91,  6.48, 6.01,  5.85,  5.32,  5.04,
      4.76, 4.56,  4.36,  4.17,  3.98,  3.82, 3.69,  3.54,  3.41,  3.27, 3.16,  3.03,  2.92,  2.8,
      2.7,  2.59,  2.49,  2.4,   2.3,   2.2,  2.11,  2.03,  1.94,  1.85, 1.76,  1.68,  1.61,  1.52,
      1.46, 1.37,  1.29,  1.24,  1.17,  1.11, 1.05,  0.99,  0.94,  0.89, 0.83,  0.79,  0.76,  0.7,
      0.66, 0.63,  0.6,   0.56,  0.53,  0.51, 0.48,  0.46,  0.44,  0.42, 0.4,   0.4,   0.4,   0.38,
      0.37, 0.36,  0.37,  0.35,  0.34,  0.35, 0.35,  0.35,  0.36,  0.35, 0.33,  0.31,  0.3,   0.28,
      0.27, 0.26,  0.25,  0.23,  0.22,  0.21, 0.2,   0.18,  0.18,  0.17, 0.17,  0.14,  0.16,  0.16,
      0.16, 0.16,  0.16,  0.16,  0.16,  0.16, 0.16,  0.15,  0.15,  0.15, 0.16,  0.16,  0.17,  0.17,
      0.17, 0.17,  0.16,  0.15,  0.15,  0.16, 0.16,  0.16,  0.16,  0.17, 0.18,  0.17,  0.16,  0.15,
      0.16, 0.18,  0.18,  0.17,  0.15,  0.08, 0.13,  0.16,  0.17,  0.17, 0.18,  0.18,  0.18,  0.18,
      0.17, 0.17,  0.17,  0.14,  0.14,  0.15, 0.16,  0.15,  0.14,  0.15, 0.16,  0.16,  0.16,  0.15,
      0.14, 0.13,  0.13,  0.11,  0.08,  0,    0.01,  0.03,  0.02,  0.04, 0.07,  0,     0.05,  0.09,
      0.07, 0.1,   0.14,  0.17};

    double lambda[3][1000] = {{0}}, efficiencyR[3][1000] = {{0}}, collectionEff = 0.65;

    for (int i = 0; i < 1000; i++) {
      // Std QE (Photonis 2)
      collectionEff = 0.65;
      lambda[0][i] = 200 + i;
      efficiencyR[0][i] = (i > 40 && i < 500) ? eff_std[i] * 0.01 * collectionEff : 0;

      // HIQ 400 (Photonis 1)
      collectionEff = 0.95;
      lambda[1][i] = 180 + i * 2;
      efficiencyR[1][i] = (i < 251) ? eff_400[i] * 0.01 * collectionEff : 0;

      // panda 9002224
      collectionEff = 0.95;
      lambda[2][i] = 200 + i * 2;
      efficiencyR[2][i] = (lambda[2][i] >= 240 && lambda[2][i] <= 850) ? eff_9002224[i] * 0.01 * collectionEff : 0;
    }

    fDetEff[0] = new TGraph(1000, lambda[0], efficiencyR[0]);
    fDetEff[1] = new TGraph(1000, lambda[1], efficiencyR[1]);
    fDetEff[2] = new TGraph(1000, lambda[2], efficiencyR[2]);

    // fDetEff[1]->Draw("APL");
    // fDetEff[0]->Draw("PL same");
    // fDetEff[2]->SetLineColor(kBlue);
    // fDetEff[2]->Draw("PL same");
    // gPad->Update();
    // gPad->Print("qe_wave_crdirc.C");
    // gPad->WaitPrimitive();
  }
}

PrtStackingAction::~PrtStackingAction() {}

G4ClassificationOfNewTrack PrtStackingAction::ClassifyNewTrack(const G4Track *aTrack) {

 G4VPhysicalVolume* currentVolume = aTrack->GetVolume();


 // Stupid pointer could be null, so check it 
 if (currentVolume) {
    G4String volumeName = currentVolume->GetName();
   // G4cout<< "CherenkovVolume: " << volumeName <<G4endl;
    CherenkovVolume = aTrack->GetVolume()->GetName();
}




  if (aTrack->GetDefinition() ==
    G4OpticalPhoton::OpticalPhotonDefinition()) 
    { // particle is optical photon
      if (aTrack->GetParentID() > 0) 
      {                // particle is secondary
        if (aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation") fScintillationCounter++;
        if (aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov") 
        {
          fCerenkovCounter++;
          if (CherenkovVolume == "Gas_phy") 
          {
            tCerenkovCounter++; // t for tagger (CA)
            //Originally photon energy was in eV. I think Roman removed all the dependency of external libraries
            // that means, that now it is in MeV. Also the tagger detector must be checked to remove the dependencies on the units
            tEnePho.push_back(aTrack->GetTotalEnergy());
          }
        }
      }
    }

  if (fRunType == 5) {
    G4String ParticleName =
      aTrack->GetDynamicParticle()->GetParticleDefinition()->GetParticleName();
    // kill secondaries
    if (aTrack->GetParentID() == 1 && ParticleName != "opticalphoton") return fKill;
  }

  if (fRunType == 0) { // for the simulation run

    // if(aTrack->GetDefinition()->GetParticleName()=="opticalphoton"
    //    && aTrack->GetParentID()!=1)  return fKill;

    G4String ParticleName = aTrack->GetDynamicParticle()->GetParticleDefinition()->GetParticleName();

    // kill opticalphotons from secondaries
    // if(aTrack->GetParentID() == 1 && ParticleName == "opticalphoton" ) 	  return fKill;
    
    if (ParticleName == "opticalphoton") {

      // //tmp phs cut
      // if(fabs(aTrack->GetMomentum().unit().y())>0.15 ) return fKill;
      double lambda = 197.0 * 2.0 * pi / (aTrack->GetMomentum().mag() * 1.0E6);

      if (true) { // apply detector efficiency at the production stage:
        double ra = gRandom->Uniform(0, 1);
        if (ra > fDetEff[fQEtype]->Eval(lambda)) return fKill;
      }

    } else {
      // remove low-energy particle
      // if(aTrack->GetMomentum().mag() < 1000) return fKill;
    }
  }

  return fUrgent;
}


void PrtStackingAction::NewStage() {
  int runtype = PrtManager::Instance()->getRun()->getRunType();
  auto e = PrtManager::Instance()->getEvent();

  bool good = (e->getT1Position().Z() < -1 && e->getT2Position().Z() < -1);
  
  int eventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

  if (eventNumber % 1 == 0 && (runtype == 0 || runtype == 5) && good ) std::cout << "Event # " << eventNumber  << " # Cherenkov photons:  " << fCerenkovCounter;
  else if (eventNumber % 1000 == 0 && runtype != 0) std::cout << "Event # " << eventNumber  << " # Cherenkov photons:  " << fCerenkovCounter ;

  PrtEvent* event = PrtManager::Instance()->getEvent();
  event-> setTPhoPro(tCerenkovCounter);
  event-> setTPhoEnePro(tEnePho);


}

void PrtStackingAction::PrepareNewEvent() {
  fScintillationCounter = 0;
  fCerenkovCounter = 0;
  CherenkovVolume = "";
  tCerenkovCounter = 0;
  tEnePho.clear();  


}
