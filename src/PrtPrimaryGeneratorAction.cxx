#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "Randomize.hh"
#include "globals.hh"

#include "PrtPrimaryGeneratorAction.h"
#include "PrtPrimaryGeneratorMessenger.h"

PrtPrimaryGeneratorAction::PrtPrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction(), fParticleGun(0) {
  int n_particle = 1;
  fRun = PrtManager::Instance()->getRun();
  double mom = fRun->getMomentum();
  fRadiatorL = fRun->getRadiatorL();
  fRadiatorW = fRun->getRadiatorW();
  fRadiatorH = fRun->getRadiatorH();

  fParticleGun = new G4ParticleGun(n_particle);

  // create a messenger for this class
  fGunMessenger = new PrtPrimaryGeneratorMessenger(this);

  // default kinematic
  particleTable = G4ParticleTable::GetParticleTable();
  fParticle[4] = particleTable->FindParticle("proton");
  fParticle[3] = particleTable->FindParticle("kaon+");
  fParticle[2] = particleTable->FindParticle("pi+");
  fParticle[1] = particleTable->FindParticle("mu+");
  fParticle[0] = particleTable->FindParticle("e-");

  fParticleOP = particleTable->FindParticle("opticalphoton");

  fParticleGun->SetParticleDefinition(fParticleOP);
  fParticleGun->SetParticleTime(0.0 * ns);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0 * cm, 0.0 * cm, 0.0 * cm));
  fParticleGun->SetParticleMomentum(G4ThreeVector(0, 0, mom * GeV));

  iter = 0;
  fPid = 4;

  // Read the cry input file
  TString inputfile = "../cry_v1.7/setup.file";
  std::ifstream inputFile;
  inputFile.open(inputfile.Data(), std::ios::in);
  char buffer[1000];

  if (inputFile.fail()) {
    if (*inputfile.Data() != 0) //....only complain if a filename was given
      G4cout << "PrimaryGeneratorAction: Failed to open CRY input file= " << inputfile.Data() << G4endl;
    InputState = -1;
  } else {
    std::string setupString("");
    while (!inputFile.getline(buffer, 1000).eof()) {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup *setup = new CRYSetup(setupString, "../cry_v1.7/data");

    gen = new CRYGenerator(setup);

    // set random number generator
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),
                                            &CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    InputState = 0;
  }
  // create a vector to store the CRY particle properties
  vect = new std::vector<CRYParticle *>;
}

PrtPrimaryGeneratorAction::~PrtPrimaryGeneratorAction() {
  delete fParticleGun;
  delete fGunMessenger;
}

void PrtPrimaryGeneratorAction::InputCRY() {
  InputState = 1;
}

void PrtPrimaryGeneratorAction::UpdateCRY(std::string *MessInput) {
  CRYSetup *setup = new CRYSetup(*MessInput, "../cry_v1.7/data");

  gen = new CRYGenerator(setup);

  // set random number generator
  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),
                                          &CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  InputState = 0;
}

void PrtPrimaryGeneratorAction::CRYFromFile(G4String newValue) {
  // Read the cry input file
  std::ifstream inputFile;
  inputFile.open(newValue, std::ios::in);
  char buffer[1000];

  if (inputFile.fail()) {
    G4cout << "Failed to open input file " << newValue << G4endl;
    G4cout << "Make sure to define the cry library on the command line" << G4endl;
    InputState = -1;
  } else {
    std::string setupString("");
    while (!inputFile.getline(buffer, 1000).eof()) {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup *setup = new CRYSetup(setupString, "../cry_v1.7/data");

    gen = new CRYGenerator(setup);

    // set random number generator
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),
                                            &CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    InputState = 0;
  }
}

void PrtPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {

  PrtManager::Instance()->addEvent(PrtEvent());

  if (fRun->getRunType() == 0) { // cry particle source

    if (InputState != 0) {
      G4String *str = new G4String("CRY library was not successfully initialized");
      // G4Exception(*str);
      G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, *str);
    }
    G4String particleName;
    vect->clear();
    gen->genEvent(vect);

    // //....debug output
    // std::cout << "\nEvent=" << anEvent->GetEventID() << " "
    //           << "CRY generated nparticles=" << vect->size() << std::endl;

    for (unsigned j = 0; j < vect->size(); j++) {
      particleName = CRYUtils::partName((*vect)[j]->id());

      // //....debug output
      // std::cout << "  " << particleName << " "
      //           << "charge=" << (*vect)[j]->charge() << " " << std::setprecision(4)
      //           << "energy (MeV)=" << (*vect)[j]->ke() * MeV << " "
      //           << "pos (m)" << G4ThreeVector((*vect)[j]->x(), (*vect)[j]->y(), (*vect)[j]->z())
      //           << " "
      //           << "direction cosines "
      //           << G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()) << " "
      // 	        << " time " << (*vect)[j]->t()
      //           << std::endl;
      
      fParticleGun->SetParticleDefinition(particleTable->FindParticle((*vect)[j]->PDGid()));
      fParticleGun->SetParticleEnergy((*vect)[j]->ke() * MeV);
      fParticleGun->SetParticlePosition(
        G4ThreeVector((*vect)[j]->x() * m, (*vect)[j]->y() * m, (*vect)[j]->z() * m));
      fParticleGun->SetParticleMomentumDirection(
        G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
      fParticleGun->SetParticleTime((*vect)[j]->t());
      fParticleGun->GeneratePrimaryVertex(anEvent);

      delete (*vect)[j];
    }
  }

  if (fRun->getRunType() == 1) { // LUT generation

    fParticleGun->SetParticlePosition(G4ThreeVector(fRun->getPrismStepY(), //+5-10*G4UniformRand(),
                                                    fRun->getPrismStepX(), //+10-20*G4UniformRand(),
                                                    fRadiatorL / 2. - 0.1));

    double angle = -G4UniformRand() * M_PI;
    G4ThreeVector v(0, 0, 1);
    v.setTheta(acos(G4UniformRand()));
    v.setPhi(2 * M_PI * G4UniformRand());

    fParticleGun->SetParticleMomentumDirection(v);
    PrtManager::Instance()->setMomentum(TVector3(v.x(), v.y(), v.z()));

    fParticleGun->GeneratePrimaryVertex(anEvent);
    G4ThreeVector dir = fParticleGun->GetParticleMomentumDirection();
    dir *= fParticleGun->GetParticleMomentum() * 0.001; // GeV/c

    PrtManager::Instance()->getEvent()->setMomentum(TVector3(dir.x(), dir.y(), dir.z()));
  }

  if (fRun->getRunType() == 7) {
    int pdg = fRun->getPid();
    if (pdg > 0) {
      if (pdg == 2212) fPid = 4;
      else if (pdg == 321) fPid = 3;
      else if (pdg == 211) fPid = 2;
      else if (pdg == 10001 && fPid > 2) fPid = 2;
      else if (pdg == 10001 && fPid == 2) fPid = 4;
      else if (pdg == 10002 && fPid > 2) fPid = 2;
      else if (pdg == 10002 && fPid == 2) fPid = 3;

      PrtManager::Instance()->getEvent()->setPid(fPid);
      fParticleGun->SetParticleDefinition(fParticle[fPid]);
    } else {
      fParticleGun->SetParticleDefinition(fParticleOP);
    }
  }
  
}

void PrtPrimaryGeneratorAction::SetOptPhotonPolar() {
  double angle = G4UniformRand() * 360.0 * deg;
  SetOptPhotonPolar(angle);
}

void PrtPrimaryGeneratorAction::SetOptPhotonPolar(double angle) {
  if (fParticleGun->GetParticleDefinition()->GetParticleName() != "opticalphoton") {
    G4cout << "--> warning from PrimaryGeneratorAction::SetOptPhotonPolar() :"
              "the particleGun is not an opticalphoton "
           << fParticleGun->GetParticleDefinition()->GetParticleName() << G4endl;
    return;
  }

  G4ThreeVector normal(1., 0., 0.);
  G4ThreeVector kphoton = fParticleGun->GetParticleMomentumDirection();
  G4ThreeVector product = normal.cross(kphoton);
  double modul2 = product * product;

  G4ThreeVector e_perpend(0., 0., 1.);
  if (modul2 > 0.) e_perpend = (1. / std::sqrt(modul2)) * product;
  G4ThreeVector e_paralle = e_perpend.cross(kphoton);

  G4ThreeVector polar = std::cos(angle) * e_paralle + std::sin(angle) * e_perpend;
  fParticleGun->SetParticlePolarization(polar);
}
