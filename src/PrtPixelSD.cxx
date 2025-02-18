#include "PrtPixelSD.h"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include <TVector3.h>
#include "G4TransportationManager.hh"

#include "PrtEvent.h"
#include "PrtPrizmHit.h"

#include "PrtRunAction.h"
#include "PrtManager.h"

PrtPixelSD::PrtPixelSD(const G4String &name, const G4String &hitsCollectionName, int nofCells)
  : G4VSensitiveDetector(name) {
  collectionName.insert(hitsCollectionName);
}

PrtPixelSD::~PrtPixelSD() {}

void PrtPixelSD::Initialize(G4HCofThisEvent *hce) {

  memset(fMultHit, 0, sizeof(fMultHit[0][0]) * 960);
  
  double qe_m2018[8][64] = {
    // Photonis XP85012  9001352
    {0.65, 0.72, 0.75, 0.77, 0.78, 0.76, 0.77, 0.63, 0.77, 0.83, 0.84, 0.85, 0.85,
     0.85, 0.84, 0.75, 0.78, 0.84, 0.85, 0.86, 0.86, 0.86, 0.85, 0.80, 0.79, 0.84,
     0.86, 0.87, 0.87, 0.87, 0.85, 0.81, 0.79, 0.84, 0.86, 0.87, 0.87, 0.86, 0.85,
     0.82, 0.78, 0.83, 0.85, 0.86, 0.86, 0.86, 0.85, 0.82, 0.76, 0.82, 0.84, 0.85,
     0.85, 0.84, 0.83, 0.81, 0.67, 0.77, 0.79, 0.81, 0.80, 0.78, 0.77, 0.71},
    // Photonis XP85012  9001358
    {0.63, 0.71, 0.71, 0.71, 0.71, 0.71, 0.70, 0.66, 0.67, 0.74, 0.73, 0.73, 0.72,
     0.72, 0.72, 0.71, 0.69, 0.75, 0.75, 0.74, 0.74, 0.74, 0.74, 0.72, 0.72, 0.82,
     0.76, 0.76, 0.74, 0.74, 0.74, 0.72, 0.73, 0.82, 0.81, 0.80, 0.75, 0.73, 0.73,
     0.71, 0.74, 0.81, 0.81, 0.80, 0.77, 0.73, 0.72, 0.70, 0.73, 0.84, 0.84, 0.84,
     0.81, 0.76, 0.73, 0.71, 0.62, 0.75, 0.75, 0.77, 0.77, 0.72, 0.67, 0.63},
    // Photonis XP85012  9001356
    {0.61, 0.76, 0.78, 0.79, 0.80, 0.80, 0.80, 0.65, 0.72, 0.89, 0.90, 0.90, 0.91,
     0.91, 0.91, 0.80, 0.72, 0.88, 0.90, 0.90, 0.91, 0.91, 0.91, 0.80, 0.71, 0.88,
     0.89, 0.90, 0.91, 0.91, 0.91, 0.81, 0.69, 0.89, 0.90, 0.91, 0.91, 0.91, 0.91,
     0.81, 0.68, 0.89, 0.90, 0.91, 0.91, 0.91, 0.91, 0.78, 0.70, 0.88, 0.89, 0.90,
     0.90, 0.90, 0.90, 0.77, 0.66, 0.78, 0.79, 0.76, 0.75, 0.76, 0.79, 0.67},
    // Photonis XP85012  9001336
    {0.58, 0.71, 0.72, 0.74, 0.76, 0.77, 0.77, 0.69, 0.65, 0.81, 0.82, 0.82, 0.82,
     0.81, 0.80, 0.75, 0.65, 0.82, 0.83, 0.83, 0.83, 0.82, 0.81, 0.76, 0.64, 0.82,
     0.83, 0.83, 0.83, 0.83, 0.82, 0.77, 0.65, 0.82, 0.83, 0.83, 0.83, 0.83, 0.82,
     0.77, 0.66, 0.81, 0.83, 0.83, 0.83, 0.83, 0.82, 0.76, 0.65, 0.80, 0.82, 0.82,
     0.82, 0.82, 0.81, 0.76, 0.57, 0.71, 0.70, 0.71, 0.72, 0.73, 0.72, 0.66},
    // Photonis XP85012  9001355
    {0.51, 0.72, 0.72, 0.73, 0.76, 0.76, 0.76, 0.63, 0.60, 0.85, 0.86, 0.87, 0.87,
     0.86, 0.85, 0.71, 0.61, 0.86, 0.88, 0.88, 0.88, 0.88, 0.86, 0.72, 0.66, 0.87,
     0.88, 0.89, 0.89, 0.88, 0.87, 0.76, 0.71, 0.87, 0.89, 0.89, 0.89, 0.88, 0.87,
     0.75, 0.71, 0.87, 0.88, 0.88, 0.88, 0.88, 0.86, 0.75, 0.71, 0.85, 0.86, 0.87,
     0.87, 0.86, 0.85, 0.76, 0.66, 0.77, 0.76, 0.77, 0.78, 0.77, 0.72, 0.64},
    // Photonis XP85012  9001359
    {0.57, 0.68, 0.68, 0.69, 0.70, 0.69, 0.68, 0.55, 0.70, 0.82, 0.83, 0.83, 0.83,
     0.83, 0.82, 0.69, 0.74, 0.83, 0.84, 0.84, 0.84, 0.84, 0.83, 0.69, 0.75, 0.83,
     0.84, 0.85, 0.85, 0.84, 0.83, 0.69, 0.75, 0.83, 0.84, 0.85, 0.85, 0.84, 0.83,
     0.69, 0.75, 0.83, 0.84, 0.84, 0.84, 0.84, 0.83, 0.72, 0.75, 0.82, 0.83, 0.84,
     0.83, 0.83, 0.82, 0.73, 0.63, 0.71, 0.73, 0.76, 0.74, 0.74, 0.74, 0.63},
    // Photonis XP85012  9001360
    {0.50, 0.64, 0.66, 0.67, 0.66, 0.66, 0.64, 0.56, 0.62, 0.76, 0.76, 0.77, 0.76,
     0.76, 0.76, 0.68, 0.63, 0.77, 0.77, 0.77, 0.77, 0.77, 0.76, 0.69, 0.68, 0.82,
     0.83, 0.83, 0.80, 0.77, 0.76, 0.70, 0.69, 0.83, 0.84, 0.85, 0.85, 0.83, 0.78,
     0.71, 0.67, 0.83, 0.85, 0.85, 0.85, 0.85, 0.83, 0.71, 0.65, 0.82, 0.83, 0.84,
     0.83, 0.83, 0.82, 0.72, 0.47, 0.68, 0.71, 0.72, 0.72, 0.72, 0.72, 0.62},
    // Photonis XP85012  9001358 // placeholder for 1306
    {0.63, 0.71, 0.71, 0.71, 0.71, 0.71, 0.70, 0.66, 0.67, 0.74, 0.73, 0.73, 0.72,
     0.72, 0.72, 0.71, 0.69, 0.75, 0.75, 0.74, 0.74, 0.74, 0.74, 0.72, 0.72, 0.82,
     0.76, 0.76, 0.74, 0.74, 0.74, 0.72, 0.73, 0.82, 0.81, 0.80, 0.75, 0.73, 0.73,
     0.71, 0.74, 0.81, 0.81, 0.80, 0.77, 0.73, 0.72, 0.70, 0.73, 0.84, 0.84, 0.84,
     0.81, 0.76, 0.73, 0.71, 0.62, 0.75, 0.75, 0.77, 0.77, 0.72, 0.67, 0.63}};

  double qe_space_9002224[64] = {
    0.842, 0.922, 0.947, 0.953, 0.952, 0.944, 0.9,   0.76,  0.885, 0.954, 0.971, 0.977, 0.979,
    0.973, 0.959, 0.856, 0.908, 0.969, 0.984, 0.99,  0.992, 0.988, 0.976, 0.925, 0.922, 0.974,
    0.989, 0.997, 0.998, 0.995, 0.985, 0.954, 0.923, 0.975, 0.99,  0.998, 1,     0.997, 0.992,
    0.967, 0.917, 0.965, 0.978, 0.984, 0.988, 0.993, 0.983, 0.954, 0.895, 0.942, 0.955, 0.962,
    0.973, 0.973, 0.961, 0.936, 0.872, 0.916, 0.931, 0.941, 0.952, 0.955, 0.939, 0.911};

  fMcpLayout = PrtManager::Instance()->getRun()->getPmtLayout();
  
  fRunType = PrtManager::Instance()->getRun()->getRunType();
  int npmt = PrtManager::Instance()->getRun()->getNpmt();
  int npix = PrtManager::Instance()->getRun()->getNpix();

  if (fMcpLayout == 2018 || fMcpLayout == 4) {
    for (int m = 0; m < 8; m++) {
      for (int i = 0; i < 64; i++) {
        fQe_space[m][i] = qe_m2018[m][i];
        if (m == 7) fQe_space[m][i] = 0.9 * qe_m2018[m][i];
      }
    }
  } else if (fMcpLayout == 12) {
    for (int m = 0; m < 12; m++) {
      for (int i = 0; i < 64; i++) {
        fQe_space[m][i] = qe_space_9002224[i];
      }
    }
  } else {
    for (int m = 0; m < 12; m++) {
      for (int i = 0; i < 64; i++) {
        fQe_space[m][i] = 1;
      }
    }
  }

  // create MPC map
  for (int ch = 0; ch < npmt * npix; ch++) {
    int mcp = ch / npix;
    int pix = ch % npix;
    // int col = pix / 2 - 8 * (pix / 16);
    // int row = pix % 2 + 2 * (pix / 16);
    // pix = col+sqrt(prt_npix)*row;

    fMap_Mpc[mcp][pix] = ch;
  }
}

bool PrtPixelSD::ProcessHits(G4Step *step, G4TouchableHistory *hist) {

  // // energy deposit
  // double edep = step->GetTotalEnergyDeposit();

  // // step length
  // double stepLength = 0.;
  // if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
  //   stepLength = step->GetStepLength();
  // }

  // if ( edep==0. && stepLength == 0. ) return false;

  if (step == 0) return false;

  // G4ThreeVector translation = hist->GetTranslation();
  // G4ThreeVector localpos = step->GetPreStepPoint()->GetPhysicalVolume()->GetObjectTranslation();
  G4TouchableHistory *touchable = (G4TouchableHistory *)(step->GetPostStepPoint()->GetTouchable());
  // time since track created
  double time = step->GetPreStepPoint()->GetLocalTime();

  G4Track *track = step->GetTrack();
  PrtHit hit;
  int mcpid = touchable->GetReplicaNumber(1);
  int pixid = touchable->GetReplicaNumber(0);
  int ch = fMap_Mpc[mcpid][pixid];

  hit.setChannel(ch);
  hit.setPmt(mcpid);
  hit.setPixel(pixid);

  if (fRunType == 5) {
    time = track->GetTrackLength() / track->CalculateVelocityForOpticalPhoton();
    hit.setLeadTime(time);
    PrtManager::Instance()->addHit(hit);
    return true;
  }

  G4ThreeVector g4pos = track->GetVertexPosition();
  G4ThreeVector lp = touchable->GetHistory()->GetTransform(1).TransformPoint(g4pos); // pos in wDirc
  TVector3 position(lp.x(), lp.y(), lp.z());

  // Get cell id
  int layerNumber = touchable->GetReplicaNumber(0);

  const G4DynamicParticle *dynParticle = track->GetDynamicParticle();
  G4ParticleDefinition *particle = dynParticle->GetDefinition();
  G4String ParticleName = particle->GetParticleName();
  
  G4ThreeVector globalpos = step->GetPostStepPoint()->GetPosition();
  G4ThreeVector localpos = touchable->GetHistory()->GetTopTransform().TransformPoint(globalpos);
  G4ThreeVector translation =
    touchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0, 0, 0));
  G4ThreeVector inPrismpos = touchable->GetHistory()->GetTransform(1).TransformPoint(globalpos);

  G4ThreeVector globalvec = track->GetVertexMomentumDirection();
  G4ThreeVector localvec = touchable->GetHistory()->GetTopTransform().TransformAxis(globalvec);

  G4ThreeVector g4mom = track->GetVertexMomentumDirection(); // track->GetMomentum();

  TVector3 globalPos(inPrismpos.x(), inPrismpos.y(), inPrismpos.z());
  TVector3 localPos(localpos.x(), localpos.y(), localpos.z());

  if (PrtManager::Instance()->getRun()->getRunType() == 6) { // focal plane scan
    globalPos = TVector3(globalpos.x(), globalpos.y(), globalpos.z());
    localPos = TVector3(g4pos.x(), g4pos.y(), g4pos.z());
  }

  translation = touchable->GetHistory()->GetTransform(1).TransformPoint(translation);
  TVector3 digiPos(translation.x(), translation.y(), translation.z());
  TVector3 vertexPos(g4pos.x(), g4pos.y(), g4pos.z());

  // information from prizm
  G4SDManager *fSDM = G4SDManager::GetSDMpointer();
  G4RunManager *fRM = G4RunManager::GetRunManager();
  int collectionID = fSDM->GetCollectionID("PrizmHitsCollection");
  const G4Event *currentEvent = fRM->GetCurrentEvent();
  G4HCofThisEvent *HCofEvent = currentEvent->GetHCofThisEvent();
  PrtPrizmHitsCollection *prizmCol = (PrtPrizmHitsCollection *)(HCofEvent->GetHC(collectionID));

  Long_t pathId = 0;
  int refl = 0;
  for (uint i = 0; i < prizmCol->entries(); i++) {
    PrtPrizmHit *phit = (*prizmCol)[i];
    if (phit->GetTrackID() == track->GetTrackID()) {
      if (phit->GetNormalId() > 0) pathId = pathId * 10 + phit->GetNormalId();
      refl++;
    }
  }

  hit.setPathInPrizm(pathId);

  // hit.SetPosition(globalPos);
  // hit.SetGlobalPos(globalPos;)
  // hit.SetLocalPos(localPos);
  // hit.SetDigiPos(digiPos);
  // hit.SetMomentum(momentum);

  if (fRunType == 6) {
    G4ThreeVector mominend = step->GetPostStepPoint()->GetMomentum();
    TVector3 mominendv(mominend.x(), mominend.y(), mominend.z());
    // hit.SetMomentum(mominendv);
  }
  
  if (fRunType == 0)
    time = G4RandGauss::shoot(time, PrtManager::Instance()->getRun()->getTimeSigma());

  hit.setLeadTime(time);
  double wavelength = 1.2398 / (track->GetMomentum().mag() * 1E6) * 1000;
  hit.setTotTime(wavelength); // set photon wavelength

  // time since event created
  // step->GetPreStepPoint()->GetGlobalTime()*1000

  if (fRunType == 1) {
    PrtManager::Instance()->addHit(hit, localPos, digiPos, vertexPos);
    return true;
  }

  bool qe_space_efficiency(true);
  bool charge_sharing(true);
  bool dead_time(true);
  bool dark_counts(true);
  bool transport_efficiency(true);

  if (fMcpLayout == 2030) {
    charge_sharing = false;
    qe_space_efficiency = false;
    dead_time = false;
  }
  
  if (transport_efficiency) {
    double pi(4 * atan(1));
    double roughness(0.5); // nm
    double angleX = localvec.angle(G4ThreeVector(1, 0, 0));
    double angleY = localvec.angle(G4ThreeVector(0, 1, 0));
    if (angleX > 0.5 * pi) angleX = pi - angleX;
    if (angleY > 0.5 * pi) angleY = pi - angleY;
    double length = track->GetTrackLength() - 400; // 400 - average path in EV
    double lengthx = fabs(length * localvec.x());  // along the bar
    double lengthy = fabs(length * localvec.y());

    int nBouncesX = (int)(lengthx) / 17.1; // 17 bar height
    int nBouncesY = (int)(lengthy) / 35.9; // 36 bar width

    double ll = wavelength * wavelength;
    double n_quartz = sqrt(1. + (0.696 * ll / (ll - pow(0.068, 2))) +
                           (0.407 * ll / (ll - pow(0.116, 2))) + 0.897 * ll / (ll - pow(9.896, 2)));
    double bounce_probX = 1 - pow(4 * pi * cos(angleX) * roughness * n_quartz / wavelength, 2);
    double bounce_probY = 1 - pow(4 * pi * cos(angleY) * roughness * n_quartz / wavelength, 2);

    double totalProb = pow(bounce_probX, nBouncesX) * pow(bounce_probY, nBouncesY);

    if (G4UniformRand() > totalProb) {
      // std::cout<<"photon lost in the radiator. n_bounces = ["<<nBouncesX<<" "<<nBouncesY<<"] with
      // prob= "<<totalProb<<std::endl;
      return true;
    }
  }

  bool is_hit(true);
  if (fRunType == 0 && qe_space_efficiency) {
    if (fQe_space[mcpid][pixid] > G4UniformRand()) {
      if (fMultHit[mcpid][pixid] == 0 || !dead_time)
        PrtManager::Instance()->addHit(hit, localPos, digiPos, position);
      fMultHit[mcpid][pixid]++;
    } else is_hit = false;

  } else {
    if (fMultHit[mcpid][pixid] == 0 || !dead_time || fMcpLayout == 0)
      PrtManager::Instance()->addHit(hit, localPos, digiPos, position);
    fMultHit[mcpid][pixid]++;
  }

  if (is_hit && charge_sharing) {
    // charge sharing for 8x8 MCP
    double pixdim(53 / 16.);
    double chargesig(1.5),threshold(0.3);
    // double chargesig(1.7),threshold(0.7); //high cs / ct  // 1.5/0.9 for 0.5mV // 1.5/0.7 for 1mV
    // double chargesig(1.5), threshold(0.8); // high cs / ct  // 1.5/0.9 for 0.5mV // 1.5/0.7 for 1mV
  
    double x(localPos.x()), y(localPos.y());
    int p(pixid + 1);
    bool ok(false);
    double expd = exp(-(pixdim - fabs(x)) / chargesig);

    if (x < 0 && pixid % 8 != 1 && expd > G4UniformRand() && expd < threshold) {
      ok = true;
      p -= 1;
    }
    if (x > 0 && pixid % 8 != 0 && expd > G4UniformRand() && expd < threshold) {
      ok = true;
      p += 1;
    }
    expd = exp(-(pixdim - fabs(y)) / chargesig);
    if (y < 0 && pixid > 8 && expd > G4UniformRand() && expd < threshold) {
      ok = true;
      p -= 8;
    }
    if (y > 0 && pixid < 57 && expd > G4UniformRand() && expd < threshold) {
      ok = true;
      p += 8;
    }
    p--;
    if (ok) {
      hit.setChannel(fMap_Mpc[mcpid][p]);
      hit.setPixel(p);
      if (fMultHit[mcpid][p] == 0 || !dead_time)
        PrtManager::Instance()->addHit(hit, localPos, digiPos, position);
      fMultHit[mcpid][p]++;
    }
  }

  if (dark_counts) {
  }

  return true;
}

void PrtPixelSD::EndOfEvent(G4HCofThisEvent *) {

  auto e = PrtManager::Instance()->getEvent();
  bool good = (e->getT1Position().Z() < -1 && e->getT2Position().Z() < -1);

  memset(fMultHit, 0, sizeof(fMultHit[0][0]) * 16 * 16 * 12);
  int eventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  if (eventNumber % 1 == 0 && (fRunType == 0 || fRunType == 5) && good)
    std::cout << " : " << PrtManager::Instance()->getEvent()->getHits().size() << std::endl;
  else if (eventNumber % 1000 == 0 && fRunType != 0)
    std::cout << " : " << PrtManager::Instance()->getEvent()->getHits().size() << std::endl;

  good = (good && e->getT3Position().Z() < -1 && e->getHits().size() > 0);
  
  // if (good) std::cout << e->getT1Position().Z() <<" " <<  e->getT2Position().Z() <<" " <<  e->getT3Position().Z() << " hits = " << e->getHits().size() << std::endl;
  
  if (good) PrtManager::Instance()->fill();
}
