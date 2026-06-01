#include "Construction.hh"
#include "detector.hh"
#include "G4SDManager.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include <cmath>
#include <string>

MyDetectorConstruction::MyDetectorConstruction()
    : fScintLogical(nullptr)
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    // ---------------------------------------------------------------
    // Materials
    // ---------------------------------------------------------------
    G4NistManager* nist     = G4NistManager::Instance();
    G4Material*    galactic = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material*    scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    // ---------------------------------------------------------------
    // World volume — cylinder large enough for full modular J-PET
    // Modular layer radius ~38.4 cm + 500 mm bar half-length along Z
    // ---------------------------------------------------------------
    G4double worldR     = 150. * cm;
    G4double worldHalfL = 60.  * cm;
    G4VSolid* worldSolid = new G4Tubs(
        "WorldSolid", 0., worldR, worldHalfL, 0., 360.*deg);
    G4LogicalVolume* worldLogic = new G4LogicalVolume(
        worldSolid, galactic, "WorldLogic");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(
        nullptr, G4ThreeVector(), worldLogic, "WorldPhys",
        nullptr, false, 0, true);

    // ---------------------------------------------------------------
    // Scintillator bar — J-PET strip: 500 mm long, 24 mm wide, 6 mm thick
    // Bars are placed with their LENGTH along Z axis
    // ---------------------------------------------------------------
    G4double hX = 24.  * mm / 2.;   // half-width  (radial direction)
    G4double hY =  6.  * mm / 2.;   // half-depth  (tangential direction)
    G4double hZ = 500. * mm / 2.;   // half-length (along Z beam axis)

    G4Box* scintBox = new G4Box("ScintillatorBox", hX, hY, hZ);
    fScintLogical   = new G4LogicalVolume(scintBox, scintMat, "ScintillatorLV");

    G4VisAttributes* visScint = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
    visScint->SetForceSolid(true);
    fScintLogical->SetVisAttributes(visScint);

    // ---------------------------------------------------------------
    // Modular J-PET: 24 modules arranged in a full ring
    // Each module has 13 bars
    // Radii taken from official J-PET code (DetectorConstants)
    // radius0 = 38.186 cm, angular displacement = 1.04 deg = 0.01815 rad
    // ---------------------------------------------------------------
    const G4double radius_24[13] = {
        38.416, 38.346, 38.289, 38.244, 38.212,
        38.192, 38.186, 38.192, 38.212, 38.244,
        38.289, 38.346, 38.416
    };                                      // all in cm

    const G4double angDisp = 0.01815;       // 1.04 deg in radians
    const G4int    nModules = 24;
    G4int          copyNo   = 201;          // matches official J-PET numbering

    for(G4int i = 0; i < nModules; i++)
    {
        // Azimuthal angle of this module
        G4double phi = i * 2. * M_PI / nModules;

        for(G4int j = -6; j < 7; j++)      // j = -6 .. +6 → 13 bars
        {
            G4double phi1   = phi + j * angDisp;
            G4double radius = radius_24[j + 6] * cm;

            // Bar centre position
            G4ThreeVector loc(
                radius * std::cos(phi1),
                radius * std::sin(phi1),
                0.0);

            // Rotate bar so its face points toward the ring centre
            G4RotationMatrix rot;
            rot.rotateZ(phi);

            G4Transform3D transform(rot, loc);

            new G4PVPlacement(
                transform,
                fScintLogical,
                "ScintillatorPhys_" + std::to_string(copyNo),
                worldLogic,
                true,
                copyNo,
                true);      // check overlaps

            copyNo++;
        }
    }

    // ---------------------------------------------------------------
    // Total bars placed: 24 modules × 13 bars = 312 bars
    // Copy numbers: 201 – 512
    // ---------------------------------------------------------------

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector* scintSD = new MySensitiveDetector("ScintillatorSD");

    // Register with G4SDManager — without this ProcessHits() is never called
    G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);

    // Attach SD to every copy of ScintillatorLV
    if(fScintLogical)
        fScintLogical->SetSensitiveDetector(scintSD);
}
