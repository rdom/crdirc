#ifndef TaggerPMTHit_h
#define TaggerPMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"



class TaggerPMTHit : public G4VHit
{
public:
  TaggerPMTHit() = default;
  TaggerPMTHit(G4int layerID);
  TaggerPMTHit(const TaggerPMTHit &right) = default;
  ~TaggerPMTHit() override = default;

  TaggerPMTHit& operator=(const TaggerPMTHit &right) = default;
  G4bool operator==(const TaggerPMTHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw() override;
  const std::map<G4String,G4AttDef>* GetAttDefs() const override;
  std::vector<G4AttValue>* CreateAttValues() const override;
 // void Print() override;

  void SetLayerID(G4int z) { fLayerID = z; }
  G4int GetLayerID() const { return fLayerID; }

  void SetTime(G4double t) { fTime = t; }
  G4double GetTime() const { return fTime; }

  void SetLocalPos(G4ThreeVector xyz) { fLocalPos = xyz; }
  G4ThreeVector GetLocalPos() const { return fLocalPos; }

  void SetWorldPos(G4ThreeVector xyz) { fWorldPos = xyz; }
  G4ThreeVector GetWorldPos() const { return fWorldPos; }

  void Add(G4double de);
  G4double GetEdep() const;

  void SetLevel(G4int xyz) { iLevel = xyz; }
  G4int GetLevel() const { return iLevel; }

  void SetPDG(G4int xyz) { iPDG = xyz; }
  G4int GetPDG() const { return iPDG; }

  void SetHitParentID(G4int xyz) { iParentID = xyz; } //I add the word "Hit" to avoid confusion with the G4TrackID
  G4int GetHitParentID() const { return iParentID; }



private:
  G4int fLayerID = -1;

  G4double fTime = 0.;
  G4ThreeVector fLocalPos;
  G4ThreeVector fWorldPos;
  G4double fEdep = 0.;

  G4int iLevel = 0;
  G4int iPDG = 0;

  G4int iParentID = 0;
  

  
};

using TaggerPMTHitsCollection = G4THitsCollection<TaggerPMTHit>;

extern G4ThreadLocal G4Allocator<TaggerPMTHit>* TaggerPMTHitAllocator;

inline void* TaggerPMTHit::operator new(size_t)
{
  if (!TaggerPMTHitAllocator) {
    TaggerPMTHitAllocator = new G4Allocator<TaggerPMTHit>;
  }
  return (void*)TaggerPMTHitAllocator->MallocSingle();
}

inline void TaggerPMTHit::operator delete(void* aHit)
{
  TaggerPMTHitAllocator->FreeSingle((TaggerPMTHit*) aHit);
}

inline G4double TaggerPMTHit::GetEdep() const {
  return fEdep;
}

inline void TaggerPMTHit::Add(G4double de)
{
  // G4cout<<"fEdep (1): "<<fEdep<<G4endl;
  //  G4cout<<"de: "<<de<<G4endl;
  fEdep += de;
  //  G4cout<<"fEdep (2): "<<fEdep<<G4endl;

}



#endif
