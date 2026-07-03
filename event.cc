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
    : G4UserEventAction(),
      fGammaCount(0), fFirstScinID(-1), fMinTime(1e9),
      fDecayX(0.0), fDecayY(0.0), fDecayZ(0.0), fTotalEnergy(0.0),
      fFormationTime(0.0), fDecayTime(0.0), fDecayDetected(false),
      fHitCount(0), fGammaHitsRecorded(0),
      fNaturalGammaSlot(0),
      fPickOffParentID(-1),
      fPickOffSlot(0),
      fPickOffVolumeID(0),
      fNextFreeTrackID(1000000)
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

void MyEventAction::BeginOfEventAction(const G4Event*)
{
    fGammaCount = 0; fTotalEnergy = 0.0;
    fDecayX = 0.0; fDecayY = 0.0; fDecayZ = 0.0;
    fFirstScinID = -1; fMinTime = 1e9;
    fFormationTime = 0.0; fDecayTime = 0.0;
    fDecayDetected = false; fHitCount = 0;
    fGammaHitsRecorded = 0;

    ResetNaturalGammaSlot();
    fPickOffParentID = -1;
    fPickOffSlot = 0;
    fPickOffVolumeID = 0;
    fNextFreeTrackID = 1000000;

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
        fSmearedHitTime[i] = 1e9;
        fGammaHitTime[i]   = 1e9;
        fGammaHitSet[i]    = false;
    }
    ClearGammaIndexMap();
    fParentIDMap.clear();
}

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

void MyEventAction::SmearHit(G4ThreeVector& smearedPos, G4double& smearedTime,
                             const G4ThreeVector& truePos, G4double trueTime)
{
    // Smearing completely disabled to output ideal truth geometry profiles.
    smearedPos  = truePos;
    smearedTime = trueTime;
}

G4ThreeVector MyEventAction::ReconstructVertex()
{
    const G4double c = c_light;

    G4ThreeVector H[3] = { fSmearedHitPos[0], fSmearedHitPos[1], fSmearedHitPos[2] };
    G4double      t[3] = { fSmearedHitTime[0], fSmearedHitTime[1], fSmearedHitTime[2] };

    G4double d12 = c * (t[0] - t[1]);
    G4double d13 = c * (t[0] - t[2]);

    G4ThreeVector V       = (H[0] + H[1] + H[2]) / 3.0;
    G4double      damping = 0.1;

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

G4ThreeVector MyEventAction::ReconstructVertex2Gamma()
{
    G4ThreeVector posA = fSmearedHitPos[0];
    G4ThreeVector posB = fSmearedHitPos[1];
    G4double tA = fSmearedHitTime[0];
    G4double tB = fSmearedHitTime[1];

    G4ThreeVector midpoint = (posA + posB) / 2.0;
    G4ThreeVector LOR      = (posB - posA).unit();

    G4double delta_t = tA - tB;
    G4double dist    = c_light * delta_t / 2.0;

    return midpoint + dist * LOR;
}

static G4double SafeAngleDeg(G4ThreeVector a, G4ThreeVector b)
{
    G4double c = a.unit().dot(b.unit());
    c = std::max(-1.0, std::min(1.0, c));
    return std::acos(c) / CLHEP::deg;
}

void MyEventAction::FillRecoHistograms(const G4ThreeVector& recoV,
                                      G4int, G4AnalysisManager* man,
                                      G4bool isTrue)
{
    G4double x = recoV.x() / cm;
    G4double y = recoV.y() / cm;
    G4double z = recoV.z() / cm;

    man->FillH1(man->GetH1Id("fRecoVertexX"), recoV.x() / cm);
    man->FillH1(man->GetH1Id("fRecoVertexY"), recoV.y() / cm);
    man->FillH1(man->GetH1Id("fRecoVertexZ"), recoV.z() / cm);

    G4int xyID = man->GetH2Id("fRecoVertex_XY");
    if (xyID >= 0) {
        man->FillH2(xyID, x, y);
    }

    man->FillH2(man->GetH2Id("fRecoVertex_XY"), recoV.x() / cm, recoV.y() / cm);
    man->FillH2(man->GetH2Id("fRecoVertex_XZ"), recoV.x() / cm, recoV.z() / cm);

    // NEW: 3G Y-Z fringe/consistency projection (detector-level / smeared)
    G4int recoYZID = man->GetH2Id("fRecoVertex_YZ");
    if(recoYZID >= 0) man->FillH2(recoYZID, recoV.y() / cm, recoV.z() / cm);

    G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
    man->FillH1(man->GetH1Id("fVertexResolution"), res / cm);

    // Fill the new 2D XY projection
    man->FillH2(man->GetH2Id("fRecoVertex_XY"), recoV.x() / cm, recoV.y() / cm);

    G4ThreeVector d1 = (fSmearedHitPos[0] - recoV).unit();
    G4ThreeVector d2 = (fSmearedHitPos[1] - recoV).unit();
    G4ThreeVector d3 = (fSmearedHitPos[2] - recoV).unit();

    G4double a12 = SafeAngleDeg(d1, d2);
    G4double a23 = SafeAngleDeg(d2, d3);
    G4double a13 = SafeAngleDeg(d1, d3);

    G4double angs[3] = {a12, a23, a13};
    std::sort(angs, angs + 3);

    man->FillH1(man->GetH1Id("fHitAngle_a"), angs[0]);
    man->FillH1(man->GetH1Id("fHitAngle_b"), angs[1]);
    man->FillH1(man->GetH1Id("fHitAngle_c"), angs[2]);
    man->FillH2(man->GetH2Id("fHitAngle"), angs[0] + angs[1], angs[1] - angs[0]);

    if(isTrue) {
        man->FillH1(man->GetH1Id("fRecoVertexX_True"), recoV.x() / cm);
        man->FillH1(man->GetH1Id("fRecoVertexY_True"), recoV.y() / cm);
        man->FillH1(man->GetH1Id("fRecoVertexZ_True"), recoV.z() / cm);
        man->FillH1(man->GetH1Id("fVertexResolution_True"), res / cm);
        man->FillH2(man->GetH2Id("fRecoVertex_XY_True"), recoV.x() / cm, recoV.y() / cm);
        man->FillH2(man->GetH2Id("fRecoVertex_XZ_True"), recoV.x() / cm, recoV.z() / cm);

        // NEW: 3G Y-Z fringe/consistency projection (generator-level / true)
        G4int recoYZTrueID = man->GetH2Id("fRecoVertex_YZ_True");
        if(recoYZTrueID >= 0) man->FillH2(recoYZTrueID, recoV.y() / cm, recoV.z() / cm);

        man->FillH2(man->GetH2Id("fHitAngle_True"), angs[0] + angs[1], angs[1] - angs[0]);
        man->FillH3(man->GetH3Id("f3G_RecoVertex_3D"),
                    recoV.x() / cm, recoV.y() / cm, recoV.z() / cm);
    }
}

void MyEventAction::Fill2GammaHistograms(const G4ThreeVector& recoV,
                                        G4AnalysisManager* man)
{
    // Standard 2G histograms (all pick-off)
    man->FillH1(man->GetH1Id("f2G_RecoVertexX"), recoV.x() / cm);
    man->FillH1(man->GetH1Id("f2G_RecoVertexY"), recoV.y() / cm);
    man->FillH1(man->GetH1Id("f2G_RecoVertexZ"), recoV.z() / cm);

    G4double res = (recoV - G4ThreeVector(fDecayX, fDecayY, fDecayZ)).mag();
    man->FillH1(man->GetH1Id("f2G_Resolution"), res / cm);

    G4double openAngle = SafeAngleDeg(fGammaDirection[0], fGammaDirection[1]);
    man->FillH1(man->GetH1Id("f2G_OpeningAngle"), openAngle);

    // Broad pooled maps
    man->FillH2(man->GetH2Id("fFringePatternXY"), recoV.x() / cm, recoV.y() / cm);

    // FIXED: this was previously filling X vs Z into a histogram labeled/binned
    // as XY ("f2G_RecoVertex_XY"), which is almost certainly the source of the
    // entry-count mismatch vs fFringeRecoVertexXY (this one fires for every
    // pick-off volume, not just the Counter). Now correctly fills X vs Y, and
    // the histogram itself uses the same ±6 cm / 200-bin grid as the other
    // fringe views (see run.cc) for direct comparability.
    man->FillH2(man->GetH2Id("f2G_RecoVertex_XY"), recoV.x() / cm, recoV.y() / cm);

    // X-Z fringe view (all pick-off origins pooled), matching the ±6 cm /
    // ±25 cm binning of f2G_RecoVertex_3D.
    G4int recoXZID = man->GetH2Id("f2G_RecoVertex_XZ");
    if(recoXZID >= 0) man->FillH2(recoXZID, recoV.x() / cm, recoV.z() / cm);

    // NEW: Y-Z fringe view (all pick-off origins pooled), same binning as
    // f2G_RecoVertex_XZ, so true vs. reco and X vs. Y fringe structure can
    // all be compared on a common grid.
    G4int recoYZID = man->GetH2Id("f2G_RecoVertex_YZ");
    if(recoYZID >= 0) man->FillH2(recoYZID, recoV.y() / cm, recoV.z() / cm);

    man->FillH3(man->GetH3Id("f2G_RecoVertex_3D"),
                recoV.x() / cm, recoV.y() / cm, recoV.z() / cm);

    // -----------------------------------------------------------------
    // Per-volume gated fills (Now featuring micro-2D matrix validation)
    // -----------------------------------------------------------------
    if(fPickOffVolumeID == 1)   // Grating 1 wall
    {
        G4int id = man->GetH1Id("fWall1_RecoX");
        if(id >= 0) man->FillH1(id, recoV.x() / cm);

        // Fill high resolution 2D micro map to see specific discrete stripes
        G4int microID = man->GetH2Id("fMicroGrating2D");
        if(microID >= 0) man->FillH2(microID, recoV.x() / cm, recoV.y() / cm);
    }
    else if(fPickOffVolumeID == 2)  // Grating 2 wall
    {
        G4int id = man->GetH1Id("fWall2_RecoX");
        if(id >= 0) man->FillH1(id, recoV.x() / cm);
    }
    else if(fPickOffVolumeID == 3)  // Counter — Moiré fringe pattern
    {
        G4int idY  = man->GetH1Id("fFringeRecoVertexY");
        if(idY >= 0) man->FillH1(idY, recoV.y() / cm);

        G4int idX  = man->GetH1Id("fFringeRecoVertexX");
        if(idX >= 0) man->FillH1(idX, recoV.x() / cm);

        G4int idXY = man->GetH2Id("fFringeRecoVertexXY");
        if(idXY >= 0) man->FillH2(idXY, recoV.x() / cm, recoV.y() / cm);
    }
}

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

    G4bool is2GammaPickOff = (fGammaHitSet[0] && fGammaHitSet[1] && !fGammaHitSet[2]);
    G4bool is3GammaNatural = (fGammaHitSet[0] && fGammaHitSet[1] && fGammaHitSet[2]);

    // FIX: Dynamically find which exact gamma indices were hit
    std::vector<G4int> validIndices;
    for(G4int i = 0; i < 3; i++) {
        if(fGammaHitSet[i]) {
            validIndices.push_back(i);
        }
    }

    if(is3GammaNatural && validIndices.size() == 3)
    {
        for(G4int i = 0; i < 3; i++) {
            fSmearedHitPos[i]  = fGammaHitPos[i];
            fSmearedHitTime[i] = fGammaHitTime[i];
        }
    }
    else if(is2GammaPickOff && validIndices.size() == 2)
    {
        // Extract the two true hitting gamma indices to avoid (0,0,0) ghost hits
        G4int idxA = validIndices[0];
        G4int idxB = validIndices[1];

        fSmearedHitPos[0]  = fGammaHitPos[idxA];
        fSmearedHitTime[0] = fGammaHitTime[idxA];

        fSmearedHitPos[1]  = fGammaHitPos[idxB];
        fSmearedHitTime[1] = fGammaHitTime[idxB];

        // Map primary photon directions to elements 0 and 1 for accurate angular cuts
        fGammaDirection[0] = fGammaDirection[idxA];
        fGammaDirection[1] = fGammaDirection[idxB];
    }

    if(totalEdep > 0.) man->FillH1(man->GetH1Id("fEdep"), totalEdep / keV);

    for(G4int i = 0; i < kNumModules; i++)
        if(fEdepScin[i] > 0.)
            man->FillH1(man->GetH1Id("fScinID"), static_cast<G4double>(i));

    if(fGammaCount == 3 && fTotalEnergy > 0.)
        man->FillH1(man->GetH1Id("fTotalEnergy"), fTotalEnergy / keV);

    man->FillH1(man->GetH1Id("fOrthops"),          is3GammaNatural ? 1. : 0.);
    man->FillH1(man->GetH1Id("fGammaMultiplicity"), static_cast<G4double>(fGammaCount));

    if(fDecayTime > 0. && lifetime > 0.)
        man->FillH1(man->GetH1Id("fLifetime"), lifetime / ns);

    // Dynamic Angular Distributions (1D)
    if(fGammaCount >= 3) {
        man->FillH1(man->GetH1Id("fGammaDir_XZ"),
                    std::atan2(fGammaDirection[0].x(), fGammaDirection[0].z()) / deg);
        man->FillH1(man->GetH1Id("fGammaDir_YZ"),
                    std::atan2(fGammaDirection[1].y(), fGammaDirection[1].z()) / deg);
        man->FillH1(man->GetH1Id("fGammaDir_XY"),
                    std::atan2(fGammaDirection[2].y(), fGammaDirection[2].x()) / deg);
    }

    if(fGammaHitSet[0]) man->FillH1(man->GetH1Id("fGamma1_HitZ"), fGammaHitPos[0].z() / cm);
    if(fGammaHitSet[1]) man->FillH1(man->GetH1Id("fGamma2_HitZ"), fGammaHitPos[1].z() / cm);
    if(fGammaHitSet[2]) man->FillH1(man->GetH1Id("fGamma3_HitZ"), fGammaHitPos[2].z() / cm);

    if(fDecayTime > 0. && fDecayDetected) {
        man->FillH1(man->GetH1Id("fGamma1_DecayZ"), fDecayZ / cm);
        man->FillH1(man->GetH1Id("fGamma2_DecayZ"), fDecayZ / cm);
        man->FillH1(man->GetH1Id("fGamma3_DecayZ"), fDecayZ / cm);
    }

    for(G4int i = 0; i < 3; i++) {
        if(fGammaHitSet[i]) {
            man->FillH2(man->GetH2Id("fHitMap_XY"),
                        fGammaHitPos[i].x()/cm, fGammaHitPos[i].y()/cm);
            man->FillH2(man->GetH2Id("fHitMap_YZ"),
                        fGammaHitPos[i].y()/cm, fGammaHitPos[i].z()/cm);
            man->FillH2(man->GetH2Id("fHitMap_XZ"),
                        fGammaHitPos[i].x()/cm, fGammaHitPos[i].z()/cm);
        }
    }

    if(is3GammaNatural)
    {
        G4ThreeVector decayPos(fDecayX, fDecayY, fDecayZ);
        G4ThreeVector v1 = (fGammaHitPos[0] - decayPos).unit();
        G4ThreeVector v2 = (fGammaHitPos[1] - decayPos).unit();
        G4ThreeVector v3 = (fGammaHitPos[2] - decayPos).unit();

        G4double t12 = SafeAngleDeg(v1, v2);
        G4double t23 = SafeAngleDeg(v2, v3);
        G4double t31 = SafeAngleDeg(v3, v1);

        G4double angles[3] = {t12, t23, t31};
        std::sort(angles, angles + 3);
        man->FillH2(man->GetH2Id("fAnglePlot"),
                    angles[0] + angles[1], angles[1] - angles[0]);
    }
    else if(fGammaCount == 3)
    {
        G4double t12 = SafeAngleDeg(fGammaDirection[0], fGammaDirection[1]);
        G4double t23 = SafeAngleDeg(fGammaDirection[1], fGammaDirection[2]);
        G4double t31 = SafeAngleDeg(fGammaDirection[2], fGammaDirection[0]);
        G4double gen[3] = {t12, t23, t31};
        std::sort(gen, gen + 3);
        man->FillH2(man->GetH2Id("fAnglePlot"),
                    gen[0] + gen[1], gen[1] - gen[0]);
    }

    if(fGammaHitSet[0])
        man->FillH2(man->GetH2Id("fGamma1_XZ"),
                    fGammaHitPos[0].x()/cm, fGammaHitPos[0].z()/cm);
    if(fGammaHitSet[1])
        man->FillH2(man->GetH2Id("fGamma2_YZ"),
                    fGammaHitPos[1].y()/cm, fGammaHitPos[1].z()/cm);
    if(fGammaHitSet[2])
        man->FillH2(man->GetH2Id("fGamma3_XY"),
                    fGammaHitPos[2].x()/cm, fGammaHitPos[2].y()/cm);

    if(is3GammaNatural)
    {
        G4ThreeVector recoV = ReconstructVertex();
        recoV.setZ(fDecayZ);
        FillRecoHistograms(recoV, eventID, man, true);
    }
    else if(fGammaCount == 3 && fDecayDetected)
    {
        G4double virtualRadius = 38.5 * cm;
        const G4double c = c_light;
        G4bool validFallback = true;

        G4ThreeVector savedSmeared[3];
        G4double      savedTime[3];
        G4bool        savedSet[3];
        for(G4int i = 0; i < 3; i++) {
            savedSmeared[i] = fSmearedHitPos[i];
            savedTime[i]    = fGammaHitTime[i];
            savedSet[i]     = fGammaHitSet[i];
        }

        for(G4int i = 0; i < 3; i++) {
            G4double dir_t = std::sqrt(fGammaDirection[i].x()*fGammaDirection[i].x() +
                                       fGammaDirection[i].y()*fGammaDirection[i].y());
            if(dir_t > 1e-6) {
                G4double dist = virtualRadius / dir_t;
                G4ThreeVector vHit =
                    G4ThreeVector(fDecayX, fDecayY, fDecayZ) + fGammaDirection[i] * dist;
                G4double vHitTime =
                    (vHit - G4ThreeVector(fDecayX,fDecayY,fDecayZ)).mag() / c;

                fSmearedHitPos[i] = vHit;
                fSmearedHitTime[i] = vHitTime;

                fGammaHitSet[i] = true;
            } else {
                validFallback = false;
            }
        }

        if(validFallback) {
            G4ThreeVector recoV = ReconstructVertex();
            recoV.setZ(fDecayZ);
            FillRecoHistograms(recoV, eventID, man, false);
        }

        for(G4int i = 0; i < 3; i++) {
            fSmearedHitPos[i] = savedSmeared[i];
            fGammaHitTime[i]  = savedTime[i];
            fGammaHitSet[i]   = savedSet[i];
        }
    }

    if(is2GammaPickOff)
    {
        // FIX: Verify the two gammas are truly back-to-back to filter out 3G background mist
        G4double openAngle = SafeAngleDeg(fGammaDirection[0], fGammaDirection[1]);
        if(openAngle > 175.0)
        {
            G4ThreeVector recoV = ReconstructVertex2Gamma();
            Fill2GammaHistograms(recoV, man);

            // Generator-level truth fill: the actual o-Ps decay vertex for this
            // event, independent of TOF reconstruction quality.
            G4int trueID = man->GetH3Id("f2G_TrueVertex_3D");
            if(trueID >= 0)
                man->FillH3(trueID, fDecayX / cm, fDecayY / cm, fDecayZ / cm);

            // Generator-level X-Y and X-Z projections, matching the
            // f2G_RecoVertex_XY / f2G_RecoVertex_XZ views above so true vs.
            // TOF-reconstructed fringe visibility can be compared directly.
            G4int trueXYID = man->GetH2Id("f2G_TrueVertex_XY");
            if(trueXYID >= 0) man->FillH2(trueXYID, fDecayX / cm, fDecayY / cm);

            G4int trueXZID = man->GetH2Id("f2G_TrueVertex_XZ");
            if(trueXZID >= 0) man->FillH2(trueXZID, fDecayX / cm, fDecayZ / cm);

            // NEW: generator-level Y-Z projection, matching f2G_RecoVertex_YZ,
            // completing the true-vs-reco comparison set on all three planes.
            G4int trueYZID = man->GetH2Id("f2G_TrueVertex_YZ");
            if(trueYZID >= 0) man->FillH2(trueYZID, fDecayY / cm, fDecayZ / cm);
        }
    }

    MyRunAction* runAction = const_cast<MyRunAction*>(
        static_cast<const MyRunAction*>(
            G4RunManager::GetRunManager()->GetUserRunAction()));

    if(runAction) {
        if(is3GammaNatural) {
            runAction->AddEventData(3, fTotalEnergy, fDecayZ);
            runAction->AddTrueCoincidence();
        }
        else if(is2GammaPickOff) {
            runAction->AddEventData(2, fTotalEnergy, fDecayZ);
            runAction->Add2GammaEvent();
        }
        else {
            runAction->AddEventData(fGammaCount, fTotalEnergy, fDecayZ);
        }
    }
}
