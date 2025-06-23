// Implemented by Carlos https://github.com/ayerbeg

#include "PrtTaggerDetectorConstruction.h"
#include "PrtTaggerPmtSD.h"

#include "G4Element.hh"
#include "G4Material.hh"

#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SDManager.hh"

PrtTaggerDetectorConstruction::PrtTaggerDetectorConstruction() {
  G4cout << "PrtTaggerDetectorConstruction" << G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrtTaggerDetectorConstruction::~PrtTaggerDetectorConstruction() {
  G4cout << "PrtTaggerDetectorConstruction done" << G4endl;
}

void PrtTaggerDetectorConstruction::DefineMaterials() {

  // Vacuum and air are just to test the optical properties
  // inside the Tagger volume

  // Vacuum
  new G4Material("Galactic", 1., 1.01 * g / mole, universe_mean_density, kStateGas, 2.73 * kelvin,
                 3.e-18 * pascal);

  // Air
  Air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  //  defaultMaterial = G4Material::GetMaterial("Galactic");
  defaultMaterial = Air;

  // Define photon energy range
  G4double AirphotonEnergy[] = {2.0 * eV, 3.5 * eV}; // Visible light range
  const G4int AirnEntries = sizeof(AirphotonEnergy) / sizeof(G4double);

  // Define refractive index for air
  G4double AirrefractiveIndex[] = {1.0003, 1.0003}; // Refractive index for air
  assert(sizeof(AirrefractiveIndex) == sizeof(AirphotonEnergy));

  // Define absorption length for air
  G4double AirabsorptionLength[] = {10 * km, 10 * km}; // Absorption length for air
  assert(sizeof(AirabsorptionLength) == sizeof(AirphotonEnergy));

  // Create material properties table for air
  G4MaterialPropertiesTable *airMPT = new G4MaterialPropertiesTable();
  airMPT->AddProperty("RINDEX", AirphotonEnergy, AirrefractiveIndex, AirnEntries);
  airMPT->AddProperty("ABSLENGTH", AirphotonEnergy, AirabsorptionLength, AirnEntries);

  // Assign the material properties table to air
  Air->SetMaterialPropertiesTable(airMPT);

  // Test materials *******************************************************

  // Materials for the Tagger
  C = new G4Element("Carbon", "C", 6., 12.01 * g / mole); // name, symbol, Z, A
  O = new G4Element("Oxygen", "O", 8., 16.00 * g / mole); // name, symbol, Z, A

  CO2 = new G4Material("CO2",
                       1.98 * kg / m3, // density
                       2,              // number of components
                       kStateGas,
                       300. * kelvin,    // temperature
                       1. * atmosphere); // pressure
  CO2->AddElement(C, 1);                 // element, number of atoms
  CO2->AddElement(O, 2);                 // element, number of atoms

  // This is the energy range of the visible photons
  // This is common for all the optical properties
  // unless is discretized, as will be in with the PMT
  G4double photonEnergy[] = {2.0 * eV, 3.5 * eV}; // Define the energy range
  const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

  //***********From Copilot****************************** */
  // Add optical properties to the CO2

  // refractive index for CO2 tabulated from
  // https://refractiveindex.info/?shelf=main&book=CO2&page=Bideau-Mehu

  G4double CO2photonEnergy[27] = {1.78 * eV, 1.82 * eV, 1.86 * eV, 1.91 * eV, 1.95 * eV, 2 * eV,
                                  2.05 * eV, 2.1 * eV,  2.16 * eV, 2.22 * eV, 2.28 * eV, 2.34 * eV,
                                  2.41 * eV, 2.49 * eV, 2.56 * eV, 2.65 * eV, 2.74 * eV, 2.83 * eV,
                                  2.93 * eV, 3.04 * eV, 3.16 * eV, 3.28 * eV, 3.42 * eV, 3.57 * eV,
                                  3.73 * eV, 3.91 * eV, 4.11 * eV};

  G4double CO2refractiveIndex[27] = {
    1.0004743212134, 1.0004709796105, 1.0004681046295, 1.0004656422063, 1.0004634882855,
    1.0004616158487, 1.0004599661373, 1.0004584950913, 1.0004571936391, 1.0004560284268,
    1.0004549738379, 1.0004540278644, 1.0004531645796, 1.000452384167,  1.0004516712586,
    1.0004510136925, 1.00045041322,   1.0004498558989, 1.0004493439316, 1.0004488690745,
    1.0004484246442, 1.0004480130911, 1.0004476284183, 1.0004472656991, 1.0004469273812,
    1.0004466069034, 1.0004463066487};

  // G4double refractiveIndex[] = {1.00045, 1.00045}; // Refractive index for CO2
  // assert(sizeof(refractiveIndex) == sizeof(photonEnergy));

  // I believe the absorption length is this large because the C02 is quite transparent to visible
  // light there is NOTHING in the web which says anything about the absorption length of CO2 in the
  // visible range
  G4double absorption[] = {10 * m, 10 * m}; // Absorption length for CO2
  assert(sizeof(absorption) == sizeof(photonEnergy));

  G4MaterialPropertiesTable *CO2MPT = new G4MaterialPropertiesTable();
  CO2MPT->AddProperty("RINDEX", CO2photonEnergy, CO2refractiveIndex, 27);
  CO2MPT->AddProperty("ABSLENGTH", photonEnergy, absorption, nEntries);

  CO2->SetMaterialPropertiesTable(CO2MPT);

  //***********From Copilot****************************** */
  Al = new G4Element("Aluminum", "Al", 13., 26.98 * g / mole);
  Aluminum = new G4Material("Al", 13, 26.98 * g / mole, 2.70 * g / cm3);
  // Aluminum->AddElement(Al, 1); // Add Aluminum element with 100% composition
  /*

  Aluminum = new G4Material("Aluminum", 2.70*g/cm3, 1);


  */

  // the Winston cone material
  PLA = new G4Material("PLA", 1.25 * g / cm3, 3);
  PLA->AddElement(C, 3);
  PLA->AddElement(G4Element::GetElement("H"), 4);
  PLA->AddElement(O, 2);

  // Define Mylar (C10H8O4)
  G4Element *H = new G4Element("Hydrogen", "H", 1., 1.01 * g / mole);

  G4Material *Mylar = new G4Material("Mylar", 1.39 * g / cm3, 3);
  Mylar->AddElement(C, 10);
  Mylar->AddElement(H, 8);
  Mylar->AddElement(O, 4);

  // Define Aluminized Mylar (assuming a thin layer of aluminum on Mylar)
  G4double density = 1.39 * g / cm3; // Approximate density
  AluminizedMylar = new G4Material("AluminizedMylar", density, 2);
  AluminizedMylar->AddMaterial(Mylar, 0.98); // 98% Mylar
  AluminizedMylar->AddElement(Al, 0.02);     // 2% Aluminum

  // Add optical properties to Aluminized Mylar
  G4double reflectivityAlMylar[] = {0.9, 0.9}; // Reflectivity for Aluminized Mylar
  assert(sizeof(reflectivityAlMylar) == sizeof(photonEnergy));

  MPTAluminizedMylar = new G4MaterialPropertiesTable();
  MPTAluminizedMylar->AddProperty("REFLECTIVITY", photonEnergy, reflectivityAlMylar, nEntries);

  // PMMA C5H8O2 ( Acrylic )
  // -------------

  Acrylic = new G4Material("Acrylic", 1.19 * g / cm3, 3);
  Acrylic->AddElement(C, 5);
  Acrylic->AddElement(H, 8);
  Acrylic->AddElement(O, 2);

  //////////////////////////////////////////////////////////////////
  //               ACRYLIC Optical properties
  // from copilot. I found a more complex parameterization here:
  // https://apc.u-paris.fr/~franco/g4doxy4.10/html/_ultra_detector_construction_8cc_source.html
  // but there is a problem with the order of the elements
  // in the array. The enerygy is not in increasing order.
  //////////////////////////////////////////////////////////////////

  const G4int nAcryEntries = 9;
  // Define photon energy in increasing order
  G4double AcryPhotonEnergy[nAcryEntries] = {1.55 * eV, 1.77 * eV, 2.07 * eV, 2.48 * eV, 3.1 * eV,
                                             3.54 * eV, 4.13 * eV, 4.96 * eV, 6.2 * eV};

  // Define absorption length (in meters) corresponding to the sorted photon energy
  G4double AcryAbsorptionLength[nAcryEntries] = {10.0 * m, 50.0 * m, 100.0 * m, 50.0 * m, 20.0 * m,
                                                 10.0 * m, 1.0 * m,  0.1 * m,   0.01 * m};

  // Refractive index for acrylic
  // G4double AcryrefractiveIndex[nAcryEntries] = {
  //  1.489, 1.490, 1.491, 1.492, 1.493,
  // 1.494, 1.495, 1.496, 1.497, 1.498
  //};

  // Roman suggester to remove the refractive index property
  // but then Cherenkov light is not reflected
  // I set the refractive index to 1.0
  // but with that absorption length, not sure why the acrylic is useful
  G4double AcryRefractiveIndex[nAcryEntries] = {1., 1., 1., 1., 1., 1., 1., 1., 1.};

  // Create material properties table for acrylic
  G4MaterialPropertiesTable *MPT_Acrylic = new G4MaterialPropertiesTable();
  // From Roman, removing the refractive index, remove the Cherenkov light
  MPT_Acrylic->AddProperty("RINDEX", AcryPhotonEnergy, AcryRefractiveIndex, nAcryEntries);
  MPT_Acrylic->AddProperty("ABSLENGTH", AcryPhotonEnergy, AcryAbsorptionLength, nAcryEntries);

  Acrylic->SetMaterialPropertiesTable(MPT_Acrylic);

  // Perhaps Acrylic should be removed. Extra code and probably memory
  // consumption, and maybe useless for the simulation.
  //////////////////////////////////////////////////////////////////

  // Silver
  Ag = new G4Element("Silver", "Ag", 47., 107.87 * g / mole);

  Silver = new G4Material("Silver", 10.49 * g / cm3, 1);
  Silver->AddElement(Ag, 1);

  //*******************From CoPilot****************** */
  // Define optical properties for Silver

  G4double reflectivity[] = {0.95, 0.95}; // Reflectivity for Silver
  assert(sizeof(reflectivity) == sizeof(photonEnergy));

  MPTSilver = new G4MaterialPropertiesTable();
  MPTSilver->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, nEntries);
  //*******************From CoPilot****************** */

  // Define the material for the PMT (e.g., Aluminum)

  G4Material *diskMaterial = G4Material::GetMaterial("Aluminum");

  //*******************From CoPilot****************** */
  // This looks quite similar to the borosilicate glass
  // defintion in the wikipedia page
  // https://en.wikipedia.org/wiki/Borosilicate_glass

  // Define elements
  Si = new G4Element("Silicon", "Si", 14., 28.09 * g / mole);
  B = new G4Element("Boron", "B", 5., 10.81 * g / mole);
  Na = new G4Element("Sodium", "Na", 11., 22.99 * g / mole);

  // Define borosilicate glass material
  BorosilicateGlass = new G4Material("BorosilicateGlass", 2.23 * g / cm3, 5);
  BorosilicateGlass->AddElement(Si, 80.6 * perCent); // Silicon dioxide (SiO₂)
  BorosilicateGlass->AddElement(B, 13.0 * perCent);  // Boron trioxide (B₂O₃)
  BorosilicateGlass->AddElement(Na, 4.0 * perCent);  // Sodium oxide (Na₂O)
  BorosilicateGlass->AddElement(Al, 2.0 * perCent);  // Aluminum oxide (Al₂O₃)
  BorosilicateGlass->AddElement(O, 0.4 * perCent);   // Remaining oxygen

  // Define optical properties for borosilicate glass
  G4double refractiveIndex[] = {1.47, 1.47}; // Refractive index for borosilicate glass
  assert(sizeof(refractiveIndex) == sizeof(photonEnergy));

  // THIS IS AN ARTIFICIAL VALUE TO KILL THE PHOTON IN THE MATERIAL
  G4double absorptionLength[] = {10 * um, 10 * um}; // Absorption length for borosilicate glass
  assert(sizeof(absorptionLength) == sizeof(photonEnergy));

  G4MaterialPropertiesTable *MPTBorosilicateGlass = new G4MaterialPropertiesTable();
  MPTBorosilicateGlass->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
  MPTBorosilicateGlass->AddProperty("ABSLENGTH", photonEnergy, absorptionLength, nEntries);

  // These numbers are taken from a plot showing a characteristic curve for bialkaly photocathodes
  // I didn't find the original for the ET 4312KB. 40 points

  // For the moment I add the QE propeties of the photocathode to the borosilicate glass
  // From Roman code, MAYBE reflectivity should be added to the MPTBorosilicateGlass

  const int num = 40;

  G4double en_photonCathode[num] = {
    1.78 * eV, 1.8 * eV,  1.83 * eV, 1.86 * eV, 1.89 * eV, 1.91 * eV, 1.95 * eV, 1.97 * eV,
    2.01 * eV, 2.04 * eV, 2.07 * eV, 2.11 * eV, 2.14 * eV, 2.18 * eV, 2.22 * eV, 2.26 * eV,
    2.3 * eV,  2.34 * eV, 2.39 * eV, 2.44 * eV, 2.49 * eV, 2.54 * eV, 2.59 * eV, 2.65 * eV,
    2.7 * eV,  2.76 * eV, 2.83 * eV, 2.89 * eV, 2.96 * eV, 3.03 * eV, 3.1 * eV,  3.18 * eV,
    3.27 * eV, 3.35 * eV, 3.44 * eV, 3.53 * eV, 3.64 * eV, 3.76 * eV, 3.87 * eV, 3.99 * eV};

  G4double qe_photonCathode[num] = {0.632,  0.402,  0.46,   0.46,   0.402,  0.402,  0.402,  0.632,
                                    0.805,  1.207,  1.667,  2.299,  2.931,  3.621,  4.483,  5.345,
                                    6.379,  8.046,  10.747, 13.103, 14.368, 15.057, 16.207, 17.069,
                                    18.563, 19.885, 21.149, 21.954, 22.701, 23.391, 23.908, 24.023,
                                    23.448, 23.103, 21.954, 18.563, 12.471, 3.563,  0.92,   0.517};

  // from Roman https://github.com/rdom/crdirc/blob/master/src/PrtStackingAction.cxx#L94
  // [200,850] nm with 2 nm step
  // quantum efficiency data from Alex Britting, Jan 25, 2011
  // unit is percent
  // first value is at 200 nm, last at 700 nm
  // credible range start around 250nm, >= 280nm to be safe

  double eff_9002224[326] = {
    2.21, -24.6, -27,   -10.8, -6.2,  -8.2, -14.8, -19.9, -14.1, -14,  -22.4, -21.9, -13.8, -16.1,
    -7.3, -9.26, -7.43, -4.04, -0.49, 2.86, 4.55,  5.34,  10.1,  8.51, 8.35,  11,    11.6,  12.9,
    12.6, 13.1,  13.8,  13.7,  13.8,  13.7, 13.9,  13.8,  14.1,  14.6, 15,    15.4,  15.8,  16.2,
    16.4, 16.8,  17.1,  17.3,  17.6,  17.6, 17.9,  18,    18,    18.1, 18.2,  18.3,  18.3,  18.4,
    18.4, 18.4,  18.4,  18.5,  18.7,  18.7, 18.9,  19.1,  19.3,  19.5, 19.6,  19.7,  19.7,  19.8,
    19.8, 19.8,  19.6,  19.6,  19.6,  19.6, 19.5,  19.5,  19.5,  19.6, 19.7,  19.6,  19.8,  19.9,
    20.1, 20.3,  20.3,  20.6,  20.7,  20.8, 20.9,  21,    21,    21,   21,    20.9,  20.9,  20.9,
    20.8, 20.6,  20.4,  20.4,  20.2,  20.1, 19.9,  19.7,  19.8,  19.6, 19.4,  19.4,  19.2,  19,
    18.8, 18.6,  18.5,  18.3,  18.1,  18.1, 17.9,  17.7,  17.5,  17.4, 17.2,  17.1,  16.9,  16.7,
    16.5, 16.4,  15.9,  15.7,  15.1,  14.6, 14.7,  14.4,  14.3,  14,   13.6,  13.3,  13.1,  12.9,
    12.8, 12.5,  12.2,  12,    11.9,  11.7, 11.5,  11.3,  11.2,  11.1, 10.9,  10.8,  10.7,  10.5,
    10.3, 10,    9.73,  9.33,  8.86,  8.34, 7.86,  7.27,  6.91,  6.48, 6.01,  5.85,  5.32,  5.04,
    4.76, 4.56,  4.36,  4.17,  3.98,  3.82, 3.69,  3.54,  3.41,  3.27, 3.16,  3.03,  2.92,  2.8,
    2.7,  2.59,  2.49,  2.4,   2.3,   2.2,  2.11,  2.03,  1.94,  1.85, 1.76,  1.68,  1.61,  1.52,
    1.46, 1.37,  1.29,  1.24,  1.17,  1.11, 1.05,  0.99,  0.94,  0.89, 0.83,  0.79,  0.76,  0.7,
    0.66, 0.63,  0.6,   0.56,  0.53,  0.51, 0.48,  0.46,  0.44,  0.42, 0.4,   0.4,   0.4,   0.38,
    0.37, 0.36,  0.37,  0.35,  0.34,  0.35, 0.35,  0.35,  0.36,  0.35, 0.33,  0.31,  0.3,   0.28,
    0.27, 0.26,  0.25,  0.23,  0.22,  0.21, 0.2,   0.18,  0.18,  0.17, 0.17,  0.14,  0.16,  0.16,
    0.16, 0.16,  0.16,  0.16,  0.16,  0.16, 0.16,  0.15,  0.15,  0.15, 0.16,  0.16,  0.17,  0.17,
    0.17, 0.17,  0.16,  0.15,  0.15,  0.16, 0.16,  0.16,  0.16,  0.17, 0.18,  0.17,  0.16,  0.15,
    0.16, 0.18,  0.18,  0.17,  0.15,  0.08, 0.13,  0.16,  0.17,  0.17, 0.18,  0.18,  0.18,  0.18,
    0.17, 0.17,  0.17,  0.14,  0.14,  0.15, 0.16,  0.15,  0.14,  0.15, 0.16,  0.16,  0.16,  0.15,
    0.14, 0.13,  0.13,  0.11,  0.08,  0,    0.01,  0.03,  0.02,  0.04, 0.07,  0,     0.05,  0.09,
    0.07, 0.1,   0.14,  0.17};

  // Convert the energy from eV to nm
  // The energy is in eV, the wavelength in nm
  for (int i = 0; i < 326; i++) {
    lambda[i] = 200 + i * 2;

    en_phoCath_Roman[325 - i] = 1240 / lambda[i] * eV; // Convert to eV
  }

  MPTBorosilicateGlass->AddProperty("EFFICIENCY", en_phoCath_Roman, eff_9002224, 325);

  BorosilicateGlass->SetMaterialPropertiesTable(MPTBorosilicateGlass);
  //*******************From CoPilot****************** */

  // Print materials
  //  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrtTaggerDetectorConstruction::BuildTagger(G4LogicalVolume *worldLV) {
  DefineMaterials();

  // Geometry parameters

  //
  //  cosmic          z |  /y
  //    |      --->     | /
  //    |     MIRROR    |/_____x   PMT
  //    V
  //
  //

  G4double x_box = 901.1 * mm;
  G4double y_box = 440.0 * mm;
  G4double z_box = 1612.1 * mm;

  // We make a box with the above dimensions and place it at the origin
  // We will then place the Tagger components inside this box

  MainBox = new G4Box("Main_box", // its name
                      x_box / 2, y_box / 2,
                      z_box / 2); // its size

  G4double x_extbox = 703. * mm;
  G4double y_extbox = 433. * mm;
  G4double z_extbox = 451.3 * mm;

  ExtBox = new G4Box("Ext_box", // its name
                     x_extbox / 2, y_extbox / 2,
                     z_extbox / 2); // its size

  Tagger = new G4UnionSolid(
    "Main_box+Ext_box", MainBox, ExtBox, 0,
    G4ThreeVector(x_box / 2 + x_extbox / 2, 0,
                  -z_box / 2 + z_extbox / 2)); //, nullptr, G4ThreeVector(0., 0., 0.));

  TaggerLV = new G4LogicalVolume(Tagger,      // its solid
                                 Aluminum,    // its material
                                 "TaggerLV"); // its name

  // Roman has all the elementw placed wrt the DIRC. The tagger main box is placed at the origin
  G4double dircz = -4000 * mm;
  G4double track1 = dircz + 1300 * mm;

  new G4PVPlacement(nullptr,                                           // no rotation
                    G4ThreeVector(0, 0, track1 + z_box / 2 + 50 * mm), // its position
                    TaggerLV,                                          // its logical volume
                    "Tagger_phy",                                      // its name
                    worldLV,                                           // its mother volume
                    false,                                             // no boolean operation
                    0,                                                 // copy number
                    fCheckOverlaps);                                   // checking overlaps

  G4double AlBoxThick = 3. * mm; // the thickness of the Al box

  // Main: main box, Ext: extension box
  MainGas = new G4Box("Main_Gas", // its name
                      x_box / 2 - AlBoxThick, y_box / 2 - AlBoxThick,
                      z_box / 2 - AlBoxThick); // its size

  ExtGas = new G4Box("Ext_Gas", // its name
                     x_extbox / 2, y_extbox / 2 - AlBoxThick,
                     z_extbox / 2 - AlBoxThick); // its size

  Gas = new G4UnionSolid(
    "Main_Gas+Ext_Gas", MainGas, ExtGas, 0,
    G4ThreeVector(x_box / 2 + x_extbox / 2 - AlBoxThick, 0,
                  -z_box / 2 + z_extbox / 2)); //, nullptr, G4ThreeVector(0., 0., 0.));

  GasLV = new G4LogicalVolume(Gas,      // its solid
                              CO2,      // its material
                              "GasLV"); // its name

  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, 0), // its position
                    GasLV,                  // its logical volume
                    "Gas_phy",              // its name
                    TaggerLV,               // its mother volume
                    false,                  // no boolean operation
                    0,                      // copy number
                    fCheckOverlaps);        // checking overlaps

  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
  // Winston cone construction
  // it consists of a mother volume (WCMother) and two shells (WCShell_1 and WCShell_2)
  // The mother volume is a bit larger than the Winston cone
  // and will containg the PLA skin layer and the GDML-loaded Winston cone

  G4double length = 315.47 * mm;
  G4double OutRadius = 88.91 * mm;
  G4double InRadius = 76.11 * mm;

  // The mother volume is a bit larger than the Winston cone
  // and will containg the PLA skin layer and the GDML-loaded Winston cone
  // I don't need to be very larger than the Winston cone, but I will make it 5um larger

  WCMother = new G4Tubs("WinstonConeMother", // name
                        0., OutRadius + 5 * um,
                        length / 2 + 52 * um, // Z half length
                        0. * deg,             // starting Phi
                        360. * deg);          // segment angle

  WCMotherLV = new G4LogicalVolume(WCMother,      // its solid
                                   CO2,           // its material
                                   "WCMotherLV"); // its name

  G4RotationMatrix *yRot = new G4RotationMatrix();
  yRot->rotateY(90.0 * deg);

  // the 20*cm offset of the WC should be revisited
  G4ThreeVector WCposition(x_box / 2 + length / 2 - 5 * cm, 0,
                           -z_box / 2 +
                             20 * cm); // the origin of the solid is NOT at the longitudinal center

  new G4PVPlacement(yRot,                                     // no rotation
                    WCposition + G4ThreeVector(0, 0, 0 * cm), // its position
                    WCMotherLV,                               // its logical volume
                    "WCMotherphy",                            // its name
                    GasLV,                                    // its mother volume
                    false,                                    // no boolean operation
                    0,                                        // copy number
                    fCheckOverlaps);

  WCMotherLV->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));

  WCShell_1 = new G4Tubs(
    "WCS_1",              // name
    OutRadius,            // inner radius
    OutRadius + 4 * um,   // outer radius +4um to have the real cone inside
    length / 2 + 50 * um, // Z half length. I made it 2mm larger to have the real cone inside
    0. * deg,             // starting Phi
    360. * deg);          // segment angle

  WCShell_2 = new G4Tubs("WC_2",             // name
                         InRadius,           // inner radius
                         OutRadius + 4 * um, // outer radius +4um to have the real cone inside
                         2 * um,             // Z half length
                         0. * deg,           // starting Phi
                         360. * deg);        // segment angle

  WCS1LV = new G4LogicalVolume(WCShell_1, // its solid
                               PLA,       // its material
                               "WCS1LV"); // its name

  WCS2LV = new G4LogicalVolume(WCShell_2, // its solid
                               PLA,       // its material
                               "WCS2LV"); // its name

  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0 * cm), // its position
                    WCS1LV,                               // its logical volume
                    "WCS1phy",                            // its name
                    WCMotherLV,                           // its mother volume
                    false,                                // no boolean operation
                    0,                                    // copy number
                    fCheckOverlaps);

  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, length / 2 + 50 * um), // i ts position
                    WCS2LV,                                             // its logical volume
                    "WCS2phy",                                          // its name
                    WCMotherLV,                                         // its mother volume
                    false,                                              // no boolean operation
                    0,                                                  // copy number
                    fCheckOverlaps);

  //***************************************************************************** */
  // The Winston Cone Solid from a GDML file

  fParser.Read("../data/WinstonCone.gdml", false); // reads and stores
  // in memory

  // Get the top volume from the GDML file
  G4LogicalVolume *gdmlTopVolume = fParser.GetVolume("test_mesh");

  gdmlTopVolume->SetMaterial(
    AluminizedMylar); // The winston cone is made of Aluminized Mylar only in the inner surface
                      // This is why I covered with a G4Tubs made of PLA

  G4OpticalSurface *aluminizedMylarSurface = new G4OpticalSurface("AluminizedMylarSurface");
  aluminizedMylarSurface->SetType(dielectric_metal);
  aluminizedMylarSurface->SetFinish(polished);
  aluminizedMylarSurface->SetModel(unified);
  aluminizedMylarSurface->SetMaterialPropertiesTable(MPTAluminizedMylar);

  new G4LogicalSkinSurface("AluminizedMylarSurface", gdmlTopVolume, aluminizedMylarSurface);

  if (!gdmlTopVolume) {
    G4Exception("DetectorConstruction::Construct()", "InvalidSetup", FatalException,
                "Failed to load GDML top volume. Check GDML file.");
  }

  G4RotationMatrix *antiyRot = new G4RotationMatrix();
  antiyRot->rotateY(-90.0 * deg);

  G4double x_offset = 12.7 * mm; // this is the distance from the origin of the gdml volume to the
                                 // near end of the Winston cone

  new G4PVPlacement(
    antiyRot, // the lonfitudinal axis is on Z-direction
    G4ThreeVector(
      0, 0, -(length / 2 - x_offset)), // the origin of the solid is NOT at the longitudinal center
    gdmlTopVolume,                     // GDML-loaded volume
    "GDML_Imported",                   // Name
    WCMotherLV,                        // Place inside world
    false,                             // No boolean operation
    0,                                 // Copy number
    true);                             // Check overlaps
  //***************************************************************************** */

  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*/
  // Mirror

  // This is the height of the center of the mirror according Alexander design
  G4double dist_ori = 18.73 * cm;

  G4RotationMatrix *MirrorRot = new G4RotationMatrix();
  MirrorRot->rotateY(135.0 * deg);

  G4double kMirrorRad = 40.5 / 2. * cm;
  G4double kFocalLength = 1 * m;
  G4double sphererad = 2 * kFocalLength;

  G4double kMirrorSag = kMirrorRad * kMirrorRad / 4. / kFocalLength;

  // Mirror Surface
  G4Sphere *MirrorSurfPhys = new G4Sphere("MirrorSurfPhys", sphererad - 5 * mm, sphererad, 0. * deg,
                                          360. * deg, 0. * deg, 5.81 * deg);

  MirrorSurfLV = new G4LogicalVolume(MirrorSurfPhys, // its solid
                                     Acrylic,        // its material
                                     "MirrorSurfLV");

  new G4PVPlacement(
    MirrorRot, // the lonfitudinal axis is on Z-direction
    G4ThreeVector(-3 * cm + sphererad * cos(45 * deg), 0,
                  -z_box / 2 + dist_ori +
                    sphererad *
                      cos(45 * deg)), // the origin of the solid is NOT at the longitudinal center
    MirrorSurfLV,
    "MirrorSurf", // Name
    GasLV,        // Place inside world
    false,        // No boolean operation
    0,            // Copy number
    true);        // Check overlaps

  // I calculated the 5.81deg angle by hand, it would be nice to check it
  G4Sphere *MirrorPhys = new G4Sphere("MirrorPhys", sphererad, sphererad + 1 * um, 0. * deg,
                                      360. * deg, 0. * deg, 5.81 * deg);

  MirrorLV = new G4LogicalVolume(MirrorPhys, // its solid
                                 Silver,     // its material
                                 "MirrorLV");

  // Attach the optical surface to the logical volume
  G4OpticalSurface *opticalSurface = new G4OpticalSurface("SilverSurface");
  opticalSurface->SetType(dielectric_metal);
  opticalSurface->SetFinish(polished);
  opticalSurface->SetModel(unified);
  opticalSurface->SetMaterialPropertiesTable(MPTSilver);

  new G4LogicalSkinSurface("SilverSurface", MirrorLV, opticalSurface);

  // the terms sphererad*cos... are due to the way the Sphere is built
  // 3*cm is the correction to have the mirror at the right place
  new G4PVPlacement(
    MirrorRot, // the lonfitudinal axis is on Z-direction
    G4ThreeVector(-3 * cm + sphererad * cos(45 * deg), 0,
                  -z_box / 2 + dist_ori +
                    sphererad *
                      cos(45 * deg)), // the origin of the solid is NOT at the longitudinal center
    MirrorLV,                         // GDML-loaded volume
    "Mirror",                         // Name
    GasLV,                            // Place inside world
    false,                            // No boolean operation
    0,                                // Copy number
    true);                            // Check overlaps
  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*/

  //*-*-*-*-*-*-*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*/
  // A DISK as sensitive detector to emulate the PMT

  // Define the dimensions of the disk
  G4double innerRadius = 0.0 * mm;       // Inner radius of the disk
  G4double outerRadius = 76.2 * mm / 2.; // Outer radius of the disk, 3 inches diameter
  // G4double halfThickness = 7.0 * mm; // Half-thickness (Z half-length) 14mm thickness of the
  // window

  G4double halfThickness =
    12.7 * mm / 2; // Half-thickness (Z half-length), the thickness of the WC groove where the PMT
                   // is placed 14mm thickness of the window
  G4double startAngle = 0.0 * deg;      // Starting angle of the disk
  G4double spanningAngle = 360.0 * deg; // Spanning angle of the disk

  // Create the disk solid
  PMT = new G4Tubs("Disk", innerRadius, outerRadius, halfThickness, startAngle, spanningAngle);

  // Create the logical volume for the disk
  PMTLV = new G4LogicalVolume(PMT, BorosilicateGlass, "PMTLV");

  // Place the disk in the world volume or another mother volume
  G4ThreeVector diskPosition(0, 0, 10.0 * mm); // Position of the disk
  new G4PVPlacement(
    nullptr,                                       // No rotation
    G4ThreeVector(0, 0, -(length / 2 - 12.7 / 2)), // remember the WCmother is rotated wrt the world
    PMTLV, "Disk",
    WCMotherLV, // this is TEMPORARY, it should be, maybe, in the winston cone
    false, 0, true);

  // Visualization attributes

  TaggerLV->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
  GasLV->SetVisAttributes(G4VisAttributes(G4Colour::Blue()));
  WCS1LV->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));
  WCS2LV->SetVisAttributes(G4VisAttributes(G4Colour::White()));
  gdmlTopVolume->SetVisAttributes(G4VisAttributes(G4Colour::Red()));
  MirrorLV->SetVisAttributes(G4VisAttributes(G4Colour::Green()));
  PMTLV->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));
  MirrorSurfLV->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));
  // TaggerLV->SetVisAttributes(G4VisAttributes(G4VisAttributes::GetInvisible() ));
  // GasLV->SetVisAttributes(G4VisAttributes(G4VisAttributes::GetInvisible() ));
  // WCLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  // gdmlTopVolume->SetVisAttributes(G4VisAttributes::GetInvisible());

  //
  // Always return the physical World
  //
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrtTaggerDetectorConstruction::ConstructSDandField() {
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  // G4ThreeVector fieldValue;
  // fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  // fMagFieldMessenger->SetVerboseLevel(1);

  // // Register the field messenger for deleting
  // G4AutoDelete::Register(fMagFieldMessenger);

  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  G4SDManager *SDManager = G4SDManager::GetSDMpointer();
  G4String SDname;
  TaggerPMTSD *TaggerPMT = new TaggerPMTSD("/TaggerPMT");
  SDManager->AddNewDetector(TaggerPMT);

  PMTLV->SetSensitiveDetector(TaggerPMT);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
