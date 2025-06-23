// -----------------------------------------
// PrtEvent.h
//
// author  : r.dzhygadlo at gsi.de
// -----------------------------------------

#ifndef PrtEvent_h
#define PrtEvent_h 1

#include "TObject.h"
#include "TString.h"

#include <vector>
#include "PrtHit.h"

class PrtEvent : public TObject {

 public:
  PrtEvent();
  ~PrtEvent(){};

  void Print() const;


  void addHit(PrtHit hit) { fHitArray.push_back(hit); }

  // accessors
  Int_t getPid() const { return fPid; }
  Double_t getTime() const { return fTime; }
  Double_t getTof() { return fTof; }
  Double_t getTofPi() { return fTofPi; }
  Double_t getTofP() { return fTofP; }
  TVector3 getMomentum() const { return fMomentum; }
  TVector3 getPosition() const { return fPosition; }
  TVector3 getT1Position() const { return fT1Position; }
  TVector3 getT2Position() const { return fT2Position; }
  TVector3 getT3Position() const { return fT3Position; }
  std::vector<PrtHit> getHits() { return fHitArray; }

// T for tagger
  Int_t getTPhoDet() const { return det_n_Pho; } //number of photons detected
  Int_t getTPhoPro() const { return pro_n_Pho; } //number of photons produced
  std::vector<Double_t> getTPhoEneDet() {return det_Ene_Pho; } //energy of the photons detected (actually the wavelength);
  std::vector<Double_t> getTPhoEnePro() {return pro_Ene_Pho; } //energy of the photons produced (actually the wavelength);
  std::vector<Int_t>    getParentIDDet() {return det_ParentID; } //parent ID of the detected photons
   



  // mutators
  void setPid(Int_t v) { fPid = v; }
  void setTime(Double_t v) { fTime = v; }
  void setTof(Double_t v) { fTof = v; }
  void setTofPi(Double_t v) { fTofPi = v; }
  void setTofP(Double_t v) { fTofP = v; }
  void setMomentum(TVector3 v) { fMomentum = v; }
  void setPosition(TVector3 v) { fPosition = v; }
  void setT1Position(TVector3 v) { fT1Position = v; }
  void setT2Position(TVector3 v) { fT2Position = v; }
  void setT3Position(TVector3 v) { fT3Position = v; }

// T for tagger
  void setTPhoDet(Int_t v) { det_n_Pho = v; } //number of photons detected
  void setTPhoPro(Int_t v) { pro_n_Pho = v; } //number of photons produced
  void setTPhoEneDet(std::vector<Double_t> v) { det_Ene_Pho = v; } //energy of the photons detected (actually the wavelength);
  void setTPhoEnePro(std::vector<Double_t> v) { pro_Ene_Pho = v; } //energy of the photons produced (actually the wavelength);
  void setParentIDDet(std::vector<Int_t> v) { det_ParentID = v; } //parent ID of the detected photons

 


 private:
  Int_t fPid;
  Double_t fTime;
  Double_t fTof;
  Double_t fTofPi;
  Double_t fTofP;
  TVector3 fMomentum;
  TVector3 fPosition;
  TVector3 fT1Position;
  TVector3 fT2Position;
  TVector3 fT3Position;



  // I should change the name of the variables to be more descriptive (CA)
Int_t det_n_Pho;
Int_t pro_n_Pho;
std::vector<Int_t> det_ParentID; 
std::vector<Double_t> det_Ene_Pho; //energy of the photons detected (actually the wavelength);
std::vector<Double_t> pro_Ene_Pho; //energy of the photons produced (actually the wavelength);

  std::vector<PrtHit> fHitArray;

  ClassDef(PrtEvent, 8);
};
#endif
