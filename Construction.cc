// =========================================================================
// 3rd GRATING MOIRÉ DEFLECTOMETER
// =========================================================================

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
//   5 independently switchable geometry elements + per-module switches
// =========================================================================
bool ENABLE_SCINTILLATORS = true;    // 6 straight modules (was 24-module ring)
bool ENABLE_GRATING_1     = true;    // reference grating
bool ENABLE_GRATING_2     = true;    // rotated grating (Moire modulation)
bool ENABLE_GRATING_3     = true;    // analyser grating (NEW)
bool ENABLE_STOPPER       = true;    // Si solid counter / stopper
bool ENABLE_AL_ABSORBER   = false;   // validation mode only

// Individual scintillator modules.
//   index 0,1,2 -> +X side (lower, middle, upper)
//   index 3,4,5 -> -X side (lower, middle, upper)
bool ENABLE_MODULE[6] = { true, true, true, true, true, true };

// NOTE: initialiser order follows the declaration order in construction.hh
MyDetectorConstruction::MyDetectorConstruction()
    : fGeoKind(Geo6ModulesStraight),
      fScintLogical(nullptr),
      fAlLogical(nullptr),
      fScinLogInModule(nullptr),
      fScinLogInModuleInner(nullptr),
      fSlitWallLogical(nullptr),
      wLogic(nullptr),
      fGratingWallLogical(nullptr),
      fGratingOpeningLogical(nullptr),
      fGratingWallLogical2(nullptr),
      fGratingOpeningLogical2(nullptr),
      fGratingWallLogical3(nullptr),
      fGratingOpeningLogical3(nullptr),
      fSolidCounterLogical(nullptr),
      physWorld(nullptr)
{}

MyDetectorConstruction::~MyDetectorConstruction() {}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    G4NistManager* nist     = G4NistManager::Instance();
    G4Material* galactic    = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material* scintMat    = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4Material* fSiMaterial = nist->FindOrBuildMaterial("G4_Si");

    // =====================================================================
    // GLOBAL GEOMETRY PARAMETERS
    //   Declared here (not inside the grating block) so that the
    //   scintillator placement and any accessor can use them too.
    // =====================================================================
    const G4double pitch          = 100.0 * micrometer;
    const G4double opening_width  =  40.0 * micrometer;
    const G4double wall_width     = pitch - opening_width;
    const G4double grating_halfX  = 3.5 * cm;
    const G4double grating_halfY  = 3.5 * cm;
    const G4double grating_halfZ  = 50.0 * micrometer;

    const G4double grating1Z         = -27.0 * cm;
    const G4double gratingSeparation =  20.0 * cm;
    const G4double grating2Z         = grating1Z + gratingSeparation;   // -17 cm
    const G4double grating3Z         = grating2Z + gratingSeparation;   // -13 cm

    // Rotations about Z (0 = aligned with grating 1)
    const G4double grating2_rotation = 20.0 * mrad;
    const G4double grating3_rotation =  0.0 * mrad;

    // Stopper sits 2 cm downstream of grating 3 (surface-to-surface)
    const G4double stopperGap    = 2.0 * cm;
    const G4double counter_halfZ = 50.0 * micrometer;
    const G4double counterZ      = grating3Z + grating_halfZ
                                 + stopperGap + counter_halfZ;          // ~ -10.99 cm

    // --- Scintillator module layout ---------------------------------
    // Bars stand VERTICALLY (long axis along Y). The 13 strips of a module
    // are therefore arrayed along Z, and the 3 modules of a side sit next to
    // each other along Z as well.
    const G4int    nStripsPerModule = 13;
    const G4double stripPitchZ      = 6.931 * mm;   // = 0.01815 rad x 38.186 cm
    const G4double moduleWidthZ     = nStripsPerModule * stripPitchZ;   // 90.1 mm
    const G4double moduleGapZ       = 2.0 * mm;
    const G4double modulePitchZ     = moduleWidthZ + moduleGapZ;        // 92.1 mm

    // Perpendicular (transverse) distance of every module from the beam axis.
    const G4double moduleDistX = 10.0 * cm;

    // Longitudinal centre of the modules. Centred on the stopper, i.e. on
    // the annihilation region just behind grating 3.
    //   -> for "10 cm downstream of grating 3 along Z" instead, use:
    //      const G4double moduleCentreZ = grating3Z + 10.0*cm;
    const G4double moduleCentreZ = counterZ;

    // ---------------------------------------------------------------
    // World
    // ---------------------------------------------------------------
    G4VSolid* worldSolid = new G4Tubs("WorldSolid", 0., 150.*cm, 60.*cm, 0., 360.*deg);
    G4LogicalVolume* worldLogic = new G4LogicalVolume(worldSolid, galactic, "WorldLogic");
    worldLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
    wLogic    = worldLogic;                 // member kept for other code paths
    physWorld = new G4PVPlacement(
        nullptr, G4ThreeVector(), worldLogic, "WorldPhys", nullptr, false, 0, true);

    // ---------------------------------------------------------------
    // Scintillator bars: 6 straight-standing modules
    //   Bar long axis along Y -> bars stand vertically.
    //   Bar thin axis along Z (3 mm), depth along X (24 mm) as before.
    //   No rotation at all -> flat, upright panels (no ring curvature).
    //   3 modules at x = +10 cm, 3 modules at x = -10 cm.
    //   The middle module of each side is centred at moduleCentreZ.
    // ---------------------------------------------------------------
    // Y and Z half-lengths swapped w.r.t. the old ring geometry:
    //   old: (12, 3, 250) mm -> bars lay along the beam (horizontal)
    //   new: (12, 250, 3) mm -> bars stand upright
    G4Box* scintBox   = new G4Box("ScintillatorBox", 12.*mm, 250.*mm, 3.*mm);
    fScintLogical     = new G4LogicalVolume(scintBox, scintMat, "ScintillatorLV");
    G4VisAttributes* visScint = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
    visScint->SetForceSolid(true);
    fScintLogical->SetVisAttributes(visScint);

    if(ENABLE_SCINTILLATORS)
    {
        G4int copyNo = 201;   // copy numbers 201..278 (6 x 13 strips)

        for(G4int m = 0; m < 6; m++)
        {
            // Keep the copy-number block reserved even for a disabled module,
            // so that a strip ID always maps to the same physical position.
            if(!ENABLE_MODULE[m]) { copyNo += nStripsPerModule; continue; }

            const G4double xMod = (m < 3 ? +1. : -1.) * moduleDistX;
            const G4int    row  = (m % 3) - 1;   // -1 = upstream, 0 = middle, +1 = downstream
            const G4double zMod = moduleCentreZ + row * modulePitchZ;

            for(G4int j = -6; j < 7; j++)
            {
                G4ThreeVector loc(xMod, 0., zMod + j*stripPitchZ);
                new G4PVPlacement(nullptr, loc, fScintLogical,
                    "ScintillatorPhys_"+std::to_string(copyNo),
                    worldLogic, true, copyNo, true);
                copyNo++;
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
    // THREE-GRATING MOIRE DEFLECTOMETER + Si STOPPER
    // ---------------------------------------------------------------
    G4UserLimits* limits = new G4UserLimits(4.0 * micrometer);

    G4cout << "============================================" << G4endl;
    G4cout << " Grating 1 Z      = " << grating1Z/cm << " cm  ("
           << (ENABLE_GRATING_1 ? "ON" : "OFF") << ")" << G4endl;
    G4cout << " Grating 2 Z      = " << grating2Z/cm << " cm  ("
           << (ENABLE_GRATING_2 ? "ON" : "OFF") << ")" << G4endl;
    G4cout << " Grating 3 Z      = " << grating3Z/cm << " cm  ("
           << (ENABLE_GRATING_3 ? "ON" : "OFF") << ")" << G4endl;
    G4cout << " Stopper  Z       = " << counterZ/cm  << " cm  ("
           << (ENABLE_STOPPER ? "ON" : "OFF") << ")" << G4endl;
    G4cout << " G1-G2 = G2-G3    = " << gratingSeparation/cm << " cm" << G4endl;
    G4cout << " G3-stopper gap   = " << stopperGap/cm        << " cm" << G4endl;
    G4cout << " Fringe period    = " << pitch/grating2_rotation/mm << " mm" << G4endl;
    G4cout << " Modules          = 6 straight, |x| = " << moduleDistX/cm
           << " cm, centred at z = " << moduleCentreZ/cm << " cm" << G4endl;
    G4cout << "============================================" << G4endl;

    // -----------------------------------------------------------
    // Helper lambda: builds one grating, sliced along the X-axis
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

        G4Box* sliceBox = new G4Box("SliceBox_"+tag,
            pitch/2., grating_halfY, grating_halfZ);
        G4LogicalVolume* sliceLog = new G4LogicalVolume(
            sliceBox, galactic, "SliceLog_"+tag);

        G4int nRep = G4int(2.*grating_halfX / pitch);

        new G4PVReplica("GratingReplica_"+tag, sliceLog, motherLog,
                        kXAxis, nRep, pitch);

        G4Box* wallBox = new G4Box("WallBox_"+tag,
            wall_width/2., grating_halfY, grating_halfZ);
        wallLog = new G4LogicalVolume(wallBox, fSiMaterial, "WallLog_"+tag);

        G4Box* openBox = new G4Box("OpeningBox_"+tag,
            opening_width/2., grating_halfY, grating_halfZ);
        openLog = new G4LogicalVolume(openBox, galactic, "OpeningLog_"+tag);

        G4double wallX = pitch/2. - wall_width/2.;
        G4double openX = -pitch/2. + opening_width/2.;

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
    // Grating 1 — reference alignment
    // -----------------------------------------------------------
    if(ENABLE_GRATING_1)
    {
        G4LogicalVolume* g1Mother = buildGrating("1",
            fGratingWallLogical, fGratingOpeningLogical);
        new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,grating1Z),
            g1Mother, "GratingPhys_1", worldLogic, false, 1, true);
    }

    // -----------------------------------------------------------
    // Grating 2 — rotated to form Moire modulation along Y
    // -----------------------------------------------------------
    if(ENABLE_GRATING_2)
    {
        G4LogicalVolume* g2Mother = buildGrating("2",
            fGratingWallLogical2, fGratingOpeningLogical2);
        G4RotationMatrix* rotG2 = new G4RotationMatrix();
        rotG2->rotateZ(grating2_rotation);
        new G4PVPlacement(rotG2, G4ThreeVector(0.,0.,grating2Z),
            g2Mother, "GratingPhys_2", worldLogic, false, 2, true);
    }

    // -----------------------------------------------------------
    // Grating 3 — analyser, same separation as G1-G2
    // -----------------------------------------------------------
    if(ENABLE_GRATING_3)
    {
        G4LogicalVolume* g3Mother = buildGrating("3",
            fGratingWallLogical3, fGratingOpeningLogical3);
        G4RotationMatrix* rotG3 = new G4RotationMatrix();
        rotG3->rotateZ(grating3_rotation);
        new G4PVPlacement(rotG3, G4ThreeVector(0.,0.,grating3Z),
            g3Mother, "GratingPhys_3", worldLogic, false, 3, true);
    }

    // -----------------------------------------------------------
    // Solid counter / stopper — SILICON
    // -----------------------------------------------------------
    if(ENABLE_STOPPER)
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

        G4cout << " Stopper material: Si (pick-off here = fringe pattern)" << G4endl;
        G4cout << "============================================" << G4endl;
    }

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    // Scintillator modules
    MySensitiveDetector* scintSD = new MySensitiveDetector("ScintillatorSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);
    if(fScintLogical) fScintLogical->SetSensitiveDetector(scintSD);

    // Al absorber
    MyAlAbsorberSD* alSD = new MyAlAbsorberSD("AlAbsorberSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(alSD);
    if(fAlLogical) fAlLogical->SetSensitiveDetector(alSD);

    // Grating Si walls (all three gratings share one SD; use the mother
    // copy number 1/2/3 in the touchable history to tell them apart)
    MyGratingSD* gratingSD = new MyGratingSD("GratingSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(gratingSD);
    if(fGratingWallLogical)  fGratingWallLogical ->SetSensitiveDetector(gratingSD);
    if(fGratingWallLogical2) fGratingWallLogical2->SetSensitiveDetector(gratingSD);
    if(fGratingWallLogical3) fGratingWallLogical3->SetSensitiveDetector(gratingSD);

    // Si stopper — pick-off here generates the fringe pattern
    MyCounterSD* counterSD = new MyCounterSD("CounterSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(counterSD);
    if(fSolidCounterLogical) fSolidCounterLogical->SetSensitiveDetector(counterSD);
}
