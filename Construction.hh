// =========================================================================
// 3rd GRATING MOIRÉ DEFLECTOMETER
// =========================================================================

#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include <vector>

class MySensitiveDetector;
class MyAlAbsorberSD;
class MyGratingSD;
class MyCounterSD;                 // ADD — used in ConstructSDandField

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    virtual ~MyDetectorConstruction();

    enum GeometryKind {
        Geo1ModuleSimple,
        Geo24ModulesLayer,
        Geo24ModulesLayerDistributed,
        Geo3QuadrantStacked,
        Geo6ModulesStraight           // ADD — the new 6-module flat layout
    };

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    G4LogicalVolume* GetScintLogical() const { return fScintLogical; }

private:
    GeometryKind     fGeoKind;

    // Existing logical volumes
    G4LogicalVolume* fScintLogical;
    G4LogicalVolume* fAlLogical;
    G4LogicalVolume* fScinLogInModule;
    G4LogicalVolume* fScinLogInModuleInner;
    G4LogicalVolume* fSlitWallLogical;
    G4LogicalVolume* wLogic;

    // Grating logical volumes
    G4LogicalVolume* fGratingWallLogical;       // grating 1 — Si wall
    G4LogicalVolume* fGratingOpeningLogical;    // grating 1 — vacuum opening
    G4LogicalVolume* fGratingWallLogical2;      // grating 2
    G4LogicalVolume* fGratingOpeningLogical2;
    G4LogicalVolume* fGratingWallLogical3;      // ADD — grating 3 (analyser)
    G4LogicalVolume* fGratingOpeningLogical3;   // ADD
    G4LogicalVolume* fSolidCounterLogical;      // stopper after grating 3

    G4VPhysicalVolume* physWorld;
};
#endif
