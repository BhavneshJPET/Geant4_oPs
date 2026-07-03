#include "construction.hh"
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
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"
#include <cmath>
#include <string>

// =========================================================================
// CONFIGURATION TOGGLES
// =========================================================================
bool ENABLE_SCINTILLATORS = true;
bool ENABLE_AL_ABSORBER   = false;
bool ENABLE_GRATING       = true;
bool useSolidCounter      = true;

MyDetectorConstruction::MyDetectorConstruction()
    : fScintLogical(nullptr),
      fAlLogical(nullptr),
      fGratingWallLogical(nullptr),
      fGratingOpeningLogical(nullptr),
      fGratingWallLogical2(nullptr),
      fGratingOpeningLogical2(nullptr),
      fSolidCounterLogical(nullptr)
{}

MyDetectorConstruction::~MyDetectorConstruction() {}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    G4NistManager* nist     = G4NistManager::Instance();
    G4Material* galactic    = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material* scintMat    = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4Material* fSiMaterial = nist->FindOrBuildMaterial("G4_Si");

    // ---------------------------------------------------------------
    // World
    // ---------------------------------------------------------------
    G4VSolid* worldSolid = new G4Tubs("WorldSolid", 0., 150.*cm, 60.*cm, 0., 360.*deg);
    G4LogicalVolume* worldLogic = new G4LogicalVolume(worldSolid, galactic, "WorldLogic");
    worldLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
    G4VPhysicalVolume* physWorld = new G4PVPlacement(
        nullptr, G4ThreeVector(), worldLogic, "WorldPhys", nullptr, false, 0, true);

    // ---------------------------------------------------------------
    // Scintillator bars
    // ---------------------------------------------------------------
    G4Box* scintBox   = new G4Box("ScintillatorBox", 12.*mm, 3.*mm, 250.*mm);
    fScintLogical     = new G4LogicalVolume(scintBox, scintMat, "ScintillatorLV");
    G4VisAttributes* visScint = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
    visScint->SetForceSolid(true);
    fScintLogical->SetVisAttributes(visScint);

    if(ENABLE_SCINTILLATORS)
    {
        const G4double radius_24[13] = {
            38.416,38.346,38.289,38.244,38.212,
            38.192,38.186,38.192,38.212,38.244,
            38.289,38.346,38.416 };
        const G4double angDisp = 0.01815;
        G4int copyNo = 201;
        for(G4int i = 0; i < 24; i++)
        {
            G4double phi = i * 2.*M_PI / 24;
            for(G4int j = -6; j < 7; j++)
            {
                G4double phi1   = phi + j*angDisp;
                G4double radius = radius_24[j+6]*cm;
                G4ThreeVector loc(radius*std::cos(phi1), radius*std::sin(phi1), 0.);
                G4RotationMatrix rot; rot.rotateZ(phi);
                new G4PVPlacement(G4Transform3D(rot,loc), fScintLogical,
                    "ScintillatorPhys_"+std::to_string(copyNo),
                    worldLogic, true, copyNo++, true);
            }
        }
    }

    // ---------------------------------------------------------------
    // Al absorber (validation mode only)
    // ---------------------------------------------------------------
    if(ENABLE_AL_ABSORBER)
    {
        G4Material* aluminium = nist->FindOrBuildMaterial("G4_Al");
        G4Box* alBox = new G4Box("AlAbsorberBox", 1.*cm, 1.*cm, 0.5*cm);
        fAlLogical   = new G4LogicalVolume(alBox, aluminium, "AlAbsorberLV");
        G4VisAttributes* visAl = new G4VisAttributes(G4Colour(0.6,0.6,0.6));
        visAl->SetForceSolid(true);
        fAlLogical->SetVisAttributes(visAl);
        new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-21.1*cm),
            fAlLogical, "AlAbsorberPhys", worldLogic, false, 0, true);
    }

    // ---------------------------------------------------------------
    // TWO-GRATING MOIRÉ DEFLECTOMETER + Si COUNTER (90-deg Rotated)
    // ---------------------------------------------------------------
    if(ENABLE_GRATING)
    {
        const G4double pitch         = 100.0 * micrometer;
        const G4double opening_width =  40.0 * micrometer; 
        const G4double wall_width    = pitch - opening_width;
        const G4double grating_halfX = 3.5 * cm;
        const G4double grating_halfY = 3.5 * cm;
        const G4double grating_halfZ = 50.0 * micrometer;  

        G4UserLimits* limits = new G4UserLimits(4.0 * micrometer);

        const G4double grating1Z        = -21.0 * cm;
        const G4double gratingSeparation =   4.0 * cm;
        const G4double grating2Z        = grating1Z + gratingSeparation; 
        const G4double counterGap       = gratingSeparation;              
        const G4double counter_halfZ    = 50.0 * micrometer;
        const G4double counterZ         = grating2Z + grating_halfZ
                                        + counterGap + counter_halfZ;    

        const G4double grating2_rotation = 20.0 * mrad;

        G4cout << "============================================" << G4endl;
        G4cout << " Grating 1 Z     = " << grating1Z/cm       << " cm" << G4endl;
        G4cout << " Grating 2 Z     = " << grating2Z/cm       << " cm" << G4endl;
        G4cout << " Counter  Z      = " << counterZ/cm         << " cm" << G4endl;
        G4cout << " Fringe period   = " << pitch/grating2_rotation/mm << " mm" << G4endl;
        G4cout << "============================================" << G4endl;

        // -----------------------------------------------------------
        // Helper lambda: Now modified to slice along the X-Axis
        // -----------------------------------------------------------
        auto buildGrating = [&](
            const G4String& tag,
            G4LogicalVolume*& wallLog,
            G4LogicalVolume*& openLog) -> G4LogicalVolume*
        {
            G4Box* motherBox = new G4Box("GratingMotherBox_"+tag,
                grating_halfX, grating_halfY, grating_halfZ);
            G4LogicalVolume* motherLog = new G4LogicalVolume(
                motherBox, galactic, "GratingMotherLog_"+tag);

            // CHANGED: Slicing occurs along X now. Pitch width goes into X parameter.
            G4Box* sliceBox = new G4Box("SliceBox_"+tag,
                pitch/2., grating_halfY, grating_halfZ);
            G4LogicalVolume* sliceLog = new G4LogicalVolume(
                sliceBox, galactic, "SliceLog_"+tag);

            G4int nRep = G4int(2.*grating_halfX / pitch);
            
            // CHANGED: Replicated along kXAxis instead of kYAxis
            new G4PVReplica("GratingReplica_"+tag, sliceLog, motherLog,
                            kXAxis, nRep, pitch);

            // CHANGED: X dimensions configured to match horizontal alignment walls
            G4Box* wallBox = new G4Box("WallBox_"+tag,
                wall_width/2., grating_halfY, grating_halfZ);
            wallLog = new G4LogicalVolume(wallBox, fSiMaterial, "WallLog_"+tag);

            G4Box* openBox = new G4Box("OpeningBox_"+tag,
                opening_width/2., grating_halfY, grating_halfZ);
            openLog = new G4LogicalVolume(openBox, galactic, "OpeningLog_"+tag);

            G4double wallX = pitch/2. - wall_width/2.;
            G4double openX = -pitch/2. + opening_width/2.;

            // CHANGED: Placing internal walls along local X offsets
            new G4PVPlacement(nullptr, G4ThreeVector(wallX,0,0),
                wallLog, "Wall_"+tag, sliceLog, false, 0, true);
            new G4PVPlacement(nullptr, G4ThreeVector(openX,0,0),
                openLog, "Opening_"+tag, sliceLog, false, 0, true);

            motherLog->SetVisAttributes(G4VisAttributes::GetInvisible());
            sliceLog ->SetVisAttributes(G4VisAttributes::GetInvisible());
            G4VisAttributes* visWall = new G4VisAttributes(G4Colour(1.,0.65,0.));
            visWall->SetForceSolid(true);
            wallLog->SetVisAttributes(visWall);
            openLog->SetVisAttributes(G4VisAttributes::GetInvisible());

            wallLog->SetUserLimits(limits);
            openLog->SetUserLimits(limits);

            return motherLog;
        };

        // -----------------------------------------------------------
        // Grating 1 — Reference alignment
        // -----------------------------------------------------------
        G4LogicalVolume* g1Mother = buildGrating("1",
            fGratingWallLogical, fGratingOpeningLogical);
        new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,grating1Z),
            g1Mother, "GratingPhys_1", worldLogic, false, 0, true);

        // -----------------------------------------------------------
        // Grating 2 — Rotated to form Moiré modulation along Y
        // -----------------------------------------------------------
        G4LogicalVolume* g2Mother = buildGrating("2",
            fGratingWallLogical2, fGratingOpeningLogical2);
        G4RotationMatrix* rotG2 = new G4RotationMatrix();
        rotG2->rotateZ(grating2_rotation);
        new G4PVPlacement(rotG2, G4ThreeVector(0.,0.,grating2Z),
            g2Mother, "GratingPhys_2", worldLogic, false, 0, true);

        // -----------------------------------------------------------
        // Solid counter — SILICON
        // -----------------------------------------------------------
        if(useSolidCounter)
        {
            G4Box* counterBox = new G4Box("SolidCounterBox",
                grating_halfX, grating_halfY, counter_halfZ);

            fSolidCounterLogical = new G4LogicalVolume(
                counterBox, fSiMaterial, "SolidCounterLog");

            G4VisAttributes* visCtr = new G4VisAttributes(G4Colour(1.,0.,0.));
            visCtr->SetForceSolid(true);
            fSolidCounterLogical->SetVisAttributes(visCtr);
            fSolidCounterLogical->SetUserLimits(limits);

            new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,counterZ),
                fSolidCounterLogical, "SolidCounterPhys",
                worldLogic, false, 0, true);

            G4cout << " Counter material: Si (pick-off here = fringe pattern)" << G4endl;
            G4cout << "============================================" << G4endl;
        }
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    // Scintillator ring
    MySensitiveDetector* scintSD = new MySensitiveDetector("ScintillatorSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);
    if(fScintLogical) fScintLogical->SetSensitiveDetector(scintSD);

    // Al absorber
    MyAlAbsorberSD* alSD = new MyAlAbsorberSD("AlAbsorberSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(alSD);
    if(fAlLogical) fAlLogical->SetSensitiveDetector(alSD);

    // Grating Si walls
    MyGratingSD* gratingSD = new MyGratingSD("GratingSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(gratingSD);
    if(fGratingWallLogical)  fGratingWallLogical ->SetSensitiveDetector(gratingSD);
    if(fGratingWallLogical2) fGratingWallLogical2->SetSensitiveDetector(gratingSD);

    // Si counter — pick-off here generates the fringe pattern
    MyCounterSD* counterSD = new MyCounterSD("CounterSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(counterSD);
    if(fSolidCounterLogical) fSolidCounterLogical->SetSensitiveDetector(counterSD);
}
