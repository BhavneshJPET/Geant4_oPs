#include "run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4AccumulableManager.hh"
#include <iostream>
#include <fstream>

MyRunAction::MyRunAction(G4String filename)
    : G4UserRunAction(), fFilename(filename),
      fTotalEvents(0), fTotal3GammaEvents(0), fTotal2GammaEvents(0),
      fTotalEnergySum(0.0), fDecayZSum(0.0), fTrueCoincidences(0)
{
    // Register accumulators to the thread-safe manager
    G4AccumulableManager* accumman = G4AccumulableManager::Instance();
    accumman->RegisterAccumulable(fTotalEvents);
    accumman->RegisterAccumulable(fTotal3GammaEvents);
    accumman->RegisterAccumulable(fTotal2GammaEvents);
    accumman->RegisterAccumulable(fTotalEnergySum);
    accumman->RegisterAccumulable(fDecayZSum);
    accumman->RegisterAccumulable(fTrueCoincidences);

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->SetVerboseLevel(1);
    man->SetNtupleMerging(true);

    // =========================================================================
    // 1D HISTOGRAMS (H1)
    // =========================================================================
    man->CreateH1("fEdep", "Total Energy Deposit in Scintillators;Energy (keV);Entries", 200, 0, 2000);
    man->CreateH1("fScinID", "Hit Distribution Across Scintillator Strips;Scintillator ID;Entries", 24, 0, 24);
    man->CreateH1("fTotalEnergy", "Total Initial Gamma Energy (3G);Energy (keV);Entries", 170, 0, 1700);
    man->CreateH1("fOrthops", "Ortho-Positronium State Tag;State (0=PickOff, 1=Natural);Entries", 2, 0, 2);
    man->CreateH1("fGammaMultiplicity", "Recorded Gamma Multiplicity;Multiplicity;Entries", 6, 0, 6);
    man->CreateH1("fLifetime", "Positronium Annihilation Lifetime;Lifetime (ns);Entries", 300, 0, 150);

    // Track distributions along Z
    man->CreateH1("fGamma1_HitZ", "Gamma 1 Scintillator Hit Z;Z (cm);Entries", 100, -30, 30);
    man->CreateH1("fGamma2_HitZ", "Gamma 2 Scintillator Hit Z;Z (cm);Entries", 100, -30, 30);
    man->CreateH1("fGamma3_HitZ", "Gamma 3 Scintillator Hit Z;Z (cm);Entries", 100, -30, 30);
    man->CreateH1("fGamma1_DecayZ", "Gamma 1 Origin Decay Z;Z (cm);Entries", 100, -30, 30);
    man->CreateH1("fGamma2_DecayZ", "Gamma 2 Origin Decay Z;Z (cm);Entries", 100, -30, 30);
    man->CreateH1("fGamma3_DecayZ", "Gamma 3 Origin Decay Z;Z (cm);Entries", 100, -30, 30);

    // 3G Reco standard positions
    man->CreateH1("fRecoVertexX", "3G Reco Vertex X;X (cm);Entries", 100, -5, 5);
    man->CreateH1("fRecoVertexY", "3G Reco Vertex Y;Y (cm);Entries", 100, -5, 5);
    man->CreateH1("fRecoVertexZ", "3G Reco Vertex Z;Z (cm);Entries", 100, -25, 25);
    man->CreateH1("fVertexResolution", "3G Reco Spatial Resolution;Distance (cm);Entries", 100, 0, 10);

    man->CreateH1("fRecoVertexX_True", "3G True Reco Vertex X;X (cm);Entries", 100, -5, 5);
    man->CreateH1("fRecoVertexY_True", "3G True Reco Vertex Y;Y (cm);Entries", 100, -5, 5);
    man->CreateH1("fRecoVertexZ_True", "3G True Reco Vertex Z;Z (cm);Entries", 100, -25, 25);
    man->CreateH1("fVertexResolution_True", "3G True Reco Spatial Resolution;Distance (cm);Entries", 100, 0, 10);

    // Ordered angular spacing diagnostics
    man->CreateH1("fHitAngle_a", "Smallest Gamma Inter-Angle;Angle (deg);Entries", 180, 0, 180);
    man->CreateH1("fHitAngle_b", "Middle Gamma Inter-Angle;Angle (deg);Entries", 180, 0, 180);
    man->CreateH1("fHitAngle_c", "Largest Gamma Inter-Angle;Angle (deg);Entries", 180, 0, 180);

    // Directional 1D angles relative to beam propagation axes
    man->CreateH1("fGammaDir_XZ", "Gamma Angular Distribution XZ;Angle (deg);Entries", 360, -180, 180);
    man->CreateH1("fGammaDir_YZ", "Gamma Angular Distribution YZ;Angle (deg);Entries", 360, -180, 180);
    man->CreateH1("fGammaDir_XY", "Gamma Angular Distribution XY;Angle (deg);Entries", 360, -180, 180);

    // 2-Gamma general reconstruction arrays
    man->CreateH1("f2G_RecoVertexX", "2G LOR Reco Vertex X;X (cm);Entries", 100, -5, 5);
    man->CreateH1("f2G_RecoVertexY", "2G LOR Reco Vertex Y;Y (cm);Entries", 100, -5, 5);
    man->CreateH1("f2G_RecoVertexZ", "2G LOR Reco Vertex Z;Z (cm);Entries", 100, -25, 25);
    man->CreateH1("f2G_Resolution", "2G TOF Line-of-Response Resolution;Distance (cm);Entries", 100, 0, 10);
    man->CreateH1("f2G_OpeningAngle", "2G Back-to-Back Opening Angle;Angle (deg);Entries", 180, 0, 180);

    // Matter-Wave beam tracking profiles
    man->CreateH1("fPatternOpening1", "Beam Spread Envelope crossing Grating 1;Y (cm);Entries", 100, -2, 2);
    man->CreateH1("fPatternOpening2", "Beam Spread Envelope crossing Grating 2;Y (cm);Entries", 100, -2, 2);
    man->CreateH1("fTruePassageY", "True Matter-Wave Passage Pattern at Counter (Truth);Y (cm);Entries", 200, -2, 2);
    man->CreateH1("fTruePassageX", "True Passage Profile X;X (cm);Entries", 100, -2, 2);

    // Target 1D Gated Fringe Arrays
    man->CreateH1("fFringeRecoVertexY", "2G TOF Reco Vertex Y (cm);Y (cm);Entries", 140, -3.5, 3.5);
    man->CreateH1("fFringeRecoVertexX", "2G TOF Reco Vertex X (cm);X (cm);Entries", 140, -3.5, 3.5);

    man->CreateH1("fWall1_RecoX", "2G reco vertex X — Grating 1 wall pick-off;X (cm);Entries", 140, -3.5, 3.5);
    man->CreateH1("fWall2_RecoX", "2G reco vertex X — Grating 2 wall pick-off;X (cm);Entries", 140, -3.5, 3.5);

    // =========================================================================
    // 2D HISTOGRAMS (H2)
    // =========================================================================
    // Ultra-fine 2D layout to resolve the micro slits (10 microns per pixel width)
    man->CreateH2("fMicroGrating2D", "Microscopic Grating Vertex Map (No Smear);X (cm);Y (cm)", 1000, -0.5, 0.5, 140, -3.5, 3.5);

    man->CreateH2("fHitMap_XY", "Scintillator Hit Spatial Distribution XY;X (cm);Y (cm)", 100, -45, 45, 100, -45, 45);
    man->CreateH2("fHitMap_YZ", "Scintillator Hit Spatial Distribution YZ;Y (cm);Z (cm)", 100, -45, 45, 100, -30, 30);
    man->CreateH2("fHitMap_XZ", "Scintillator Hit Spatial Distribution XZ;X (cm);Z (cm)", 100, -45, 45, 100, -30, 30);

    // Specific individual single-gamma structural projections
    man->CreateH2("fGamma1_XZ", "Gamma 1 Hit Position XZ;X (cm);Z (cm)", 100, -45, 45, 100, -30, 30);
    man->CreateH2("fGamma2_YZ", "Gamma 2 Hit Position YZ;Y (cm);Z (cm)", 100, -45, 45, 100, -30, 30);
    man->CreateH2("fGamma3_XY", "Gamma 3 Hit Position XY;X (cm);Y (cm)", 100, -45, 45, 100, -45, 45);

    man->CreateH2("fAnglePlot", "Dalitz-like Angular Separation Map (Natural);#theta_{1}+#theta_{2} (deg);#theta_{2}-#theta_{1} (deg)", 180, 0, 360, 180, 0, 180);
    man->CreateH2("fHitAngle", "Dalitz-like Angular Separation Map (Smeared Reco);#theta_{1}+#theta_{2} (deg);#theta_{2}-#theta_{1} (deg)", 180, 0, 360, 180, 0, 180);
    man->CreateH2("fHitAngle_True", "Dalitz-like Angular Separation Map (True Reco);#theta_{1}+#theta_{2} (deg);#theta_{2}-#theta_{1} (deg)", 180, 0, 360, 180, 0, 180);

    // -----------------------------------------------------------------------
    // 3G reconstructed-vertex projections: X-Y, X-Z, and (NEW) Y-Z, for both
    // detector-level (smeared/"reco") and generator-level ("True") vertices.
    // -----------------------------------------------------------------------
    man->CreateH2("fRecoVertex_XY", "3G Reco Spatial Profiles XY;X (cm);Y (cm)", 100, -10, 10, 100, -10, 10);
    man->CreateH2("fRecoVertex_XZ", "3G Reco Spatial Profiles XZ;X (cm);Z (cm)", 100, -10, 10, 100, -30, 30);
    man->CreateH2("fRecoVertex_YZ", "3G Reco Spatial Profiles YZ;Y (cm);Z (cm)", 100, -10, 10, 100, -30, 30); // NEW

    man->CreateH2("fRecoVertex_XY_True", "3G True Reco Spatial Profiles XY;X (cm);Y (cm)", 100, -10, 10, 100, -10, 10);
    man->CreateH2("fRecoVertex_XZ_True", "3G True Reco Spatial Profiles XZ;X (cm);Z (cm)", 100, -10, 10, 100, -30, 30);
    man->CreateH2("fRecoVertex_YZ_True", "3G True Reco Spatial Profiles YZ;Y (cm);Z (cm)", 100, -10, 10, 100, -30, 30); // NEW

    // -----------------------------------------------------------------------
    // 2G reconstructed-vertex projections (detector-level / TOF reco).
    // FIX: f2G_RecoVertex_XY now uses the same ±6 cm / 200-bin grid as the
    // other fringe-view histograms below (was previously 100 bins, ±10 cm —
    // inconsistent binning made fringe visibility harder to compare directly
    // against f2G_RecoVertex_XZ / f2G_RecoVertex_3D).
    // -----------------------------------------------------------------------
    man->CreateH2("f2G_RecoVertex_XY", "2G Reco Spatial Profiles XY (Fringe view);X (cm);Y (cm)", 200, -6, 6, 200, -6, 6);
    man->CreateH2("f2G_RecoVertex_XZ", "2G TOF Reco Vertex XZ (Fringe view);X (cm);Z (cm)", 200, -6, 6, 200, -25, 25);
    man->CreateH2("f2G_RecoVertex_YZ", "2G TOF Reco Vertex YZ (Fringe view);Y (cm);Z (cm)", 200, -6, 6, 200, -25, 25); // NEW

    man->CreateH2("fTruePassageXY", "True Boundary Crossing Pattern at Counter face;X (cm);Y (cm)", 200, -4, 4, 200, -4, 4);

    man->CreateH2("fFringePatternXY", "LOR Reco Vertex XY, ALL pick-off origins pooled;X (cm);Y (cm)", 100, -5.0, 5.0, 100, -5.0, 5.0);
    man->CreateH2("fFringeRecoVertexXY", "2G-Reco Fringe Pattern, Counter-Origin Only;X (cm);Y (cm)", 140, -3.5, 3.5, 140, -3.5, 3.5);

    // -----------------------------------------------------------------------
    // 2G generator-level (MC truth) projections: X-Y, X-Z, and (NEW) Y-Z.
    // Binning matches the detector-level fringe views above (±6 cm transverse,
    // ±25 cm along Z) for direct true-vs-reco comparability.
    // -----------------------------------------------------------------------
    man->CreateH2("f2G_TrueVertex_XY", "2G True (Generator-Level) Vertex XY;X (cm);Y (cm)", 200, -6, 6, 200, -6, 6);
    man->CreateH2("f2G_TrueVertex_XZ", "2G True (Generator-Level) Vertex XZ (Fringe view);X (cm);Z (cm)", 200, -6, 6, 200, -25, 25);
    man->CreateH2("f2G_TrueVertex_YZ", "2G True (Generator-Level) Vertex YZ (Fringe view);Y (cm);Z (cm)", 200, -6, 6, 200, -25, 25); // NEW

    // =========================================================================
    // 3D HISTOGRAMS (H3)
    // =========================================================================
    // FIXED: Reduced from 1000x1000x1000 down to a standard thread-safe 100x100x100 grid
    man->CreateH3("f3G_RecoVertex_3D", "3G Fully Reconstructed Vertices 3D;X (cm);Y (cm);Z (cm)", 60, -6, 6, 60, -6, 6, 100, -25, 25);
    man->CreateH3("f2G_RecoVertex_3D", "2G Fully Reconstructed Vertices 3D;X (cm);Y (cm);Z (cm)", 200, -1, 1, 200, -1, 1, 200, -25, -10);
    man->CreateH3("f2G_TrueVertex_3D", "2G Generator-Level (True MC) Vertices 3D;X (cm);Y (cm);Z (cm)", 200, -1, 1, 200, -1, 1, 200, -25, -10);
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
    G4AccumulableManager* accumman = G4AccumulableManager::Instance();
    accumman->Reset();

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->OpenFile(fFilename);
}

void MyRunAction::AddEventData(G4int gammaCount, G4double totalEnergy, G4double decayZ)
{
    fTotalEvents += 1;
    fTotalEnergySum += totalEnergy;
    fDecayZSum += decayZ;

    if (gammaCount == 3) {
        fTotal3GammaEvents += 1;
    } else if (gammaCount == 2) {
        fTotal2GammaEvents += 1;
    }
}

void MyRunAction::AddTrueCoincidence() { fTrueCoincidences += 1; }
void MyRunAction::Add2GammaEvent()     { fTotal2GammaEvents += 1; }

void MyRunAction::EndOfRunAction(const G4Run*)
{
    G4AccumulableManager* accumman = G4AccumulableManager::Instance();
    accumman->Merge();

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();

    G4int totalEventsProcessed = fTotalEvents.GetValue();
    G4int total3G              = fTotal3GammaEvents.GetValue();
    G4int total2G              = fTotal2GammaEvents.GetValue();

    std::cout << "\n=========================================================" << std::endl;
    std::cout << "                  J-PET RUN ANALYSIS                     " << std::endl;
    std::cout << "=========================================================" << std::endl;
    std::cout << " -> Total Reconstructed Events : " << totalEventsProcessed << std::endl;
    std::cout << "    * Natural 3-Gamma Decays   : " << total3G              << std::endl;
    std::cout << "    * Solid Pick-Off 2-Gammas  : " << total2G              << std::endl;

    if(totalEventsProcessed > 0) {
        G4double meanEnergy = fTotalEnergySum.GetValue() / totalEventsProcessed;
        G4double meanDecayZ = fDecayZSum.GetValue() / totalEventsProcessed;
        std::cout << " -> Mean Primary Track Energy  : " << meanEnergy / keV << " keV" << std::endl;
        std::cout << " -> Mean Decay Position Z Axis : " << meanDecayZ / cm  << " cm" << std::endl;
    }
    std::cout << "=========================================================\n" << std::endl;

    if (G4Threading::IsMasterThread()) {
        std::ofstream logFile("run_summary.txt", std::ios::out | std::ios::app);
        if(logFile.is_open()) {
            logFile << totalEventsProcessed << "\t" << total3G << "\t" << total2G << "\n";
            logFile.close();
        }
    }
}
