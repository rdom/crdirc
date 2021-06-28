#include "PrtPrimaryGeneratorMessenger.h"
#include "PrtPrimaryGeneratorAction.h"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

PrtPrimaryGeneratorMessenger::PrtPrimaryGeneratorMessenger(PrtPrimaryGeneratorAction *PrtGun)
  : G4UImessenger(), fPrtAction(PrtGun) {
  fGunDir = new G4UIdirectory("/Prt/gun/");
  fGunDir->SetGuidance("PrimaryGenerator control");

  fPolarCmd = new G4UIcmdWithADoubleAndUnit("/Prt/gun/optPhotonPolar", this);
  fPolarCmd->SetGuidance("Set linear polarization");
  fPolarCmd->SetGuidance("  angle w.r.t. (k,n) plane");
  fPolarCmd->SetParameterName("angle", true);
  fPolarCmd->SetUnitCategory("Angle");
  fPolarCmd->SetDefaultValue(-360.0);
  fPolarCmd->SetDefaultUnit("deg");
  fPolarCmd->AvailableForStates(G4State_Idle);

  CRYDir = new G4UIdirectory("/CRY/");
  CRYDir->SetGuidance("CRY initialization");

  FileCmd = new G4UIcmdWithAString("/CRY/file", this);
  FileCmd->SetGuidance("This reads the CRY definition from a file");
  FileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  InputCmd = new G4UIcmdWithAString("/CRY/input", this);
  InputCmd->SetGuidance("CRY input lines");
  InputCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  UpdateCmd = new G4UIcmdWithoutParameter("/CRY/update", this);
  UpdateCmd->SetGuidance("Update CRY definition.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed the CRY definition.");
  UpdateCmd->AvailableForStates(G4State_Idle);

  MessInput = new std::string;
}

PrtPrimaryGeneratorMessenger::~PrtPrimaryGeneratorMessenger() {
  delete fPolarCmd;
  delete fGunDir;
  delete CRYDir;
  delete InputCmd;
  delete UpdateCmd;
  delete FileCmd;
}

void PrtPrimaryGeneratorMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
  if (command == fPolarCmd) {
    G4double angle = fPolarCmd->GetNewDoubleValue(newValue);
    if (angle == -360.0 * deg) {
      fPrtAction->SetOptPhotonPolar();
    } else {
      fPrtAction->SetOptPhotonPolar(angle);
    }
  }
  if (command == InputCmd) {
    fPrtAction->InputCRY();
    (*MessInput).append(newValue);
    (*MessInput).append(" ");
  }

  if (command == UpdateCmd) {
    fPrtAction->UpdateCRY(MessInput);
    *MessInput = "";
  }

  if (command == FileCmd) {
    fPrtAction->CRYFromFile(newValue);
  }
}
