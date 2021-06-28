#ifndef PrtPrimaryGeneratorMessenger_h
#define PrtPrimaryGeneratorMessenger_h

#include "G4UImessenger.hh"
#include "globals.hh"

class PrtPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

class PrtPrimaryGeneratorMessenger : public G4UImessenger {
 public:
  PrtPrimaryGeneratorMessenger(PrtPrimaryGeneratorAction *);
  virtual ~PrtPrimaryGeneratorMessenger();

  virtual void SetNewValue(G4UIcommand *, G4String);

 private:
  PrtPrimaryGeneratorAction *fPrtAction;
  G4UIdirectory *fGunDir;
  G4UIcmdWithADoubleAndUnit *fPolarCmd;
  G4UIdirectory *CRYDir;
  G4UIcmdWithAString *FileCmd;
  G4UIcmdWithAString *InputCmd;
  G4UIcmdWithoutParameter *UpdateCmd;
  std::string *MessInput;
};

#endif
