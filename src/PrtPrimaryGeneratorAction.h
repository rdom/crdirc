// -----------------------------------------
// PrtPrimaryGeneratorAction class
//
// author  : r.dzhygadlo at gsi.de
// -----------------------------------------

#ifndef PrtPrimaryGeneratorAction_h
#define PrtPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

#include "PrtManager.h"
#include "PrtRun.h"
#include "../cry_v1.7/src/CRYSetup.h"
#include "../cry_v1.7/src/CRYGenerator.h"
#include "../cry_v1.7/src/CRYParticle.h"
#include "../cry_v1.7/src/CRYUtils.h"
#include "vector"
#include "RNGWrapper.h"

class G4ParticleGun;
class G4Event;
class PrtPrimaryGeneratorMessenger;

class PrtPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  PrtPrimaryGeneratorAction();
  virtual ~PrtPrimaryGeneratorAction();

 public:
  virtual void GeneratePrimaries(G4Event *);

  void SetOptPhotonPolar();
  void SetOptPhotonPolar(G4double);
  void InputCRY();
  void UpdateCRY(std::string *MessInput);
  void CRYFromFile(G4String newValue);

 private:
  PrtRun *fRun;
  int fPid, fPdg, iter;
  double fRadiatorL, fRadiatorW, fRadiatorH;
  G4ParticleGun *fParticleGun;
  G4ParticleDefinition *fParticleOP, *fParticle[5];
  PrtPrimaryGeneratorMessenger *fGunMessenger;

  std::vector<CRYParticle *> *vect; // vector of generated particles
  G4ParticleTable *particleTable;
  CRYGenerator *gen;
  G4int InputState;
};

#endif
