#include "run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

MyRunAction::MyRunAction()
    : G4UserRunAction()
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->SetDefaultFileType("root");
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->OpenFile("output_142ns.root");

    // ───────────────────────────────────────────────────────────────
    // H1 Histograms (0-21)
    // ───────────────────────────────────────────────────────────────
    man->CreateH1("fEdep", "Total Energy Deposit per Event (keV)", 100, 0., 600.);          // 0
    man->CreateH1("fScinID", "Scintillator ID Hit Distribution", 513, 0., 513.);             // 1
    man->CreateH1("fTotalEnergy", "Total Gamma Energy (keV)", 100, 0., 1500.);               // 2
    man->CreateH1("fOrthops", "Ortho-Positronium Flag (0 or 1)", 2, 0., 2.);                 // 3
    man->CreateH1("fGammaMultiplicity", "Gamma Multiplicity", 10, 0., 10.);                  // 4
    man->CreateH1("fLifetime", "o-Ps Lifetime (ns)", 200, 0., 2000.);                        // 5
    man->CreateH1("fGammaDir_XZ", "Gamma 1 Direction Angle XZ (deg)", 180, -180., 180.);     // 6
    man->CreateH1("fGammaDir_YZ", "Gamma 2 Direction Angle YZ (deg)", 180, -180., 180.);     // 7
    man->CreateH1("fGammaDir_XY", "Gamma 3 Direction Angle XY (deg)", 180, -180., 180.);     // 8
    man->CreateH1("fGamma1_HitZ", "Photon 1 Hit Z Position (cm)", 60, -30., 30.);            // 9
    man->CreateH1("fGamma2_HitZ", "Photon 2 Hit Z Position (cm)", 60, -30., 30.);            // 10
    man->CreateH1("fGamma3_HitZ", "Photon 3 Hit Z Position (cm)", 60, -30., 30.);            // 11
    man->CreateH1("fGamma1_DecayZ", "Photon 1 Decay Z Position (cm)", 60, -30., 30.);        // 12
    man->CreateH1("fGamma2_DecayZ", "Photon 2 Decay Z Position (cm)", 60, -30., 30.);        // 13
    man->CreateH1("fGamma3_DecayZ", "Photon 3 Decay Z Position (cm)", 60, -30., 30.);        // 14
    
    // Trilateration Coordinates & Tracking resolution
    man->CreateH1("fRecoVertexX", "Reconstructed Vertex X (cm)", 100, -5., 5.);             // 15
    man->CreateH1("fRecoVertexY", "Reconstructed Vertex Y (cm)", 100, -5., 5.);             // 16
    man->CreateH1("fRecoVertexZ", "Reconstructed Vertex Z (cm)", 120, -30., 30.);            // 17
    man->CreateH1("fVertexResolution", "Vertex Resolution |RecoV - TrueV| (cm)", 100, 0., 15.);// 18
    
    // Kinematic Hit Angles from Smeared Reconstruction
    man->CreateH1("fHitAngle_a", "Smallest Hit Angle theta_a (deg)", 180, 0., 180.);         // 19
    man->CreateH1("fHitAngle_b", "Middle Hit Angle theta_b (deg)", 180, 0., 180.);           // 20
    man->CreateH1("fHitAngle_c", "Largest Hit Angle theta_c (deg)", 180, 0., 180.);          // 21

    // ───────────────────────────────────────────────────────────────
    // H2 Histograms (0-9)
    // ───────────────────────────────────────────────────────────────
    man->CreateH2("fHitMap_XY", "2D Hit Map X-Y (cm)", 90, -45., 45., 90, -45., 45.);        // 0
    man->CreateH2("fHitMap_YZ", "2D Hit Map Y-Z (cm)", 90, -45., 45., 60, -30., 30.);        // 1
    man->CreateH2("fHitMap_XZ", "2D Hit Map X-Z (cm)", 90, -45., 45., 60, -30., 30.);        // 2
    
    // Phase space pure Monte Carlo generation kinematics 
    man->CreateH2("fAnglePlot", "True MC Kinematics: X=a+b, Y=b-a (deg)", 240, 0., 240., 200, 0., 200.); // 3
    
    man->CreateH2("fGamma1_XZ", "Photon 1 X-Z Hit Map (cm)", 90, -45., 45., 60, -30., 30.); // 4
    man->CreateH2("fGamma2_YZ", "Photon 2 Y-Z Hit Map (cm)", 90, -45., 45., 60, -30., 30.); // 5
    man->CreateH2("fGamma3_XY", "Photon 3 X-Y Hit Map (cm)", 90, -45., 45., 90, -45., 45.); // 6
    
    // Spatial Reco Maps
    man->CreateH2("fRecoVertex_XY", "Reconstructed Vertex X-Y (cm)", 100, -5., 5., 100, -5., 5.);       // 7
    man->CreateH2("fRecoVertex_XZ", "Reconstructed Vertex X-Z (cm)", 100, -5., 5., 120, -30., 30.);     // 8
    
    // Smeared Reconstructed Kinematic Dalitz
    man->CreateH2("fHitAngle", "Reconstructed Hit Angle Dalitz: X=a+b, Y=b-a (deg)", 240, 0., 240., 200, 0., 200.); // 9                                                // 9

    // ---------------------------------------------------------------
    // Ntuples
    // ---------------------------------------------------------------
    man->CreateNtuple("Photons", "Photons");
    man->CreateNtupleIColumn("fEvent");
    man->CreateNtupleDColumn("fE");
    man->FinishNtuple();                                               // 0

    man->CreateNtuple("Hits", "Hits");
    man->CreateNtupleIColumn("fEvent");
    man->CreateNtupleDColumn("fX");
    man->FinishNtuple();                                               // 1

    man->CreateNtuple("Scoring", "Scoring");
    man->CreateNtupleIColumn("fEvent");
    man->CreateNtupleDColumn("fEdep");
    man->CreateNtupleIColumn("fScinID");
    man->CreateNtupleDColumn("fTime");
    man->CreateNtupleDColumn("fDecayX");
    man->CreateNtupleDColumn("fTotalEnergy");
    man->CreateNtupleDColumn("fLifetime");
    man->FinishNtuple();                                               // 2
}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}

void MyRunAction::AddEventData(G4int, G4double, G4double) {}
