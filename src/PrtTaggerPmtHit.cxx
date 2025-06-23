#include "PrtTaggerPmtHit.h"

#include <iomanip>

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

G4ThreadLocal G4Allocator<PrtTaggerPmtHit> *PrtTaggerPmtHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrtTaggerPmtHit::PrtTaggerPmtHit(G4int layerID) : fLayerID(layerID) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PrtTaggerPmtHit::operator==(const PrtTaggerPmtHit & /*right*/) const {
  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrtTaggerPmtHit::Draw() {
  auto visManager = G4VVisManager::GetConcreteInstance();
  if (!visManager) return;

  G4Circle circle(fWorldPos);
  circle.SetScreenSize(2);
  circle.SetFillStyle(G4Circle::filled);
  G4VisAttributes attribs(G4Colour::Yellow());
  circle.SetVisAttributes(attribs);
  visManager->Draw(circle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::map<G4String, G4AttDef> *PrtTaggerPmtHit::GetAttDefs() const {
  G4bool isNew;
  auto store = G4AttDefStore::GetInstance("PrtTaggerPmtHit", isNew);

  if (isNew) {
    (*store)["HitType"] = G4AttDef("HitType", "Hit Type", "Physics", "", "G4String");

    (*store)["ID"] = G4AttDef("ID", "ID", "Physics", "", "G4int");

    (*store)["Time"] = G4AttDef("Time", "Time", "Physics", "G4BestUnit", "G4double");

    (*store)["Pos"] = G4AttDef("Pos", "Position", "Physics", "G4BestUnit", "G4ThreeVector");
  }

  return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue> *PrtTaggerPmtHit::CreateAttValues() const {
  auto values = new std::vector<G4AttValue>;

  values->push_back(G4AttValue("HitType", "PrtTaggerPmtHit", ""));
  values->push_back(G4AttValue("ID", G4UIcommand::ConvertToString(fLayerID), ""));
  values->push_back(G4AttValue("Time", G4BestUnit(fTime, "Time"), ""));
  values->push_back(G4AttValue("Pos", G4BestUnit(fWorldPos, "Length"), ""));

  return values;
}
