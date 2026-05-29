//#ifndef CONSTRUCTION_HH
//#define CONSTRUCTION_HH

//#include "G4VUserDetectorConstruction.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4LogicalVolume.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4PVPlacement.hh"
//#include "G4NistManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4GenericMessenger.hh"
//#include "G4OpticalSurface.hh"
//#include "G4LogicalBorderSurface.hh"
//#include "G4LogicalSkinSurface.hh"

//#include "detector.hh"

//class MyDetectorConstruction : public G4VUserDetectorConstruction
//{
//public:
//    MyDetectorConstruction();
//    ~MyDetectorConstruction();

//    G4LogicalVolume *GetScoringVolume() const { return fScoringVolume; }

//    virtual G4VPhysicalVolume *Construct();
//    void ConstructCherenkov();
//    void ConstructScintillator();
//   // void ConstructTOF();
//    
//private:
//    G4Box *solidWorld, *solidRadiator, *solidDetector, *solidScintillator;
//    G4LogicalVolume *logicWorld, *logicRadiator, *logicDetector, *logicScintillator;
//    G4VPhysicalVolume *physWorld, *physDetector, *physRadiator, *physScintillator;

//    G4Material *SiO2, *H2O, *Aerogel, *worldMat, *NaI;
//    G4Element *C, *Na, *I;

//    void DefineMaterials();
//    virtual void ConstructSDandField();

//    G4GenericMessenger *fMessenger;
//    G4LogicalVolume *fScoringVolume;

//    G4int nRows, nCols;
//    G4double xWorld, yWorld, zWorld;
//   // G4bool isCherenkov, isScintillator, isTOF;
//   // G4OpticalSurface *mirrorSurface;
//  
//};

//#endif



//#ifndef GEOCONSTRUCTION_HH
//#define GEOCONSTRUCTION_HH

//#include "G4VUserDetectorConstruction.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4LogicalVolume.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4PVPlacement.hh"
//#include "G4NistManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4VisAttributes.hh"
//#include "G4Color.hh"

//// Include your Sensitive Detector header
//#include "detector.hh" 

//class MyDetectorConstruction : public G4VUserDetectorConstruction
//{
//public:
//    MyDetectorConstruction();
//    virtual ~MyDetectorConstruction();

//    // Mandatory method
//    virtual G4VPhysicalVolume* Construct();
//    
//    // Method to define sensitive detectors
//    virtual void ConstructSDandField();

//    // Helper to access the scoring volume if needed by other classes
//    G4LogicalVolume* GetScintLogical() const { return fScintLogical; }
//    

//private:
//    // Logical volume for the scintillator bars (needed for SD attachment)
//    G4LogicalVolume* fScintLogical;

//    // Optional: Keep pointers for the world if you need to reference them later
//    G4LogicalVolume* wLogic;
//    G4VPhysicalVolume* physWorld;
//};

//#endif



//// 24
//#ifndef GEOCONSTRUCTION_HH
//#define GEOCONSTRUCTION_HH

//#include "G4VUserDetectorConstruction.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4LogicalVolume.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4PVPlacement.hh"
//#include "G4NistManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4VisAttributes.hh"
//#include "G4Color.hh"
//#include <vector> // Required for the radius vector

//#include "detector.hh" 

//class MyDetectorConstruction : public G4VUserDetectorConstruction
//{
//public:
//    MyDetectorConstruction();
//    virtual ~MyDetectorConstruction();

//    // 1. Define the Geometry Options
//    enum GeometryKind { Geo24ModulesLayer, Geo24ModulesLayerDistributed };

//    virtual G4VPhysicalVolume* Construct();
//    virtual void ConstructSDandField();

//    // 2. Add the Modular Setup Functions
//    void ConstructScintillatorsModularLayer();
//    void ConstructLayers(std::vector<G4double>& radius_dynamic, G4int numberofModules, G4double angDisp_dynamic, G4int& icopyI);

//    G4LogicalVolume* GetScintLogical() const { return fScintLogical; }

//private:
//    // 3. Add the configuration toggle and logical volume for the modules
//    GeometryKind fGeoKind;
//    G4LogicalVolume* fScintLogical; 
//    G4LogicalVolume* fScinLogInModule; // Logical volume specifically for modular bars

//    G4LogicalVolume* wLogic;
//    G4VPhysicalVolume* physWorld;
//};

//#endif


//// 3+3
//#ifndef GEOCONSTRUCTION_HH
//#define GEOCONSTRUCTION_HH

//#include "G4VUserDetectorConstruction.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4LogicalVolume.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4PVPlacement.hh"
//#include "G4NistManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4VisAttributes.hh"
//#include "G4Color.hh"
//#include <vector> 

//#include "detector.hh" 

//class MyDetectorConstruction : public G4VUserDetectorConstruction
//{
//public:
//    MyDetectorConstruction();
//    virtual ~MyDetectorConstruction();

//    // 1. ADDED: Geo3QuadrantStacked to the enum
//    enum GeometryKind { 
//        Geo24ModulesLayer, 
//        Geo24ModulesLayerDistributed,
//        Geo3QuadrantStacked 
//    };

//    virtual G4VPhysicalVolume* Construct();
//    virtual void ConstructSDandField();

//    void ConstructScintillatorsModularLayer();
//    
//    // 2. UPDATED: Signature to include logVol and manualPhi
//    // Change the vector to a simple G4double for the radius
//void ConstructLayers(G4LogicalVolume* logVol, G4double centerRadius, G4int numberofModules, G4double manualPhi, G4int& icopyI);

//    G4LogicalVolume* GetScintLogical() const { return fScintLogical; }

//private:
//    GeometryKind fGeoKind;
//    G4LogicalVolume* fScintLogical; 
//    G4LogicalVolume* fScinLogInModule; 
//    
//    // 3. ADDED: This variable for the second color (Red/Inner)
//    G4LogicalVolume* fScinLogInModuleInner; 

//    G4LogicalVolume* wLogic;
//    G4VPhysicalVolume* physWorld;
//};


//DEEP

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
