// -----------------------------------------
// PrtStackingAction class
//
// author  : r.dzhygadlo at gsi.de
// --------------------------------------

#ifndef PrtStackingAction_H
#define PrtStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"

#include "TGraph.h"
#include "TRandom.h"

class PrtStackingAction : public G4UserStackingAction {
 public:
  PrtStackingAction();
  virtual ~PrtStackingAction();

 public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);
  virtual void NewStage();
  virtual void PrepareNewEvent();

 private:
  int fScintillationCounter;
  int fCerenkovCounter;
  TGraph *fDetEff[3];
  int fQEtype;
  int fRunType;

  int tCerenkovCounter;
  std::vector<double> tEnePho; // t for tagger, energy of the photons produced in eV
  G4String CherenkovVolume;
};

#endif
