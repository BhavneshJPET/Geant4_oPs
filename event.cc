#include "event.hh"
#include "run.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include <algorithm>
#include <cmath>

MyEventAction::MyEventAction(MyRunAction*)
    : G4UserEventAction(), fGammaCount(0), fFirstScinID(-1), fMinTime(1e9),
      fDecayX(0.0), fDecayY(0.0), fDecayZ(0.0), fTotalEnergy(0.0),
      fFormationTime(0.0), fDecayTime(0.0), fDecayDetected(false),
      fHitCount(0), fGammaHitsRecorded(0)
{
    fEdepScin.assign(kNumModules, 0.0);
    fHitTime .assign(kNumModules, 1e9);

    for(G4int i = 0; i < 3; i++) {
        fGammaEnergy[i]    = 0.0;
        fGammaDirection[i] = G4ThreeVector(0,0,0);
        fHitPos[i]         = G4ThreeVector(0,0,0);
        fGammaHitPos[i]    = G4ThreeVector(0,0,0);
        fSmearedHitPos[i]  = G4ThreeVector(0,0,0);
        fGammaHitTime[i]   = 1e9;
        fGammaHitSet[i]    = false;
    }
}

MyEventAction::~MyEventAction() {}

// ---------------------------------------------------------------------------
void MyEventAction::BeginOfEventAction(const G4Event*)
{
    fGammaCount = 0; fTotalEnergy = 0.0;
    fDecayX = 0.0; fDecayY = 0.0; fDecayZ = 0.0;
    fFirstScinID = -1; fMinTime = 1e9;
    fFormationTime = 0.0; fDecayTime = 0.0;
    fDecayDetected = false; fHitCount = 0;
    fGammaHitsRecorded = 0;

    for(G4int i = 0; i < kNumModules; i++) {
        fEdepScin[i] = 0.0;
        fHitTime[i]  = 1e9;
    }
    for(G4int i = 0; i < 3; i++) {
        fGammaEnergy[i]    = 0.0;
        fGammaDirection[i] = G4ThreeVector(0,0,0);
        fHitPos[i]         = G4ThreeVector(0,0,0);
        fGammaHitPos[i]    = G4ThreeVector(0,0,0);
        fSmearedHitPos[i]  = G4ThreeVector(0,0,0);
        fGammaHitTime[i]   = 1e9;
        fGammaHitSet[i]    = false;
    }
}

// ---------------------------------------------------------------------------
void MyEventAction::AddGammaInfo(G4double energy, G4ThreeVector direction)
{
    if(fGammaCount < 3) {
        fGammaEnergy[fGammaCount]    = energy;
        fGammaDirection[fGammaCount] = direction;
        fTotalEnergy += energy;
        fGammaCount++;
    }
}

void MyEventAction::AddHitPosition(G4ThreeVector pos)
{
    if(fHitCount < 3) fHitPos[fHitCount++] = pos;
}

void MyEventAction::SetGammaHit(G4int gammaIndex, G4ThreeVector pos)
{
    if(gammaIndex >= 0 && gammaIndex < 3 && !fGammaHitSet[gammaIndex]) {
        fGammaHitPos[gammaIndex] = pos;
        fGammaHitSet[gammaIndex] = true;
    }
}

void MyEventAction::SetGammaHitTime(G4int gammaIndex, G4double time)
{
    if(gammaIndex >= 0 && gammaIndex < 3)
        if(time < fGammaHitTime[gammaIndex])
            fGammaHitTime[gammaIndex] = time;
}

void MyEventAction::AddEdep(G4int scinID, G4double edep, G4double time)
{
    if(scinID >= 0 && scinID < kNumModules) {
        fEdepScin[scinID] += edep;
        if(time < fHitTime[scinID]) fHitTime[scinID] = time;
        if(time < fMinTime) { fMinTime = time; fFirstScinID = scinID; }
    }
}

void MyEventAction::AddTotalEnergy(G4double energy) { fTotalEnergy = energy; }

void MyEventAction::SetDecayPosition(G4double x, G4double y, G4double z)
{ fDecayX = x; fDecayY = y; fDecayZ = z; }

// ---------------------------------------------------------------------------
// SMEARING
// ---------------------------------------------------------------------------

G4ThreeVector MyEventAction::SmearHitPosition(const G4ThreeVector& truePos)
{
    const G4double sigmaRadial = 1.0 * cm;
    const G4double sigmaZ      = 3.0 * cm;

    G4double phi = std::atan2(truePos.y(), truePos.x());

    G4ThreeVector radial_hat(std::cos(phi), std::sin(phi), 0.0);
    G4ThreeVector z_hat(0.0, 0.0, 1.0);

    G4double deltaRadial = G4RandGauss::shoot(0.0, sigmaRadial);
    G4double deltaZ      = G4RandGauss::shoot(0.0, sigmaZ);

    return truePos + deltaRadial * radial_hat + deltaZ * z_hat;
}

// ---------------------------------------------------------------------------
// TRILATERATION
// Uses fSmearedHitPos[] and fGammaHitTime[]
// Reconstructs XY well; Z set separately from smeared true decay Z
// ---------------------------------------------------------------------------
G4ThreeVector MyEventAction::ReconstructVertex()
{
    const G4double c = c_light;

    G4ThreeVector H[3] = { fSmearedHitPos[0],
                            fSmearedHitPos[1],
                            fSmearedHitPos[2] };
    G4double      t[3] = { fGammaHitTime[0],
                            fGammaHitTime[1],
                            fGammaHitTime[2] };

    G4double d12 = c * (t[0] - t[1]);
    G4double d13 = c * (t[0] - t[2]);

    G4ThreeVector V = (H[0] + H[1] + H[2]) / 3.0;
    G4double damping = 0.1;

    for(G4int iter = 0; iter < 50; iter++)
    {
        G4double r0 = (H[0] - V).mag();
        G4double r1 = (H[1] - V).mag();
        G4double r2 = (H[2] - V).mag();

        if(r0 < 1e-3 || r1 < 1e-3 || r2 < 1e-3) break;

        G4double f0 = (r0 - r1) - d12;
        G4double f1 = (r0 - r2) - d13;

        G4ThreeVector u0 = (V - H[0]).unit();
        G4ThreeVector u1 = (V - H[1]).unit();
        G4ThreeVector u2 = (V - H[2]).unit();

        G4ThreeVector J0 = u0 - u1;
        G4ThreeVector J1 = u0 - u2;

        G4ThreeVector J_cross = J0.cross(J1);
        G4double norm_sq = J_cross.mag2();
        if(norm_sq < 1e-6) break;

        G4ThreeVector delta =
            ((f0 * J1.cross(J_cross)) - (f1 * J0.cross(J_cross))) / norm_sq;
        V -= damping * delta;

        if(delta.mag() < 0.01 * mm) break;
    }
    return V;
}

// ---------------------------------------------------------------------------
static G4double SafeAngleDeg(G4ThreeVector a, G4ThreeVector b)
{
    G4double c = a.unit().dot(b.unit());
    c = std::max(-1.0, std::min(1.0, c));
    return std::acos(c) / CLHEP::deg;
}

// ---------------------------------------------------------------------------
// Helper: fill trilateration + hit-angle histograms
// Called from both the primary path and the fallback path
// recoV must already have Z set from smeared true decay Z
// ---------------------------------------------------------------------------
void MyEventAction::FillRecoHistograms(const G4ThreeVector& recoV,
                                        G4int eventID,
                                        G4AnalysisManager* man)
{
    man->FillH1(15, recoV.x() / cm);
    man->FillH1(16, recoV.y() / cm);
    man->FillH1(17, recoV.z() / cm);

    // Reconstructed vertex 2D maps mapped to H2 (7, 8)
    man->FillH2(7,  recoV.x() / cm, recoV.y() / cm); // fRecoVertex_XY
    man->FillH2(8,  recoV.x() / cm, recoV.z() / cm); // fRecoVertex_XZ

    // Reconstructed vertex spatial resolution mapped to H1 (18)
    G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
    man->FillH1(18, res / cm);

    // Hit angles at reconstructed vertex using SMEARED positions
    G4ThreeVector d1 = (fSmearedHitPos[0] - recoV).unit();
    G4ThreeVector d2 = (fSmearedHitPos[1] - recoV).unit();
    G4ThreeVector d3 = (fSmearedHitPos[2] - recoV).unit();

    G4double a12 = SafeAngleDeg(d1, d2);
    G4double a23 = SafeAngleDeg(d2, d3);
    G4double a13 = SafeAngleDeg(d1, d3);

    G4double angs[3] = {a12, a23, a13};
    std::sort(angs, angs + 3);

    // Smeared/Reconstructed hit angles mapped to separate H1 slots (19, 20, 21)
    man->FillH1(19, angs[0]); // fHitAngle_a
    man->FillH1(20, angs[1]); // fHitAngle_b
    man->FillH1(21, angs[2]); // fHitAngle_c
    
    // Smeared/Reconstructed Kinematic Dalitz mapped to H2 (9)
    man->FillH2(9,  angs[0] + angs[1], angs[1] - angs[0]); // fHitAngle

    if(eventID < 10) {
        G4cout << "\n=== Reco Event " << eventID << " ===" << G4endl;
        G4cout << "  RecoVertex: (" << recoV.x()/cm << ", " << recoV.y()/cm << ", " << recoV.z()/cm << ") cm" << G4endl;
        G4cout << "  Resolution: " << res/cm << " cm" << G4endl;
        G4cout << "  Dalitz X=" << angs[0]+angs[1] << "  Y=" << angs[1]-angs[0] << G4endl;
    }
}

// ---------------------------------------------------------------------------
// END OF EVENT
// ---------------------------------------------------------------------------
void MyEventAction::EndOfEventAction(const G4Event* event)
{
    G4AnalysisManager* man     = G4AnalysisManager::Instance();
    G4int              eventID = event->GetEventID();

    G4double totalEdep = 0.;
    G4int    hitCount  = 0;
    for(G4int i = 0; i < kNumModules; i++) {
        if(fEdepScin[i] > 0.) { totalEdep += fEdepScin[i]; hitCount++; }
    }
    G4double lifetime = fDecayTime - fFormationTime;

    // ---------------------------------------------------------------
    // STEP 1 — SMEARING
    // Apply BEFORE trilateration and Dalitz calculation
    // fGammaHitPos[]   = true MC positions (kept for reference)
    // fSmearedHitPos[] = smeared positions (used for all reconstruction)
    // ---------------------------------------------------------------
    if(fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2])
    {
        for(G4int i = 0; i < 3; i++)
            fSmearedHitPos[i] = SmearHitPosition(fGammaHitPos[i]);
    }

    // ---------------------------------------------------------------
    // H1(0-14): Standard histograms
    // ---------------------------------------------------------------
    if(totalEdep > 0.)
        man->FillH1(0, totalEdep / keV);

    for(G4int i = 0; i < kNumModules; i++)
        if(fEdepScin[i] > 0.)
            man->FillH1(1, static_cast<G4double>(i));

    if(fGammaCount == 3 && fTotalEnergy > 0.)
        man->FillH1(2, fTotalEnergy / keV);

    man->FillH1(3, fGammaCount == 3 ? 1. : 0.);
    man->FillH1(4, static_cast<G4double>(fGammaCount));

    if(fDecayTime > 0. && lifetime > 0.)
        man->FillH1(5, lifetime / ns);

    if(fGammaCount >= 3) {
        man->FillH1(6, std::atan2(fGammaDirection[0].x(),
                                   fGammaDirection[0].z()) / deg);
        man->FillH1(7, std::atan2(fGammaDirection[1].y(),
                                   fGammaDirection[1].z()) / deg);
        man->FillH1(8, std::atan2(fGammaDirection[2].y(),
                                   fGammaDirection[2].x()) / deg);
    }

    // H1(9-11): True hit Z positions
    if(fGammaHitSet[0]) man->FillH1(9,  fGammaHitPos[0].z() / cm);
    if(fGammaHitSet[1]) man->FillH1(10, fGammaHitPos[1].z() / cm);
    if(fGammaHitSet[2]) man->FillH1(11, fGammaHitPos[2].z() / cm);

    if(fDecayTime > 0. && fDecayDetected) {
        man->FillH1(12, fDecayZ / cm);
        man->FillH1(13, fDecayZ / cm);
        man->FillH1(14, fDecayZ / cm);
    }

    // ---------------------------------------------------------------
    // H2(0-2): Hit maps using TRUE positions (geometry validation)
    // ---------------------------------------------------------------
    for(G4int i = 0; i < 3; i++) {
        if(fGammaHitSet[i]) {
            man->FillH2(0, fGammaHitPos[i].x()/cm, fGammaHitPos[i].y()/cm);
            man->FillH2(1, fGammaHitPos[i].y()/cm, fGammaHitPos[i].z()/cm);
            man->FillH2(2, fGammaHitPos[i].x()/cm, fGammaHitPos[i].z()/cm);
        }
    }

    // ---------------------------------------------------------------
    // H2(3): Dalitz plot using SMEARED positions + true decay vertex
    // ---------------------------------------------------------------
    if(fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2])
    {
        G4ThreeVector decayPos(fDecayX, fDecayY, fDecayZ);
        G4ThreeVector v1 = (fSmearedHitPos[0] - decayPos).unit();
        G4ThreeVector v2 = (fSmearedHitPos[1] - decayPos).unit();
        G4ThreeVector v3 = (fSmearedHitPos[2] - decayPos).unit();

        G4double t12 = SafeAngleDeg(v1, v2);
        G4double t23 = SafeAngleDeg(v2, v3);
        G4double t31 = SafeAngleDeg(v3, v1);

        G4double angles[3] = {t12, t23, t31};
        std::sort(angles, angles + 3);
        man->FillH2(9, angles[0] + angles[1], angles[1] - angles[0]);
    }
    else if(fGammaCount == 3)
    {
        // Fallback: generation directions
        G4double t12 = SafeAngleDeg(fGammaDirection[0], fGammaDirection[1]);
        G4double t23 = SafeAngleDeg(fGammaDirection[1], fGammaDirection[2]);
        G4double t31 = SafeAngleDeg(fGammaDirection[2], fGammaDirection[0]);
        G4double gen[3] = {t12, t23, t31};
        std::sort(gen, gen + 3);
        man->FillH2(3, gen[0] + gen[1], gen[1] - gen[0]);
    }

    if(fGammaHitSet[0]) man->FillH2(4, fGammaHitPos[0].x()/cm, fGammaHitPos[0].z()/cm);
    if(fGammaHitSet[1]) man->FillH2(5, fGammaHitPos[1].y()/cm, fGammaHitPos[1].z()/cm);
    if(fGammaHitSet[2]) man->FillH2(6, fGammaHitPos[2].x()/cm, fGammaHitPos[2].y()/cm);

    // ---------------------------------------------------------------
    // STEP 2 — TRILATERATION using SMEARED positions
    //
    // Z FIX: Trilateration cannot reconstruct Z when beam is along Z
    // because all 3 gammas travel mostly in XY plane and their hit
    // times carry no independent Z information.
    //
    // Solution: Z is taken as smeared true decay Z (sigma=3cm)
    // This simulates the real J-PET Z measurement which uses the
    // time difference of signals at the two ends of each bar —
    // a separate measurement from the trilateration between bars.
    // ---------------------------------------------------------------
    if(fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2])
    {
        G4ThreeVector recoV = ReconstructVertex();

        // Replace Z with smeared true decay Z (sigma = 3 cm)
        G4double recoZ = fDecayZ + G4RandGauss::shoot(0., 3.0 * cm);
        recoV.setZ(recoZ);

        FillRecoHistograms(recoV, eventID, man);
    }
    else if(fGammaCount == 3 && fDecayDetected)
    {
        // Fallback: project gammas to virtual cylinder
        G4double virtualRadius = 42.5 * cm;
        const G4double c = c_light;
        G4bool validFallback = true;

        // Save state
        G4ThreeVector savedSmeared[3];
        G4double      savedTime[3];
        G4bool        savedSet[3];
        for(G4int i = 0; i < 3; i++) {
            savedSmeared[i] = fSmearedHitPos[i];
            savedTime[i]    = fGammaHitTime[i];
            savedSet[i]     = fGammaHitSet[i];
        }

        for(G4int i = 0; i < 3; i++) {
            G4double dir_t = std::sqrt(
                fGammaDirection[i].x()*fGammaDirection[i].x() +
                fGammaDirection[i].y()*fGammaDirection[i].y());

            if(dir_t > 1e-6) {
                G4double dist = virtualRadius / dir_t;
                G4ThreeVector vHit =
                    G4ThreeVector(fDecayX, fDecayY, fDecayZ)
                    + fGammaDirection[i] * dist;

                // Smear the virtual hit
                fSmearedHitPos[i] = SmearHitPosition(vHit);
                fGammaHitTime[i]  = (vHit
                    - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag() / c;
                fGammaHitSet[i]   = true;
            } else {
                validFallback = false;
            }
        }

        if(validFallback) {
            G4ThreeVector recoV = ReconstructVertex();

            // Same Z fix in fallback path
            G4double recoZ = fDecayZ + G4RandGauss::shoot(0., 3.0 * cm);
            recoV.setZ(recoZ);

            FillRecoHistograms(recoV, eventID, man);
        }

        // Restore state
        for(G4int i = 0; i < 3; i++) {
            fSmearedHitPos[i] = savedSmeared[i];
            fGammaHitTime[i]  = savedTime[i];
            fGammaHitSet[i]   = savedSet[i];
        }
    }
}


//#include "event.hh"
//#include "run.hh"
//#include "G4RunManager.hh"
//#include "G4AnalysisManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4PhysicalConstants.hh"
//#include "Randomize.hh"
//#include <algorithm>
//#include <cmath>

//MyEventAction::MyEventAction(MyRunAction*)
//    : G4UserEventAction(), fGammaCount(0), fFirstScinID(-1), fMinTime(1e9),
//      fDecayX(0.0), fDecayY(0.0), fDecayZ(0.0), fTotalEnergy(0.0),
//      fFormationTime(0.0), fDecayTime(0.0), fDecayDetected(false), fHitCount(0)
//{
//    fEdepScin.assign(kNumModules, 0.0);
//    fHitTime .assign(kNumModules, 1e9);

//    for(G4int i = 0; i < 3; i++) {
//        fGammaEnergy[i]    = 0.0;
//        fGammaDirection[i] = G4ThreeVector(0,0,0);
//        fHitPos[i]         = G4ThreeVector(0,0,0);
//        fGammaHitPos[i]    = G4ThreeVector(0,0,0);
//        fGammaHitTime[i]   = 1e9;
//        fGammaHitSet[i]    = false;
//    }
//}

//MyEventAction::~MyEventAction() {}

//void MyEventAction::BeginOfEventAction(const G4Event*)
//{
//    fGammaCount = 0; fTotalEnergy = 0.0;
//    fDecayX = 0.0; fDecayY = 0.0; fDecayZ = 0.0;
//    fFirstScinID = -1; fMinTime = 1e9;
//    fFormationTime = 0.0; fDecayTime = 0.0; fDecayDetected = false; fHitCount = 0;

//    for(G4int i = 0; i < kNumModules; i++) {
//        fEdepScin[i] = 0.0; fHitTime [i] = 1e9;
//    }
//    for(G4int i = 0; i < 3; i++) {
//        fGammaEnergy[i]    = 0.0;
//        fGammaDirection[i] = G4ThreeVector(0,0,0);
//        fHitPos[i]         = G4ThreeVector(0,0,0);
//        fGammaHitPos[i]    = G4ThreeVector(0,0,0);
//        fGammaHitTime[i]   = 1e9;
//        fGammaHitSet[i]    = false;
//    }
//}

//void MyEventAction::AddGammaInfo(G4double energy, G4ThreeVector direction)
//{
//    if(fGammaCount < 3) {
//        fGammaEnergy[fGammaCount] = energy;
//        fGammaDirection[fGammaCount] = direction;
//        fTotalEnergy += energy; fGammaCount++;
//    }
//}

//void MyEventAction::AddHitPosition(G4ThreeVector pos) { if(fHitCount < 3) fHitPos[fHitCount++] = pos; }

//void MyEventAction::SetGammaHit(G4int gammaIndex, G4ThreeVector pos)
//{
//    if(gammaIndex >= 0 && gammaIndex < 3 && !fGammaHitSet[gammaIndex]) {
//        fGammaHitPos[gammaIndex] = pos;
//        fGammaHitSet[gammaIndex] = true;
//    }
//}

//void MyEventAction::SetGammaHitTime(G4int gammaIndex, G4double time)
//{
//    if(gammaIndex >= 0 && gammaIndex < 3) {
//        if(time < fGammaHitTime[gammaIndex]) fGammaHitTime[gammaIndex] = time;
//    }
//}

//void MyEventAction::AddEdep(G4int scinID, G4double edep, G4double time)
//{
//    if(scinID >= 0 && scinID < kNumModules) {
//        fEdepScin[scinID] += edep;
//        if(time < fHitTime[scinID]) fHitTime[scinID] = time;
//        if(time < fMinTime) { fMinTime = time; fFirstScinID = scinID; }
//    }
//}

//void MyEventAction::AddTotalEnergy(G4double energy) { fTotalEnergy = energy; }
//void MyEventAction::SetDecayPosition(G4double x, G4double y, G4double z) { fDecayX = x; fDecayY = y; fDecayZ = z; }

//// --- TRILATERATION RECONSTRUCTION METHOD ---
//G4ThreeVector MyEventAction::ReconstructVertex()
//{
//    if(!fGammaHitSet[0] || !fGammaHitSet[1] || !fGammaHitSet[2])
//        return G4ThreeVector(0,0,0);

//    // CRITICAL FIX: Match the synchronization velocity parameter with the medium properties 
//    const G4double v_scin = 126.0 * mm / ns; // Effective light propagation velocity in J-PET strip (~12.6 cm/ns)
//    
//    G4ThreeVector H[3] = { fGammaHitPos[0], fGammaHitPos[1], fGammaHitPos[2] };
//    G4double t[3]      = { fGammaHitTime[0], fGammaHitTime[1], fGammaHitTime[2] };

//    // Range constraints matching scintillator timing properties
//    G4double d12 = v_scin * (t[0] - t[1]);
//    G4double d13 = v_scin * (t[0] - t[2]);

//    // Initial guess: Centroid of hit positions
//    G4ThreeVector V = (H[0] + H[1] + H[2]) / 3.0;
//    G4double damping = 0.1;

//    // Newton-Raphson Loop over residual vector fields
//    for(G4int iter = 0; iter < 50; iter++)
//    {
//        G4double r0 = (H[0] - V).mag();
//        G4double r1 = (H[1] - V).mag();
//        G4double r2 = (H[2] - V).mag();

//        if(r0 < 1e-3 || r1 < 1e-3 || r2 < 1e-3) break;

//        // Residual constraints
//        G4double f0 = (r0 - r1) - d12;
//        G4double f1 = (r0 - r2) - d13;

//        // Unit vector derivatives
//        G4ThreeVector u0 = (V - H[0]).unit();
//        G4ThreeVector u1 = (V - H[1]).unit();
//        G4ThreeVector u2 = (V - H[2]).unit();

//        // Standard Jacobians
//        G4ThreeVector J0 = u0 - u1;
//        G4ThreeVector J1 = u0 - u2;

//        // Compute update step via basic pseudo-inverse vector projection
//        G4ThreeVector J_cross = J0.cross(J1);
//        G4double norm_sq = J_cross.mag2();
//        if(norm_sq < 1e-6) break;

//        G4ThreeVector delta = ( (f0 * J1.cross(J_cross)) - (f1 * J0.cross(J_cross)) ) / norm_sq;
//        V -= damping * delta; 
//        
//        if(delta.mag() < 0.01 * mm) break;
//    }
//    return V;
//}

//void MyEventAction::EndOfEventAction(const G4Event* event)
//{
//    G4AnalysisManager* man     = G4AnalysisManager::Instance();
//    G4int              eventID = event->GetEventID();

//    G4double totalEdep = 0.; G4int hitCount = 0;
//    for(G4int i = 0; i < kNumModules; i++) {
//        if(fEdepScin[i] > 0.) { totalEdep += fEdepScin[i]; hitCount++; }
//    }
//    G4double lifetime = fDecayTime - fFormationTime;

//    // Standard Histograms (H1: 0 - 14)
//    if(totalEdep > 0.) man->FillH1(0, totalEdep / keV);
//    for(G4int i = 201; i < kNumModules; i++) if(fEdepScin[i] > 0.) man->FillH1(1, static_cast<G4double>(i));
//    if(fGammaCount == 3 && fTotalEnergy > 0.) man->FillH1(2, fTotalEnergy / keV);
//    man->FillH1(3, fGammaCount == 3 ? 1. : 0.);
//    man->FillH1(4, static_cast<G4double>(fGammaCount));
//    if(fDecayTime > 0. && lifetime > 0.) man->FillH1(5, lifetime / ns);

//    if(fGammaCount >= 3) {
//        man->FillH1(6, std::atan2(fGammaDirection[0].x(), fGammaDirection[0].z()) / deg);
//        man->FillH1(7, std::atan2(fGammaDirection[1].y(), fGammaDirection[1].z()) / deg);
//        man->FillH1(8, std::atan2(fGammaDirection[2].y(), fGammaDirection[2].x()) / deg);
//    }
//    if(fGammaHitSet[0]) man->FillH1(9,  fGammaHitPos[0].z() / cm);
//    if(fGammaHitSet[1]) man->FillH1(10, fGammaHitPos[1].z() / cm);
//    if(fGammaHitSet[2]) man->FillH1(11, fGammaHitPos[2].z() / cm);
//    if(fDecayTime > 0. && fDecayDetected) {
//        man->FillH1(12, fDecayZ / cm); man->FillH1(13, fDecayZ / cm); man->FillH1(14, fDecayZ / cm);
//    }

//    // 2D Projections (H2: 0 - 2)
//    for(G4int i = 0; i < 3; i++) {
//        if(fGammaHitSet[i]) {
//            man->FillH2(0, fGammaHitPos[i].x()/cm, fGammaHitPos[i].y()/cm);
//            man->FillH2(1, fGammaHitPos[i].y()/cm, fGammaHitPos[i].z()/cm);
//            man->FillH2(2, fGammaHitPos[i].x()/cm, fGammaHitPos[i].z()/cm);
//        }
//    }

//    // H2(3) - Pure Kinematics Kinematic Sorting 
//    if (fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2]) {
//        G4ThreeVector decayPos(fDecayX, fDecayY, fDecayZ);
//        G4ThreeVector v1 = (fGammaHitPos[0] - decayPos).unit(), v2 = (fGammaHitPos[1] - decayPos).unit(), v3 = (fGammaHitPos[2] - decayPos).unit();
//        G4double theta12 = std::acos(std::max(-1.0, std::min(1.0, v1.dot(v2)))) / deg;
//        G4double theta23 = std::acos(std::max(-1.0, std::min(1.0, v2.dot(v3)))) / deg;
//        G4double theta31 = std::acos(std::max(-1.0, std::min(1.0, v3.dot(v1)))) / deg;
//        G4double angles[3] = {theta12, theta23, theta31}; std::sort(angles, angles + 3);
//        man->FillH2(3, angles[0] + angles[1], angles[1] - angles[0]);
//    }
//    else if (fGammaCount == 3) {
//        G4double t12 = std::acos(std::max(-1.0, std::min(1.0, fGammaDirection[0].dot(fGammaDirection[1])))) / deg;
//        G4double t23 = std::acos(std::max(-1.0, std::min(1.0, fGammaDirection[1].dot(fGammaDirection[2])))) / deg;
//        G4double t31 = std::acos(std::max(-1.0, std::min(1.0, fGammaDirection[2].dot(fGammaDirection[0])))) / deg;
//        G4double gen_angles[3] = {t12, t23, t31}; std::sort(gen_angles, gen_angles + 3);
//        man->FillH2(3, gen_angles[0] + gen_angles[1], gen_angles[1] - gen_angles[0]);
//    }

//    if(fGammaHitSet[0]) man->FillH2(4, fGammaHitPos[0].x()/cm, fGammaHitPos[0].z()/cm);
//    if(fGammaHitSet[1]) man->FillH2(5, fGammaHitPos[1].y()/cm, fGammaHitPos[1].z()/cm);
//    if(fGammaHitSet[2]) man->FillH2(6, fGammaHitPos[2].x()/cm, fGammaHitPos[2].y()/cm);

//    // ---------------------------------------------------------------
//    // NEW: TRILATERATION FILL LOGIC WITH DOUBLE-ENDED SMEARING & FALLBACK
//    // ---------------------------------------------------------------
//    if(fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2])
//    {
//        // --- PATH A: TRUE DETECTOR COINCIDENCES WITH EXPERIMENTAL READOUT SMEARING ---
//        const G4double v_eff = 126.0 * mm / ns; 
//        const G4double barLength = 500.0 * mm;  
//        const G4double sigma_t = 0.080 * ns;    // 80 ps electronics jitter

//        G4ThreeVector experimentalHitPos[3];
//        G4double experimentalHitTime[3];

//        for(G4int i = 0; i < 3; i++) {
//            G4double trueZ = fGammaHitPos[i].z();
//            G4double trueHitTime = fGammaHitTime[i];

//            // Map physical delays inside plastic medium
//            G4double t_left_ideal  = trueHitTime + ((barLength / 2.0) + trueZ) / v_eff;
//            G4double t_right_ideal = trueHitTime + ((barLength / 2.0) - trueZ) / v_eff;

//            // Apply electronic noise
//            G4double t_left_smeared  = G4RandGauss::shoot(t_left_ideal, sigma_t);
//            G4double t_right_smeared = G4RandGauss::shoot(t_right_ideal, sigma_t);

//            // Reconstruct positions from edge differences
//            G4double measuredZ = v_eff * (t_left_smeared - t_right_smeared) / 2.0;
//            G4double measuredTime = ((t_left_smeared + t_right_smeared) / 2.0) - (barLength / (2.0 * v_eff));

//            experimentalHitPos[i]  = G4ThreeVector(fGammaHitPos[i].x(), fGammaHitPos[i].y(), measuredZ);
//            experimentalHitTime[i] = measuredTime;
//        }

//        // Cache perfect data vectors to protect downstream systems
//        G4ThreeVector trueHitPosBackup[3] = { fGammaHitPos[0], fGammaHitPos[1], fGammaHitPos[2] };
//        G4double trueHitTimeBackup[3]    = { fGammaHitTime[0], fGammaHitTime[1], fGammaHitTime[2] };

//        for(G4int i = 0; i < 3; i++) {
//            fGammaHitPos[i]  = experimentalHitPos[i];
//            fGammaHitTime[i] = experimentalHitTime[i];
//        }

//        // Run multi-photon trilateration vertex loop using realistic smeared values
//        G4ThreeVector recoV = ReconstructVertex();

//        // Instantly roll back local arrays to keep primary objects clean
//        for(G4int i = 0; i < 3; i++) {
//            fGammaHitPos[i]  = trueHitPosBackup[i];
//            fGammaHitTime[i] = trueHitTimeBackup[i];
//        }

//        // Fill standard monitoring channels
//        man->FillH1(15, recoV.x() / cm);
//        man->FillH1(16, recoV.y() / cm);
//        man->FillH1(17, recoV.z() / cm);
//        man->FillH2(7,  recoV.x() / cm, recoV.y() / cm);
//        man->FillH2(8,  recoV.x() / cm, recoV.z() / cm);

//        G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
//        man->FillH1(18, res / cm);

//        // Fill NEW isolated Z-residual histogram to peel back the exponential decay
//        G4double deltaZ = recoV.z() - fDecayZ;
//        man->FillH1(19, deltaZ / cm);
//    }
//    else if(fGammaCount == 3 && fDecayDetected)
//    {
//        // --- PATH B: FALLBACK FOR TESTING (VACUUM SENSITIVITY TESTING) ---
//        G4double virtualRadius = 42.5 * cm; 
//        const G4double c = c_light;
//        G4bool validFallback = true;

//        G4ThreeVector savedPos[3];
//        G4double savedTime[3];
//        G4bool savedSet[3];

//        for(G4int i = 0; i < 3; i++) {
//            savedPos[i]  = fGammaHitPos[i];
//            savedTime[i] = fGammaHitTime[i];
//            savedSet[i]  = fGammaHitSet[i];

//            G4double r_xy = virtualRadius; 
//            G4double dir_transverse = std::sqrt(fGammaDirection[i].x()*fGammaDirection[i].x() + fGammaDirection[i].y()*fGammaDirection[i].y());
//            
//            if(dir_transverse > 1e-6) {
//                G4double distance = r_xy / dir_transverse; 
//                fGammaHitPos[i] = G4ThreeVector(fDecayX, fDecayY, fDecayZ) + fGammaDirection[i] * distance;
//                fGammaHitTime[i] = (fGammaHitPos[i] - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag() / c;
//                fGammaHitSet[i] = true;
//            } else {
//                validFallback = false;
//            }
//        }

//        if(validFallback) {
//            G4ThreeVector recoV = ReconstructVertex();
//            man->FillH1(15, recoV.x() / cm);
//            man->FillH1(16, recoV.y() / cm);
//            man->FillH1(17, recoV.z() / cm);
//            man->FillH2(7,  recoV.x() / cm, recoV.y() / cm);
//            man->FillH2(8,  recoV.x() / cm, recoV.z() / cm);

//            G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
//            man->FillH1(18, res / cm);
//            
//            // ADD THIS LINE HERE TO CAPTURE RESIDUALS IN PATH B:
//            man->FillH1(19, (recoV.z() - fDecayZ) / cm);
//        }

//        for(G4int i = 0; i < 3; i++) {
//            fGammaHitPos[i]  = savedPos[i];
//            fGammaHitTime[i] = savedTime[i];
//            fGammaHitSet[i]  = savedSet[i];
//        }
//    }

//    MyRunAction* runAction = const_cast<MyRunAction*>(static_cast<const MyRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
//    if(runAction) runAction->AddEventData(fGammaCount, fTotalEnergy, fDecayX);
//}


//Try

//#include "event.hh"
//#include "run.hh"
//#include "G4RunManager.hh"
//#include "G4AnalysisManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4PhysicalConstants.hh"
//#include <algorithm>
//#include <cmath>

//MyEventAction::MyEventAction(MyRunAction*)
//    : G4UserEventAction(), fGammaCount(0), fFirstScinID(-1), fMinTime(1e9),
//      fDecayX(0.0), fDecayY(0.0), fDecayZ(0.0), fTotalEnergy(0.0),
//      fFormationTime(0.0), fDecayTime(0.0), fDecayDetected(false), fHitCount(0),
//      fGammaHitsRecorded(0)
//{
//    fEdepScin.assign(kNumModules, 0.0);
//    fHitTime .assign(kNumModules, 1e9);

//    for(G4int i = 0; i < 3; i++) {
//        fGammaEnergy[i]    = 0.0;
//        fGammaDirection[i] = G4ThreeVector(0,0,0);
//        fHitPos[i]         = G4ThreeVector(0,0,0);
//        fGammaHitPos[i]    = G4ThreeVector(0,0,0);
//        fGammaHitTime[i]   = 1e9;
//        fGammaHitSet[i]    = false;
//    }
//}

//MyEventAction::~MyEventAction() {}

//void MyEventAction::BeginOfEventAction(const G4Event*)
//{
//    fGammaCount = 0; fTotalEnergy = 0.0;
//    fDecayX = 0.0; fDecayY = 0.0; fDecayZ = 0.0;
//    fFirstScinID = -1; fMinTime = 1e9;
//    fFormationTime = 0.0; fDecayTime = 0.0; fDecayDetected = false; fHitCount = 0;
//    fGammaHitsRecorded = 0;

//    for(G4int i = 0; i < kNumModules; i++) {
//        fEdepScin[i] = 0.0; fHitTime [i] = 1e9;
//    }
//    for(G4int i = 0; i < 3; i++) {
//        fGammaEnergy[i]    = 0.0;
//        fGammaDirection[i] = G4ThreeVector(0,0,0);
//        fHitPos[i]         = G4ThreeVector(0,0,0);
//        fGammaHitPos[i]    = G4ThreeVector(0,0,0);
//        fGammaHitTime[i]   = 1e9;
//        fGammaHitSet[i]    = false;
//    }
//}

//void MyEventAction::AddGammaInfo(G4double energy, G4ThreeVector direction)
//{
//    if(fGammaCount < 3) {
//        fGammaEnergy[fGammaCount] = energy;
//        fGammaDirection[fGammaCount] = direction;
//        fTotalEnergy += energy; fGammaCount++;
//    }
//}

//void MyEventAction::AddHitPosition(G4ThreeVector pos) { if(fHitCount < 3) fHitPos[fHitCount++] = pos; }

//void MyEventAction::SetGammaHit(G4int gammaIndex, G4ThreeVector pos)
//{
//    if(gammaIndex >= 0 && gammaIndex < 3 && !fGammaHitSet[gammaIndex]) {
//        fGammaHitPos[gammaIndex] = pos;
//        fGammaHitSet[gammaIndex] = true;
//    }
//}

//void MyEventAction::SetGammaHitTime(G4int gammaIndex, G4double time)
//{
//    if(gammaIndex >= 0 && gammaIndex < 3) {
//        if(time < fGammaHitTime[gammaIndex]) fGammaHitTime[gammaIndex] = time;
//    }
//}

//void MyEventAction::AddEdep(G4int scinID, G4double edep, G4double time)
//{
//    if(scinID >= 0 && scinID < kNumModules) {
//        fEdepScin[scinID] += edep;
//        if(time < fHitTime[scinID]) fHitTime[scinID] = time;
//        if(time < fMinTime) { fMinTime = time; fFirstScinID = scinID; }
//    }
//}

//void MyEventAction::AddTotalEnergy(G4double energy) { fTotalEnergy = energy; }
//void MyEventAction::SetDecayPosition(G4double x, G4double y, G4double z) { fDecayX = x; fDecayY = y; fDecayZ = z; }

//// --- TRILATERATION RECONSTRUCTION METHOD ---
//G4ThreeVector MyEventAction::ReconstructVertex()
//{
//    if(!fGammaHitSet[0] || !fGammaHitSet[1] || !fGammaHitSet[2])
//        return G4ThreeVector(0,0,0);

//    const G4double c = c_light;
//    
//    G4ThreeVector H[3] = { fGammaHitPos[0], fGammaHitPos[1], fGammaHitPos[2] };
//    G4double t[3]      = { fGammaHitTime[0], fGammaHitTime[1], fGammaHitTime[2] };

//    G4double d12 = c * (t[0] - t[1]);
//    G4double d13 = c * (t[0] - t[2]);

//    G4ThreeVector V = (H[0] + H[1] + H[2]) / 3.0;
//    G4double damping = 0.1;

//    for(G4int iter = 0; iter < 50; iter++)
//    {
//        G4double r0 = (H[0] - V).mag();
//        G4double r1 = (H[1] - V).mag();
//        G4double r2 = (H[2] - V).mag();

//        if(r0 < 1e-3 || r1 < 1e-3 || r2 < 1e-3) break;

//        G4double f0 = (r0 - r1) - d12;
//        G4double f1 = (r0 - r2) - d13;

//        G4ThreeVector u0 = (V - H[0]).unit();
//        G4ThreeVector u1 = (V - H[1]).unit();
//        G4ThreeVector u2 = (V - H[2]).unit();

//        G4ThreeVector J0 = u0 - u1;
//        G4ThreeVector J1 = u0 - u2;

//        G4ThreeVector J_cross = J0.cross(J1);
//        G4double norm_sq = J_cross.mag2();
//        if(norm_sq < 1e-6) break;

//        G4ThreeVector delta = ( (f0 * J1.cross(J_cross)) - (f1 * J0.cross(J_cross)) ) / norm_sq;
//        V -= damping * delta; 
//        
//        if(delta.mag() < 0.01 * mm) break;
//    }
//    return V;
//}

//void MyEventAction::EndOfEventAction(const G4Event* event)
//{
//    G4AnalysisManager* man     = G4AnalysisManager::Instance();
//    G4int              eventID = event->GetEventID();

//    G4double totalEdep = 0.; G4int hitCount = 0;
//    for(G4int i = 0; i < kNumModules; i++) {
//        if(fEdepScin[i] > 0.) { totalEdep += fEdepScin[i]; hitCount++; }
//    }
//    G4double lifetime = fDecayTime - fFormationTime;

//    // Standard Histograms (H1: 0 - 14)
//    if(totalEdep > 0.) man->FillH1(0, totalEdep / keV);
//    for(G4int i = 201; i < kNumModules; i++) if(fEdepScin[i] > 0.) man->FillH1(1, static_cast<G4double>(i));
//    if(fGammaCount == 3 && fTotalEnergy > 0.) man->FillH1(2, fTotalEnergy / keV);
//    man->FillH1(3, fGammaCount == 3 ? 1. : 0.);
//    man->FillH1(4, static_cast<G4double>(fGammaCount));
//    if(fDecayTime > 0. && lifetime > 0.) man->FillH1(5, lifetime / ns);

//    if(fGammaCount >= 3) {
//        man->FillH1(6, std::atan2(fGammaDirection[0].x(), fGammaDirection[0].z()) / deg);
//        man->FillH1(7, std::atan2(fGammaDirection[1].y(), fGammaDirection[1].z()) / deg);
//        man->FillH1(8, std::atan2(fGammaDirection[2].y(), fGammaDirection[2].x()) / deg);
//    }
//    if(fGammaHitSet[0]) man->FillH1(9,  fGammaHitPos[0].z() / cm);
//    if(fGammaHitSet[1]) man->FillH1(10, fGammaHitPos[1].z() / cm);
//    if(fGammaHitSet[2]) man->FillH1(11, fGammaHitPos[2].z() / cm);
//    if(fDecayTime > 0. && fDecayDetected) {
//        man->FillH1(12, fDecayZ / cm); man->FillH1(13, fDecayZ / cm); man->FillH1(14, fDecayZ / cm);
//    }

//    // 2D Projections (H2: 0 - 2)
//    for(G4int i = 0; i < 3; i++) {
//        if(fGammaHitSet[i]) {
//            man->FillH2(0, fGammaHitPos[i].x()/cm, fGammaHitPos[i].y()/cm);
//            man->FillH2(1, fGammaHitPos[i].y()/cm, fGammaHitPos[i].z()/cm);
//            man->FillH2(2, fGammaHitPos[i].x()/cm, fGammaHitPos[i].z()/cm);
//        }
//    }

//    // H2(3) - Pure Kinematics Kinematic Sorting 
//    if (fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2]) {
//        G4ThreeVector decayPos(fDecayX, fDecayY, fDecayZ);
//        G4ThreeVector v1 = (fGammaHitPos[0] - decayPos).unit(), v2 = (fGammaHitPos[1] - decayPos).unit(), v3 = (fGammaHitPos[2] - decayPos).unit();
//        G4double theta12 = std::acos(std::max(-1.0, std::min(1.0, v1.dot(v2)))) / deg;
//        G4double theta23 = std::acos(std::max(-1.0, std::min(1.0, v2.dot(v3)))) / deg;
//        G4double theta31 = std::acos(std::max(-1.0, std::min(1.0, v3.dot(v1)))) / deg;
//        G4double angles[3] = {theta12, theta23, theta31}; std::sort(angles, angles + 3);
//        man->FillH2(3, angles[0] + angles[1], angles[1] - angles[0]);
//    }
//    else if (fGammaCount == 3) {
//        G4double t12 = std::acos(std::max(-1.0, std::min(1.0, fGammaDirection[0].dot(fGammaDirection[1])))) / deg;
//        G4double t23 = std::acos(std::max(-1.0, std::min(1.0, fGammaDirection[1].dot(fGammaDirection[2])))) / deg;
//        G4double t31 = std::acos(std::max(-1.0, std::min(1.0, fGammaDirection[2].dot(fGammaDirection[0])))) / deg;
//        G4double gen_angles[3] = {t12, t23, t31}; std::sort(gen_angles, gen_angles + 3);
//        man->FillH2(3, gen_angles[0] + gen_angles[1], gen_angles[1] - gen_angles[0]);
//    }

//    if(fGammaHitSet[0]) man->FillH2(4, fGammaHitPos[0].x()/cm, fGammaHitPos[0].z()/cm);
//    if(fGammaHitSet[1]) man->FillH2(5, fGammaHitPos[1].y()/cm, fGammaHitPos[1].z()/cm);
//    if(fGammaHitSet[2]) man->FillH2(6, fGammaHitPos[2].x()/cm, fGammaHitPos[2].y()/cm);

//    // Trilateration filling
//    if(fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2])
//    {
//        G4ThreeVector recoV = ReconstructVertex();

//        man->FillH1(15, recoV.x() / cm);
//        man->FillH1(16, recoV.y() / cm);
//        man->FillH1(17, recoV.z() / cm);
//        man->FillH2(7, recoV.x() / cm, recoV.y() / cm);
//        man->FillH2(8, recoV.x() / cm, recoV.z() / cm);

//        G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
//        man->FillH1(18, res / cm);
//    }
//    else if(fGammaCount == 3 && fDecayDetected)
//    {
//        G4double virtualRadius = 42.5 * cm; 
//        const G4double c = c_light;
//        G4bool validFallback = true;

//        G4ThreeVector savedPos[3];
//        G4double savedTime[3];
//        G4bool savedSet[3];

//        for(G4int i=0; i<3; i++) {
//            savedPos[i] = fGammaHitPos[i];
//            savedTime[i] = fGammaHitTime[i];
//            savedSet[i] = fGammaHitSet[i];

//            G4double r_xy = virtualRadius; 
//            G4double dir_transverse = std::sqrt(fGammaDirection[i].x()*fGammaDirection[i].x() + fGammaDirection[i].y()*fGammaDirection[i].y());
//            
//            if(dir_transverse > 1e-6) {
//                G4double distance = r_xy / dir_transverse;
//                fGammaHitPos[i] = G4ThreeVector(fDecayX, fDecayY, fDecayZ) + fGammaDirection[i] * distance;
//                fGammaHitTime[i] = (fGammaHitPos[i] - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag() / c;
//                fGammaHitSet[i] = true;
//            } else {
//                validFallback = false;
//            }
//        }

//        if(validFallback) {
//            G4ThreeVector recoV = ReconstructVertex();

//            man->FillH1(15, recoV.x() / cm);
//            man->FillH1(16, recoV.y() / cm);
//            man->FillH1(17, recoV.z() / cm);
//            man->FillH2(7, recoV.x() / cm, recoV.y() / cm);
//            man->FillH2(8, recoV.x() / cm, recoV.z() / cm);

//            G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
//            man->FillH1(18, res / cm);
//        }

//        for(G4int i=0; i<3; i++) {
//            fGammaHitPos[i] = savedPos[i];
//            fGammaHitTime[i] = savedTime[i];
//            fGammaHitSet[i] = savedSet[i];
//        }
//    }
//    
//    MyRunAction* runAction = const_cast<MyRunAction*>(static_cast<const MyRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
//    if(runAction) runAction->AddEventData(fGammaCount, fTotalEnergy, fDecayX);
//}
