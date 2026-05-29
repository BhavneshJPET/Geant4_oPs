//#include "run.hh"

//MyRunAction::MyRunAction()
//{
//    G4AnalysisManager *man = G4AnalysisManager::Instance();

//    man->CreateH1("fEdep", "Energy Deposition", 100, 0., 0.6*MeV);
//    // ID 1: Name, Title, Bins (one per module), Min ID, Max ID
//    man->CreateH1("fHitMap", "Hits per Scintillator", 13, 0, 13);
//    man->CreateH1("fTotalEnergy", "Sum of 3 Gammas Verification", 100, 0., 1.5*MeV);

//    // Ntuple 0: Photons
//    man->CreateNtuple("Photons", "Photons");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->CreateNtupleDColumn("fY");
//    man->CreateNtupleDColumn("fZ");
//    man->CreateNtupleDColumn("fWlen");
//    man->FinishNtuple(0);

//    // Ntuple 1: Hits
//    man->CreateNtuple("Hits", "Hits");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->CreateNtupleDColumn("fY");
//    man->CreateNtupleDColumn("fZ");
//    man->CreateNtupleDColumn("fTime");
//    man->FinishNtuple(1);

//    // Ntuple 2: Scoring (For your Compton Edge)
//    man->CreateNtuple("Scoring", "Scoring");
//    man->CreateNtupleIColumn("fEvent");    // Column 0
//    man->CreateNtupleDColumn("fEdep");     // Column 1
//    man->CreateNtupleIColumn("fCopyNo");
//    man->CreateNtupleDColumn("fTime");
//    man->CreateNtupleDColumn("DecayX");
//    man->CreateNtupleDColumn("fTotalEnergy");
//    man->FinishNtuple(2);
//}

//// VERY IMPORTANT: The destructor must have a body { }
//MyRunAction::~MyRunAction()
//{}

//void MyRunAction::BeginOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager *man = G4AnalysisManager::Instance();

//    G4int runID = run->GetRunID();
//    std::stringstream strRunID;
//    strRunID << runID;

//    // This creates output0.root, output1.root, etc.
//    man->OpenFile("output" + strRunID.str() + ".root");
//}

//void MyRunAction::EndOfRunAction(const G4Run*)
//{
//    G4AnalysisManager *man = G4AnalysisManager::Instance();

//    man->Write();
//    man->CloseFile();
//}

// DEEP

//#include "run.hh"
//#include "G4AnalysisManager.hh"
//#include "G4SystemOfUnits.hh"

//MyRunAction::MyRunAction()
//    : fTotal3GammaEvents(0),
//      fTotalEnergySum(0.0),
//      fDecayXSum(0.0)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    man->SetVerboseLevel(1);

//    // ---------------------------------------------------------------
//    // Histograms
//    // Index 0 — total energy deposited in scintillators per event
//    man->CreateH1("fEdep",
//                  "Energy Deposition in Scintillators",
//                  150, 0., 600.);

//    // Index 1 — hit map: which bar numbers registered a hit
//    man->CreateH1("fHitMap",
//                  "Hits per Scintillator Bar",
//                  13, -0.5, 12.5);

//    // Index 2 — sum of 3 decay-gamma energies (should peak at 1022 keV)
//    man->CreateH1("fTotalEnergy",
//                  "Sum of 3 Decay Gammas (expect 1022 keV peak)",
//                  150., 0., 1500.);

//    // Index 3 — binary flag: was this event a 3-gamma o-Ps decay?
//    man->CreateH1("fIsOrthoPs",
//                  "Ortho-Ps 3-gamma flag (1=yes, 0=no)",
//                  2, -0.5, 1.5);

//    // Index 4 — gamma multiplicity per event
//    man->CreateH1("fGammaMultiplicity",
//                  "Number of decay gammas per event",
//                  5, -0.5, 4.5);
//                  
//    // Add as Index 5:
//    man->CreateH1("fLifetime",
//              "o-Ps Lifetime Distribution",
//              200, 0., 600.);  // 0 to 600 ns

//    // ---------------------------------------------------------------
//    // Ntuples
//    // Ntuple 0 — Photons (optical / secondary photons if any)
//    man->CreateNtuple("Photons", "Photons");
//    man->CreateNtupleIColumn("fEvent");   // col 0
//    man->CreateNtupleDColumn("fX");       // col 1
//    man->CreateNtupleDColumn("fY");       // col 2
//    man->CreateNtupleDColumn("fZ");       // col 3
//    man->CreateNtupleDColumn("fWlen");    // col 4
//    man->FinishNtuple(0);

//    // Ntuple 1 — Hits (raw hit positions)
//    man->CreateNtuple("Hits", "Hits");
//    man->CreateNtupleIColumn("fEvent");   // col 0
//    man->CreateNtupleDColumn("fX");       // col 1
//    man->CreateNtupleDColumn("fY");       // col 2
//    man->CreateNtupleDColumn("fZ");       // col 3
//    man->CreateNtupleDColumn("fTime");    // col 4
//    man->FinishNtuple(1);

//    // Ntuple 2 — Scoring (per-bar energy & time, one row per hit bar per event)
//    // Column order must match the FillNtupleXColumn calls in event.cc exactly:
//    //   col 0: fEvent       (int)
//    //   col 1: fEdep        (double, MeV)
//    //   col 2: fCopyNo      (int)
//    //   col 3: fTime        (double, ns)
//    //   col 4: DecayX       (double, mm)
//    //   col 5: fTotalEnergy (double, MeV)
//    man->CreateNtuple("Scoring", "Scoring");
//    man->CreateNtupleIColumn("fEvent");        // col 0
//    man->CreateNtupleDColumn("fEdep");         // col 1
//    man->CreateNtupleIColumn("fCopyNo");       // col 2
//    man->CreateNtupleDColumn("fTime");         // col 3
//    man->CreateNtupleDColumn("DecayX");        // col 4
//    man->CreateNtupleDColumn("fTotalEnergy");  // col 5
//    man->CreateNtupleDColumn("fLifetime");     // col 6
//    man->FinishNtuple(2);
//}

//MyRunAction::~MyRunAction() {}

//// ---------------------------------------------------------------------------
//void MyRunAction::BeginOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();

//    // Open a separate output file per run
//    std::ostringstream ss;
//    ss << "output" << run->GetRunID() << ".root";
//    man->OpenFile(ss.str());

//    // Reset accumulators
//    fTotal3GammaEvents = 0;
//    fTotalEnergySum    = 0.0;
//    fDecayXSum         = 0.0;

//    G4cout << "\n[RunAction] Output file: " << ss.str() << G4endl;
//}

//// ---------------------------------------------------------------------------
//void MyRunAction::EndOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();

//    G4cout << "\n========================================" << G4endl;
//    G4cout << "     J-PET Ortho-Ps Beam Summary        " << G4endl;
//    G4cout << "========================================" << G4endl;
//    G4cout << "  Total events run      : " << run->GetNumberOfEvent() << G4endl;
//    G4cout << "  3-gamma (o-Ps) events : " << fTotal3GammaEvents << G4endl;

//    if(fTotal3GammaEvents > 0) {
//        G4double avgE = (fTotalEnergySum / fTotal3GammaEvents) / keV;
//        G4double avgX = (fDecayXSum      / fTotal3GammaEvents) / cm;
//        G4cout << "  Avg 3-gamma sum energy: " << avgE
//               << " keV  (expect 1022 keV)" << G4endl;
//        G4cout << "  Avg decay X position  : " << avgX << " cm" << G4endl;
//    } else {
//        G4cout << "  [WARN] No 3-gamma events recorded." << G4endl;
//        G4cout << "         Check stepping.cc gamma capture and" << G4endl;
//        G4cout << "         physics.cc o-Ps particle definition." << G4endl;
//    }

//    G4cout << "========================================\n" << G4endl;

//    man->Write();
//    man->CloseFile();
//}

//// ---------------------------------------------------------------------------
//// Called by EventAction::EndOfEventAction for every event.
//// Only accumulates statistics for genuine 3-gamma o-Ps decays.
//// ---------------------------------------------------------------------------
//void MyRunAction::AddEventData(G4int gammaCount,
//                                G4double totalEnergy,
//                                G4double decayX)
//{
//    if(gammaCount == 3) {
//        fTotal3GammaEvents++;
//        fTotalEnergySum += totalEnergy;
//        fDecayXSum      += decayX;
//    }
//}


//#include "run.hh"
//#include "G4AnalysisManager.hh"
//#include "G4SystemOfUnits.hh"

//MyRunAction::MyRunAction()
//    : fTotal3GammaEvents(0),
//      fTotalEnergySum(0.0),
//      fDecayXSum(0.0)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    man->SetVerboseLevel(1);

//    // ---------------------------------------------------------------
//    // H1 Histograms
//    // ---------------------------------------------------------------

//    // Index 0 — total energy deposited in scintillators per event (keV)
//    man->CreateH1("fEdep",
//                  "Energy Deposition in Scintillators",
//                  100, 0., 600.);

//    // Index 1 — hit map: bar copy numbers 201-512
//    man->CreateH1("fHitMap",
//                  "Hits per Scintillator Bar",
//                  312, 200.5, 512.5);

//    // Index 2 — sum of 3-gamma energies (keV, peak at 1022 keV)
//    man->CreateH1("fTotalEnergy",
//                  "Sum of 3 Decay Gammas (expect 1022 keV peak)",
//                  150, 0., 1500.);

//    // Index 3 — binary flag: 3-gamma o-Ps event?
//    man->CreateH1("fIsOrthoPs",
//                  "Ortho-Ps 3-gamma flag (1=yes, 0=no)",
//                  2, -0.5, 1.5);

//    // Index 4 — gamma multiplicity per event
//    man->CreateH1("fGammaMultiplicity",
//                  "Number of decay gammas per event",
//                  5, -0.5, 4.5);

//    // Index 5 — o-Ps lifetime distribution (ns)
//    man->CreateH1("fLifetime",
//                  "o-Ps Lifetime Distribution",
//                  500, 0., 1600.);

//    // ---------------------------------------------------------------
//    // H1(6,7,8) — Gamma direction angle histograms
//    // ---------------------------------------------------------------

//    // Index 6 — Gamma 1: azimuthal angle in X-Z plane (deg)
//    man->CreateH1("fGammaDir_XZ",
//                  "Gamma 1 Direction Angle in X-Z Plane (deg)",
//                  180, -180., 180.);

//    // Index 7 — Gamma 2: azimuthal angle in Y-Z plane (deg)
//    man->CreateH1("fGammaDir_YZ",
//                  "Gamma 2 Direction Angle in Y-Z Plane (deg)",
//                  180, -180., 180.);

//    // Index 8 — Gamma 3: azimuthal angle in X-Y plane (deg)
//    man->CreateH1("fGammaDir_XY",
//                  "Gamma 3 Direction Angle in X-Y Plane (deg)",
//                  180, -180., 180.);

//    // ---------------------------------------------------------------
//    // NEW: H1(9,10,11) — Z hit position along bar length per photon
//    // These should NOT be flat — shaped by o-Ps decay position
//    // which depends on lifetime τ = 142 ns and beam direction (+Z)
//    // Early decays hit near z = -25 cm, late decays near z = +25 cm
//    // ---------------------------------------------------------------

//    // Index 9 — Photon 1: Z hit position along bar (cm)
//    man->CreateH1("fGamma1_HitZ",
//                  "Photon 1 Hit Position along Bar Length Z (cm)",
//                  120, -60., 60.);

//    // Index 10 — Photon 2: Z hit position along bar (cm)
//    man->CreateH1("fGamma2_HitZ",
//                  "Photon 2 Hit Position along Bar Length Z (cm)",
//                  120, -60., 60.);

//    // Index 11 — Photon 3: Z hit position along bar (cm)
//    man->CreateH1("fGamma3_HitZ",
//                  "Photon 3 Hit Position along Bar Length Z (cm)",
//                  120, -60., 60.);

//    // ---------------------------------------------------------------
//    // H2 Histograms
//    // ---------------------------------------------------------------

//    // Index H2(0) — Hit map: X vs Y (transverse plane, cm)
//    man->CreateH2("fHitMap_XY",
//                  "Hit Map X-Y (cm)",
//                  100, -50., 50.,
//                  100, -50., 50.);

//    // Index H2(1) — Hit map: Y vs Z (longitudinal plane, cm)
//    man->CreateH2("fHitMap_YZ",
//                  "Hit Map Y-Z (cm)",
//                  100, -50., 50.,
//                  120, -60., 60.);

//    // Index H2(2) — Hit map: X vs Z (longitudinal plane, cm)
//    man->CreateH2("fHitMap_XZ",
//                  "Hit Map X-Z (cm)",
//                  100, -50., 50.,
//                  120, -60., 60.);

//    // Index H2(3) — Kinematics plot
//    // X = theta_a + theta_b, Y = theta_b - theta_a (deg)
//    man->CreateH2("fAnglePlot",
//                  "o-Ps Kinematics: X=theta_a+theta_b, Y=theta_b-theta_a (deg)",
//                  360, 0., 360.,
//                  180, 0., 180.);

//    // ---------------------------------------------------------------
//    // Ntuples
//    // ---------------------------------------------------------------

//    // Ntuple 0 — Photons
//    man->CreateNtuple("Photons", "Photons");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->CreateNtupleDColumn("fY");
//    man->CreateNtupleDColumn("fZ");
//    man->CreateNtupleDColumn("fWlen");
//    man->FinishNtuple(0);

//    // Ntuple 1 — Hits
//    man->CreateNtuple("Hits", "Hits");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->CreateNtupleDColumn("fY");
//    man->CreateNtupleDColumn("fZ");
//    man->CreateNtupleDColumn("fTime");
//    man->FinishNtuple(1);

//    // Ntuple 2 — Scoring (per-bar energy & time)
//    man->CreateNtuple("Scoring", "Scoring");
//    man->CreateNtupleIColumn("fEvent");        // col 0
//    man->CreateNtupleDColumn("fEdep");         // col 1 — keV
//    man->CreateNtupleIColumn("fCopyNo");       // col 2 — bar ID
//    man->CreateNtupleDColumn("fTime");         // col 3 — ns
//    man->CreateNtupleDColumn("DecayX");        // col 4 — mm
//    man->CreateNtupleDColumn("fTotalEnergy");  // col 5 — keV
//    man->CreateNtupleDColumn("fLifetime");     // col 6 — ns
//    man->FinishNtuple(2);
//}

//MyRunAction::~MyRunAction() {}

//// ---------------------------------------------------------------------------
//void MyRunAction::BeginOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();

//    std::ostringstream ss;
//    ss << "output" << run->GetRunID() << ".root";
//    man->OpenFile(ss.str());

//    fTotal3GammaEvents = 0;
//    fTotalEnergySum    = 0.0;
//    fDecayXSum         = 0.0;

//    G4cout << "\n[RunAction] Output file: " << ss.str() << G4endl;
//}

//// ---------------------------------------------------------------------------
//void MyRunAction::EndOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();

//    G4cout << "\n========================================" << G4endl;
//    G4cout << "     J-PET Ortho-Ps Beam Summary        " << G4endl;
//    G4cout << "========================================" << G4endl;
//    G4cout << "  Total events run      : "
//           << run->GetNumberOfEvent()  << G4endl;
//    G4cout << "  3-gamma (o-Ps) events : "
//           << fTotal3GammaEvents << G4endl;

//    if(fTotal3GammaEvents > 0) {
//        G4double avgE = (fTotalEnergySum / fTotal3GammaEvents) / keV;
//        G4double avgX = (fDecayXSum      / fTotal3GammaEvents) / cm;
//        G4cout << "  Avg 3-gamma sum energy: " << avgE
//               << " keV  (expect 1022 keV)" << G4endl;
//        G4cout << "  Avg decay X position  : " << avgX
//               << " cm" << G4endl;
//    } else {
//        G4cout << "  [WARN] No 3-gamma events recorded." << G4endl;
//        G4cout << "         Check stepping.cc gamma capture and"
//               << G4endl;
//        G4cout << "         physics.cc o-Ps particle definition."
//               << G4endl;
//    }

//    G4cout << "========================================\n" << G4endl;

//    man->Write();
//    man->CloseFile();
//}

//// ---------------------------------------------------------------------------
//void MyRunAction::AddEventData(G4int    gammaCount,
//                               G4double totalEnergy,
//                               G4double decayX)
//{
//    if(gammaCount == 3) {
//        fTotal3GammaEvents++;
//        fTotalEnergySum += totalEnergy;
//        fDecayXSum      += decayX;
//    }
//}



//#include "run.hh"
//#include "G4AnalysisManager.hh"
//#include "G4SystemOfUnits.hh"

//MyRunAction::MyRunAction()
//    : fTotal3GammaEvents(0),
//      fTotalEnergySum(0.0),
//      fDecayXSum(0.0)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    man->SetVerboseLevel(1);

//    // ---------------------------------------------------------------
//    // H1 Histograms
//    // ---------------------------------------------------------------

//    man->CreateH1("fEdep",
//                  "Energy Deposition in Scintillators",
//                  100, 0., 600.);                    // 0

//    man->CreateH1("fHitMap",
//                  "Hits per Scintillator Bar",
//                  312, 200.5, 512.5);                // 1

//    man->CreateH1("fTotalEnergy",
//                  "Sum of 3 Decay Gammas (expect 1022 keV)",
//                  150, 0., 1500.);                   // 2

//    man->CreateH1("fIsOrthoPs",
//                  "Ortho-Ps 3-gamma flag (1=yes, 0=no)",
//                  2, -0.5, 1.5);                     // 3

//    man->CreateH1("fGammaMultiplicity",
//                  "Number of decay gammas per event",
//                  5, -0.5, 4.5);                     // 4

//    man->CreateH1("fLifetime",
//                  "o-Ps Lifetime Distribution",
//                  500, 0., 1600.);                   // 5

//    // Gamma direction angles
//    man->CreateH1("fGammaDir_XZ",
//                  "Gamma 1 Direction Angle in X-Z Plane (deg)",
//                  180, -180., 180.);                 // 6

//    man->CreateH1("fGammaDir_YZ",
//                  "Gamma 2 Direction Angle in Y-Z Plane (deg)",
//                  180, -180., 180.);                 // 7

//    man->CreateH1("fGammaDir_XY",
//                  "Gamma 3 Direction Angle in X-Y Plane (deg)",
//                  180, -180., 180.);                 // 8

//    // ---------------------------------------------------------------
//    // H1(9,10,11) — Hit Z per photon
//    // Where each gamma HITS detector along bar length
//    // Non-flat distribution proves lifetime effect on hit positions
//    // ---------------------------------------------------------------
//    man->CreateH1("fGamma1_HitZ",
//                  "Photon 1: Hit Z Position along Bar (cm)",
//                  100, -30., 30.);                   // 9

//    man->CreateH1("fGamma2_HitZ",
//                  "Photon 2: Hit Z Position along Bar (cm)",
//                  100, -30., 30.);                   // 10

//    man->CreateH1("fGamma3_HitZ",
//                  "Photon 3: Hit Z Position along Bar (cm)",
//                  100, -30., 30.);                   // 11

//    // ---------------------------------------------------------------
//    // H1(12,13,14) — Decay Z per photon
//    // Where o-Ps DECAYED along beam — all 3 same point
//    // Exponential shape directly proves tau = 142 ns!
//    // ---------------------------------------------------------------
//    man->CreateH1("fGamma1_DecayZ",
//                  "Photon 1: o-Ps Decay Z Position (cm)",
//                  100, -30., 30.);                   // 12

//    man->CreateH1("fGamma2_DecayZ",
//                  "Photon 2: o-Ps Decay Z Position (cm)",
//                  100, -30., 30.);                   // 13

//    man->CreateH1("fGamma3_DecayZ",
//                  "Photon 3: o-Ps Decay Z Position (cm)",
//                  100, -30., 30.);                   // 14

//    // ---------------------------------------------------------------
//    // H2 Histograms
//    // ---------------------------------------------------------------

//    man->CreateH2("fHitMap_XY",
//                  "Hit Map X-Y (cm)",
//                  100, -50., 50.,
//                  100, -50., 50.);                   // H2: 0

//    man->CreateH2("fHitMap_YZ",
//                  "Hit Map Y-Z (cm)",
//                  100, -50., 50.,
//                  100, -60., 60.);                   // H2: 1

//    man->CreateH2("fHitMap_XZ",
//                  "Hit Map X-Z (cm)",
//                  100, -50., 50.,
//                  100, -60., 60.);                   // H2: 2

//    man->CreateH2("fAnglePlot",
//                  "o-Ps Kinematics: X=theta_a+theta_b, Y=theta_b-theta_a (deg)",
//                  360, 0., 360.,
//                  180, 0., 180.);                    // H2: 3

//    man->CreateH2("fGamma1_XZ",
//                  "Photon 1 Hit: X vs Z (cm)",
//                  100, -50., 50.,
//                  100, -60., 60.);                   // H2: 4

//    man->CreateH2("fGamma2_YZ",
//                  "Photon 2 Hit: Y vs Z (cm)",
//                  100, -50., 50.,
//                  100, -60., 60.);                   // H2: 5

//    man->CreateH2("fGamma3_XY",
//                  "Photon 3 Hit: X vs Y (cm)",
//                  100, -50., 50.,
//                  100, -50., 50.);                   // H2: 6

//    // ---------------------------------------------------------------
//    // Ntuples
//    // ---------------------------------------------------------------
//    man->CreateNtuple("Photons", "Photons");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->CreateNtupleDColumn("fY");
//    man->CreateNtupleDColumn("fZ");
//    man->CreateNtupleDColumn("fWlen");
//    man->FinishNtuple(0);

//    man->CreateNtuple("Hits", "Hits");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->CreateNtupleDColumn("fY");
//    man->CreateNtupleDColumn("fZ");
//    man->CreateNtupleDColumn("fTime");
//    man->FinishNtuple(1);

//    man->CreateNtuple("Scoring", "Scoring");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fEdep");
//    man->CreateNtupleIColumn("fCopyNo");
//    man->CreateNtupleDColumn("fTime");
//    man->CreateNtupleDColumn("DecayX");
//    man->CreateNtupleDColumn("fTotalEnergy");
//    man->CreateNtupleDColumn("fLifetime");
//    man->FinishNtuple(2);
//}

//MyRunAction::~MyRunAction() {}

//void MyRunAction::BeginOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    std::ostringstream ss;
//    ss << "output" << run->GetRunID() << ".root";
//    man->OpenFile(ss.str());
//    fTotal3GammaEvents = 0;
//    fTotalEnergySum    = 0.0;
//    fDecayXSum         = 0.0;
//    G4cout << "\n[RunAction] Output file: " << ss.str() << G4endl;
//}

//void MyRunAction::EndOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    G4cout << "\n========================================" << G4endl;
//    G4cout << "     J-PET Ortho-Ps Beam Summary        " << G4endl;
//    G4cout << "========================================" << G4endl;
//    G4cout << "  Total events run      : "
//           << run->GetNumberOfEvent() << G4endl;
//    G4cout << "  3-gamma (o-Ps) events : "
//           << fTotal3GammaEvents << G4endl;
//    if(fTotal3GammaEvents > 0) {
//        G4cout << "  Avg 3-gamma sum energy: "
//               << (fTotalEnergySum/fTotal3GammaEvents)/keV
//               << " keV  (expect 1022 keV)" << G4endl;
//        G4cout << "  Avg decay X position  : "
//               << (fDecayXSum/fTotal3GammaEvents)/cm
//               << " cm" << G4endl;
//    }
//    G4cout << "========================================\n" << G4endl;
//    man->Write();
//    man->CloseFile();
//}

//void MyRunAction::AddEventData(G4int    gammaCount,
//                               G4double totalEnergy,
//                               G4double decayX)
//{
//    if(gammaCount == 3) {
//        fTotal3GammaEvents++;
//        fTotalEnergySum += totalEnergy;
//        fDecayXSum      += decayX;
//    }
//}


//#include "run.hh"
//#include "G4AnalysisManager.hh"
//#include "G4SystemOfUnits.hh"

//MyRunAction::MyRunAction()
//    : G4UserRunAction()
//{
//    // Initialize your analysis manager
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    man->SetDefaultFileType("root");
//}

//MyRunAction::~MyRunAction() {}

//void MyRunAction::BeginOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();

//    // Open the output file
//    man->OpenFile("output0.root");

//    // ---------------------------------------------------------------
//    // Existing Histograms H1 (0 to 14)
//    // ---------------------------------------------------------------
//    man->CreateH1("fEdep", "Total Energy Deposit per Event (keV)", 100, 0., 2000.); // 0
//    man->CreateH1("fScinID", "Scintillator ID Hit Distribution", 513, 0., 513.);    // 1
//    man->CreateH1("fTotalEnergy", "Total Gamma Energy (keV)", 100, 0., 1500.);      // 2
//    man->CreateH1("fOrthops", "Ortho-Positronium Flag (0 or 1)", 2, 0., 2.);       // 3
//    man->CreateH1("fGammaMultiplicity", "Gamma Multiplicity", 10, 0., 10.);         // 4
//    man->CreateH1("fLifetime", "o-Ps Lifetime (ns)", 100, 0., 500.);               // 5
//    man->CreateH1("fGammaDir_XZ", "Gamma 1 Direction Angle XZ (deg)", 180, -180., 180.); // 6
//    man->CreateH1("fGammaDir_YZ", "Gamma 2 Direction Angle YZ (deg)", 180, -180., 180.); // 7
//    man->CreateH1("fGammaDir_XY", "Gamma 3 Direction Angle XY (deg)", 180, -180., 180.); // 8
//    man->CreateH1("fGamma1_HitZ", "Photon 1 Hit Z Position (cm)", 100, -30., 30.); // 9
//    man->CreateH1("fGamma2_HitZ", "Photon 2 Hit Z Position (cm)", 100, -30., 30.); // 10
//    man->CreateH1("fGamma3_HitZ", "Photon 3 Hit Z Position (cm)", 100, -30., 30.); // 11
//    man->CreateH1("fGamma1_DecayZ", "Photon 1 Decay Z Position (cm)", 100, -30., 30.); // 12
//    man->CreateH1("fGamma2_DecayZ", "Photon 2 Decay Z Position (cm)", 100, -30., 30.); // 13
//    man->CreateH1("fGamma3_DecayZ", "Photon 3 Decay Z Position (cm)", 100, -30., 30.); // 14

//    // ---------------------------------------------------------------
//    // NEW: Reconstructed Vertex Histograms H1 (15 to 18)
//    // ---------------------------------------------------------------
//    man->CreateH1("fRecoVertexX", "Reconstructed Vertex X (cm)", 100, -5., 5.);          // 15
//    man->CreateH1("fRecoVertexY", "Reconstructed Vertex Y (cm)", 100, -5., 5.);          // 16
//    man->CreateH1("fRecoVertexZ", "Reconstructed Vertex Z (cm)", 100, -30., 30.);        // 17
//    man->CreateH1("fVertexResolution", "Vertex Resolution |RecoV - TrueV| (cm)", 100, 0., 10.); // 18

//    // ---------------------------------------------------------------
//    // Existing Histograms H2 (0 to 6)
//    // ---------------------------------------------------------------
//    man->CreateH2("fHitMap_XY", "2D Hit Map X-Y (cm)", 100, -50., 50., 100, -50., 50.); // 0
//    man->CreateH2("fHitMap_YZ", "2D Hit Map Y-Z (cm)", 100, -50., 50., 100, -30., 30.); // 1
//    man->CreateH2("fHitMap_XZ", "2D Hit Map X-Z (cm)", 100, -50., 50., 100, -30., 30.); // 2
//    man->CreateH2("fAnglePlot", "o-Ps Kinematics: X=theta_a+theta_b, Y=theta_b-theta_a (deg)", 180, 0., 360., 90, 0., 180.); // 3
//    man->CreateH2("fGamma1_XZ", "Photon 1 X-Z Hit Map (cm)", 100, -50., 50., 100, -30., 30.); // 4
//    man->CreateH2("fGamma2_YZ", "Photon 2 Y-Z Hit Map (cm)", 100, -50., 50., 100, -30., 30.); // 5
//    man->CreateH2("fGamma3_XY", "Photon 3 X-Y Hit Map (cm)", 100, -50., 50., 100, -50., 50.); // 6

//    // ---------------------------------------------------------------
//    // NEW: 2D Reconstructed Vertex Maps H2 (7 and 8)
//    // ---------------------------------------------------------------
//    man->CreateH2("fRecoVertex_XY", "Reconstructed Vertex X-Y (cm)", 100, -5., 5., 100, -5., 5.);    // 7
//    man->CreateH2("fRecoVertex_XZ", "Reconstructed Vertex X-Z (cm)", 100, -5., 5., 100, -30., 30.);  // 8

//    // ---------------------------------------------------------------
//    // Ntuples Setup
//    // ---------------------------------------------------------------
//    man->CreateNtuple("Photons", "Photons");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fE");
//    man->FinishNtuple();

//    man->CreateNtuple("Hits", "Hits");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fX");
//    man->FinishNtuple();

//    man->CreateNtuple("Scoring", "Scoring");
//    man->CreateNtupleIColumn("fEvent");
//    man->CreateNtupleDColumn("fEdep");
//    man->CreateNtupleIColumn("fScinID");
//    man->CreateNtupleDColumn("fTime");
//    man->CreateNtupleDColumn("fDecayX");
//    man->CreateNtupleDColumn("fTotalEnergy");
//    man->CreateNtupleDColumn("fLifetime");
//    man->FinishNtuple();
//}

//void MyRunAction::EndOfRunAction(const G4Run* run)
//{
//    G4AnalysisManager* man = G4AnalysisManager::Instance();
//    man->Write();
//    man->CloseFile();
//}

//void MyRunAction::AddEventData(G4int, G4double, G4double)
//{
//    // Kept for backward compatibility if called elsewhere
//}


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
