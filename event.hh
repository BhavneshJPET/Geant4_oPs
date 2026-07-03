#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include <algorithm>
#include <map>

class MyRunAction;

class MyEventAction : public G4UserEventAction
{
public:
    MyEventAction(MyRunAction*);
    ~MyEventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction  (const G4Event*);

    void AddEdep         (G4int scinID, G4double edep, G4double time);
    void AddGammaInfo    (G4double energy, G4ThreeVector direction);
    void SetDecayPosition(G4double x, G4double y, G4double z);
    void AddTotalEnergy  (G4double energy);
    void AddHitPosition  (G4ThreeVector pos);
    void SetGammaHit     (G4int gammaIndex, G4ThreeVector pos);
    void SetGammaHitTime (G4int gammaIndex, G4double time);

    void SetFormationTime(G4double t) { fFormationTime = t; }
    void SetDecayTime    (G4double t) { fDecayTime     = t; }
    void SetDecayZ       (G4double z) { fDecayZ        = z; }
    void SetDecayDetected(G4bool   b) { fDecayDetected = b; }

    // ── Gamma index map ───────────────────────────────────────────
    void SetGammaIndex(G4int trackID, G4int gammaIdx) {
        fTrackToGammaIdx[trackID] = gammaIdx;
    }
    G4int GetGammaIndex(G4int trackID) {
        auto it = fTrackToGammaIdx.find(trackID);
        if(it != fTrackToGammaIdx.end()) return it->second;
        return -1;
    }
    void ClearGammaIndexMap() { fTrackToGammaIdx.clear(); }

    // ── Parent ID map — needed to walk up Compton chain ──────────
    void StoreParentID(G4int trackID, G4int parentID) {
        fParentIDMap[trackID] = parentID;
    }
    G4int GetStoredParentID(G4int trackID) {
        auto it = fParentIDMap.find(trackID);
        if(it != fParentIDMap.end()) return it->second;
        return -1;
    }

    // ── Natural 3-Gamma Slot Tracker (CRITICAL FIX) ───────────────
    // Dynamically assigns positions 0, 1, or 2 to escaping natural 
    // annihilation gammas without assuming static sequential Track IDs.
    void ResetNaturalGammaSlot() { fNaturalGammaSlot = 0; }
    G4int GetNextNaturalGammaSlot() {
        return (fNaturalGammaSlot < 3) ? fNaturalGammaSlot++ : -1;
    }

    // ── Pick-off Parent Track Tracking ────────────────────────────
    void  SetPickOffParentID(G4int id) { fPickOffParentID = id; fPickOffSlot = 0; }
    G4int GetPickOffParentID() const   { return fPickOffParentID; }
    G4int GetNextPickOffSlot() {
        return (fPickOffSlot < 2) ? fPickOffSlot++ : -1;
    }

    // ── Pick-off ORIGIN volume tracking ───────────────────────────
    void  SetPickOffVolumeID(G4int id) { fPickOffVolumeID = id; }
    G4int GetPickOffVolumeID() const   { return fPickOffVolumeID; }

    // ── Unique TrackID issuer for manually-pushed tracks ─────────
    G4int GetNextFreeTrackID() { return fNextFreeTrackID++; }

    // ── General accessors ─────────────────────────────────────────
    G4int    GetGammaCount()    const { return fGammaCount;    }
    G4double GetTotalEnergy()   const { return fTotalEnergy;   }
    G4double GetDecayX()        const { return fDecayX;        }
    G4double GetFormationTime() const { return fFormationTime; }

    G4int GetNextGammaIndex() {
        if(fGammaHitsRecorded < 3) return fGammaHitsRecorded++;
        return -1;
    }
    void ResetGammaHitsRecorded() { fGammaHitsRecorded = 0; }

    // ── Reconstruction ────────────────────────────────────────────
    G4ThreeVector ReconstructVertex();        // 3-gamma trilateration
    G4ThreeVector ReconstructVertex2Gamma();  // 2-gamma TOF-LOR

    static const G4int kNumModules = 312;

private:
    void SmearHit(G4ThreeVector& smearedPos, G4double& smearedTime,
                  const G4ThreeVector& truePos, G4double trueTime);

    void FillRecoHistograms(const G4ThreeVector& recoV,
                             G4int eventID,
                             G4AnalysisManager* man,
                             G4bool isTrue);

    void Fill2GammaHistograms(const G4ThreeVector& recoV,
                               G4AnalysisManager* man);

    // ── Scintillator arrays ───────────────────────────────────────
    std::vector<G4double> fEdepScin;
    std::vector<G4double> fHitTime;
    G4int                  fFirstScinID;
    G4double              fMinTime;

    // ── Gamma kinematics ──────────────────────────────────────────
    G4int         fGammaCount;
    G4double      fGammaEnergy[3];
    G4ThreeVector fGammaDirection[3];

    // ── Decay vertex ─────────────────────────────────────────────
    G4double fDecayX, fDecayY, fDecayZ;

    // ── Energy / timing ──────────────────────────────────────────
    G4double fTotalEnergy;
    G4double fFormationTime;
    G4double fDecayTime;
    G4bool   fDecayDetected;

    // ── Raw hit positions ─────────────────────────────────────────
    G4ThreeVector fHitPos[3];
    G4int         fHitCount;

    // ── Per-gamma positions ───────────────────────────────────────
    G4ThreeVector fGammaHitPos[3];
    G4ThreeVector fSmearedHitPos[3];
    G4double      fSmearedHitTime[3];
    G4double      fGammaHitTime[3];
    G4bool        fGammaHitSet[3];

    G4int fGammaHitsRecorded;

    // ── Track maps ───────────────────────────────────────────────
    std::map<G4int, G4int> fTrackToGammaIdx;  // trackID → gamma slot
    std::map<G4int, G4int> fParentIDMap;       // trackID → parentID

    // ── Natural Decay Tracking State ──────────────────────────────
    G4int fNaturalGammaSlot;                  // Current counter track for 3G slots

    // ── Pick-off Tracking Variables ────────────────────────────────
    G4int fPickOffParentID;
    G4int fPickOffSlot;
    G4int fPickOffVolumeID;  

    // ── Unique trackID issuer state ──────────────────────────────
    G4int fNextFreeTrackID;
};

#endif
