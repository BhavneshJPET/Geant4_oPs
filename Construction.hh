#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"          // ADD — needed for grating replicas
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4UserLimits.hh"         // ADD — needed for grating step limits
#include <vector>

class MySensitiveDetector;
class MyAlAbsorberSD;
class MyGratingSD;                 // ADD forward declaration

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    virtual ~MyDetectorConstruction();

    enum GeometryKind {
        Geo1ModuleSimple,
        Geo24ModulesLayer,
        Geo24ModulesLayerDistributed,
        Geo3QuadrantStacked
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

    // NEW — Grating logical volumes
    G4LogicalVolume* fGratingWallLogical;      // Si wall (blocks o-Ps)
    G4LogicalVolume* fGratingOpeningLogical;   // vacuum opening (o-Ps passes)
    G4LogicalVolume* fGratingWallLogical2;
    G4LogicalVolume* fGratingOpeningLogical2;
    G4LogicalVolume* fSolidCounterLogical;     // optional counter after grating

    G4VPhysicalVolume* physWorld;
};
#endif
