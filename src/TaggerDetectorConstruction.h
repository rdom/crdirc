#ifndef TaggerDetectorConstruction_h
#define TaggerDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include <G4UnionSolid.hh> 
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4RotationMatrix.hh"
#include "G4GDMLParser.hh"
class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;


class TaggerDetectorConstruction
{
  public:
   TaggerDetectorConstruction();
  ~TaggerDetectorConstruction();

    void ConstructSDandField();

    // get methods
    //
    void BuildTagger(G4LogicalVolume *worldLV);


  private:

  
    // methods
    //
    void DefineMaterials();
  G4Material* Air;
  G4Element* C;
  G4Element* O; 
  G4Element *Al; 
  G4Material* Aluminum;
  G4Element *Ag; 
  G4Material *CO2;
  G4Material* PLA;
  G4Material* Silver;

  G4Element* Si;
  G4Element* B;
  G4Element* Na;
  G4Material* BorosilicateGlass;

  G4Material* Acrylic;
  G4Material* defaultMaterial; 

  G4MaterialPropertiesTable* MPTBorosilicateGlass;
    // data members
    //
  G4Box* MainBox;
  G4LogicalVolume*  MainBoxLV;

  G4Box* ExtBox;
  G4LogicalVolume*  ExtBoxLV;


  G4UnionSolid* Tagger;
  G4LogicalVolume*  TaggerLV;



  G4Box* MainGas;  
  G4Box* ExtGas;  
  G4UnionSolid* Gas;
  G4LogicalVolume*  GasLV;



//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
// Winston cone 

G4Tubs* WCMother;
G4LogicalVolume*  WCMotherLV;

G4Tubs* WCShell_1;
G4Tubs* WCShell_2;

G4LogicalVolume*  WCS1LV;
G4LogicalVolume*  WCS2LV;


G4Tubs* WC;


G4GDMLParser fParser; 

G4Material* AluminizedMylar;
G4MaterialPropertiesTable* MPTAluminizedMylar;

G4LogicalVolume* MirrorLV;
G4MaterialPropertiesTable* MPTSilver;

G4LogicalVolume* MirrorSurfLV;


G4Tubs* PMT;
G4LogicalVolume* PMTLV;
G4String SDname = "taggerPMT";

G4double lambda[326];
G4double en_phoCath_Roman[326];

    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
                                      // magnetic field messenger

    G4VPhysicalVolume* fAbsorberPV = nullptr; // the absorber physical volume
    G4VPhysicalVolume* fGapPV = nullptr;      // the gap physical volume

    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
};




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

