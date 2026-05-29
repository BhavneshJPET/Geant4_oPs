#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"        // correct spelling: G4Colour not G4Color
#include "G4SDManager.hh"     // needed for ConstructSDandField
#include <vector>

// Forward declaration — detector.hh is included in construction.cc
// where MySensitiveDetector is actually instantiated
class MySensitiveDetector;

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    virtual ~MyDetectorConstruction();

    // --- Geometry selection ---
    // Keep the enum for future multi-module configurations.
    // Currently only Geo1ModuleSimple is active (13-bar single module).
    enum GeometryKind {
        Geo1ModuleSimple,             // ← active: 1 module, 13 bars at origin
        Geo24ModulesLayer,            // future: full cylindrical layer
        Geo24ModulesLayerDistributed, // future: distributed layer
        Geo3QuadrantStacked           // future: 3-quadrant stacked
    };

    // Mandatory overrides
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    // Accessor so other classes can retrieve the scintillator logical volume
    G4LogicalVolume* GetScintLogical() const { return fScintLogical; }

private:
    GeometryKind     fGeoKind;

    // Logical volumes
    G4LogicalVolume* fScintLogical;        // shared by all 13 bar copies
    G4LogicalVolume* fScinLogInModule;     // reserved for future inner-bar variant
    G4LogicalVolume* fScinLogInModuleInner;// reserved for future inner-bar (red) variant
    G4LogicalVolume* fSlitWallLogical; 
    G4LogicalVolume* wLogic;               // world logical

    // Physical world (stored so ConstructSDandField can access it if needed)
    G4VPhysicalVolume* physWorld;
};

#endif

//SEEK

//#ifndef CONSTRUCTION_HH
//#define CONSTRUCTION_HH

//#include "G4VUserDetectorConstruction.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4LogicalVolume.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4PVPlacement.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4VisAttributes.hh"
//#include "G4GenericMessenger.hh" // For UI commands like /detector/useGravity
//#include <vector>

//class MyDetectorConstruction : public G4VUserDetectorConstruction
//{
//public:
//    MyDetectorConstruction();
//    virtual ~MyDetectorConstruction();

//    virtual G4VPhysicalVolume* Construct();
//    virtual void ConstructSDandField();

//    // Accessors for analysis/stepping
//    G4LogicalVolume* GetScintLogical() const { return fScintLogical; }
//    G4LogicalVolume* GetGratingLogical() const { return fGratingWallLogical; }

//private:
//    // --- Methods ---
//    void DefineMaterials();
//    void DefineCommands();

//    // --- Logical Volumes for the "Gravity Experiment" ---
//    G4LogicalVolume* wLogic;                // World
//    G4LogicalVolume* fScintLogical;         // J-PET Scintillator strips (Blue)
//    G4LogicalVolume* fGratingWallLogical;   // Moiré Grating silicon walls (Orange)
//    G4LogicalVolume* fGratingOpeningLogical;
//    G4LogicalVolume* fSolidCounterLogical;  // Detection end-stop (Red)
//    
//    // --- Materials ---
//    G4Material* Gal_mat;        // Vacuum (Galactic)
//    G4Material* fScinMaterial;  // Plastic Scintillator
//    G4Material* fSiMaterial;    // Silicon for gratings
//    G4Material* mat304steel;    // For vacuum chamber components

//    // --- Geometry Parameters ---
//    std::vector<G4ThreeVector> modulePositions;
//    G4double fGratingAngle;     // For rotating the first grating
//    
//    // --- Messenger for UI Control ---
//    G4GenericMessenger* fMessenger;

//    // --- Toggles (To match colleague's setup) ---
//    G4bool useMoireGratingSetup;
//    G4bool useJPETSetup;
//    G4bool useSolidCounter;
//    G4bool useGravity;
//};

//#endif
