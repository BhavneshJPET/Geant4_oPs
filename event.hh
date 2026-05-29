//#ifndef EVENT_HH
//#define EVENT_HH

//#include "G4UserEventAction.hh"
//#include "G4Event.hh"
//#include "run.hh"
//#include "G4SystemOfUnits.hh"
//#include <vector> // Ensure this is included for std::vector

//class MyEventAction : public G4UserEventAction
//{
//public:
//    MyEventAction(MyRunAction*);
//    ~MyEventAction();
//    
//    virtual void BeginOfEventAction(const G4Event*);
//    virtual void EndOfEventAction(const G4Event*);
//    
//    // Added the missing '}' below to close the function
//    void AddEdep(G4int scinID, G4double edep) { 
//        if(scinID >= 0 && scinID < 400) fEdepScin[scinID] += edep;
//    } 

//private:
//    std::vector<G4double> fEdepScin;
//};

//#endif


// O-PS
//#ifndef EVENT_HH
//#define EVENT_HH

//#include "G4UserEventAction.hh"
//#include "G4Event.hh"
//#include "G4SystemOfUnits.hh"
//#include <vector>

//class MyRunAction; // Forward declaration

//class MyEventAction : public G4UserEventAction
//{
//public:
//    MyEventAction(MyRunAction*);
//    ~MyEventAction();
//    
//    virtual void BeginOfEventAction(const G4Event*);
//    virtual void EndOfEventAction(const G4Event*);
//    
//    // Declaration only - logic moves to .cc
//    void AddEdep(G4int scinID, G4double edep, G4double time);
//    void AddDecayPosition(G4double x) { fDecayX = x; }


//#ifndef EVENT_HH
//#define EVENT_HH

//#include "G4UserEventAction.hh"
//#include "G4Event.hh"
//#include "G4SystemOfUnits.hh"
//#include <vector>

//class MyRunAction; // Forward declaration

//class MyEventAction : public G4UserEventAction
//{
//public:
//    MyEventAction(MyRunAction*);
//    ~MyEventAction();
//    
//    virtual void BeginOfEventAction(const G4Event*);
//    virtual void EndOfEventAction(const G4Event*);
//    
//    // Declaration only - logic moves to .cc
//    void AddEdep(G4int scinID, G4double edep, G4double time);
//    void AddDecayPosition(G4double x);
//    void AddTotalEnergy(G4double energy);

//private:
//    std::vector<G4double> fEdepScin;
//    
//    G4int fFirstScinID;
//    G4double fMinTime;
//    G4double fDecayX;
//    G4double fTotalEnergy;
//};

//#endif


//DEEP


//#ifndef EVENT_HH
//#define EVENT_HH

//#include "G4UserEventAction.hh"
//#include "G4Event.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ThreeVector.hh"
//#include <vector>
//#include <algorithm>

//class MyRunAction;

//class MyEventAction : public G4UserEventAction
//{
//public:
//    MyEventAction(MyRunAction*);
//    ~MyEventAction();

//    virtual void BeginOfEventAction(const G4Event*);
//    virtual void EndOfEventAction(const G4Event*);

//    void AddEdep(G4int scinID, G4double edep, G4double time);
//    void AddGammaInfo(G4double energy, G4ThreeVector direction);
//    void AddDecayPosition(G4double x);
//    void AddTotalEnergy(G4double energy);

//    // Hit position for kinematics angle plot
//    void AddHitPosition(G4ThreeVector pos);
//    
//    void AddGammaHitZ(G4double z);

//    void SetFormationTime(G4double t) { fFormationTime = t; }
//    void SetDecayTime(G4double t)     { fDecayTime = t; }

//    G4int         GetGammaCount()     const { return fGammaCount; }
//    G4double      GetTotalEnergy()    const { return fTotalEnergy; }
//    G4double      GetDecayX()         const { return fDecayX; }
//    G4double      GetFormationTime()  const { return fFormationTime; }

//    // FIXED: 513 covers copy numbers 201-512
//    static const G4int kNumModules = 513;

//private:
//    std::vector<G4double> fEdepScin;
//    std::vector<G4double> fHitTime;

//    G4int         fGammaCount;
//    G4double      fGammaEnergy[3];
//    G4ThreeVector fGammaDirection[3];

//    G4int    fFirstScinID;
//    G4double fMinTime;
//    G4double fDecayX;
//    G4double fTotalEnergy;
//    G4double fFormationTime;
//    G4double fDecayTime;

//    // Store Z hit position per gamma
//    G4double fGammaHitZ[3];
//    G4int    fGammaHitCount;  // track which gamma hit

//    // ====== FIXED: ADDED MISSING HIT TRACKING VARIABLES ======
//    G4int         fHitCount;   // Tracks number of recorded hit positions (max 3)
//    G4ThreeVector fHitPos[3];  // Stores the 3D position vector for each hit
//    // =========================================================
//};
// #endif




//#ifndef EVENT_HH
//#define EVENT_HH

//#include "G4UserEventAction.hh"
//#include "G4Event.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ThreeVector.hh"
//#include <vector>
//#include <algorithm>

//class MyRunAction;

//class MyEventAction : public G4UserEventAction
//{
//public:
//    MyEventAction(MyRunAction*);
//    ~MyEventAction();

//    virtual void BeginOfEventAction(const G4Event*);
//    virtual void EndOfEventAction(const G4Event*);

//    void AddEdep(G4int scinID, G4double edep, G4double time);
//    void AddGammaInfo(G4double energy, G4ThreeVector direction);
//    //void AddDecayPosition(G4double x);
//    
//    void SetDecayPosition(
//    G4double x,
//    G4double y,
//    G4double z);
//    
//    void AddTotalEnergy(G4double energy);
//    void AddHitPosition(G4ThreeVector pos);
//    void SetGammaHit(G4int gammaIndex, G4ThreeVector pos);

//    void SetFormationTime(G4double t)  { fFormationTime  = t; }
//    void SetDecayTime(G4double t)      { fDecayTime      = t; }
//    void SetDecayZ(G4double z)         { fDecayZ         = z; }
//    void SetDecayDetected(G4bool b)    { fDecayDetected  = b; }  // NEW

//    G4int         GetGammaCount()    const { return fGammaCount; }
//    G4double      GetTotalEnergy()   const { return fTotalEnergy; }
//    G4double      GetDecayX()        const { return fDecayX; }
//    G4double      GetFormationTime() const { return fFormationTime; }

//    static const G4int kNumModules = 513;

//private:
//    std::vector<G4double> fEdepScin;
//    std::vector<G4double> fHitTime;

//    G4int         fGammaCount;
//    G4double      fGammaEnergy[3];
//    G4ThreeVector fGammaDirection[3];

//    G4int    fFirstScinID;
//    G4double fMinTime;
//    G4double fDecayX;
//    
//    G4double fDecayY;
//    
//    G4double fDecayZ;
//    G4double fTotalEnergy;
//    G4double fFormationTime;
//    G4double fDecayTime;
//    G4bool   fDecayDetected;   // NEW: true only when decay step is caught

//    G4ThreeVector fHitPos[3];
//    G4int         fHitCount;

//    G4ThreeVector fGammaHitPos[3];
//    G4bool        fGammaHitSet[3];
//};

//#endif


//#ifndef EVENT_HH
//#define EVENT_HH

//#include "G4UserEventAction.hh"
//#include "G4Event.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ThreeVector.hh"
//#include <vector>
//#include <algorithm>

//class MyRunAction;

//class MyEventAction : public G4UserEventAction
//{
//public:
//    MyEventAction(MyRunAction*);
//    ~MyEventAction();

//    virtual void BeginOfEventAction(const G4Event*);
//    virtual void EndOfEventAction(const G4Event*);

//    void AddEdep(G4int scinID, G4double edep, G4double time);
//    void AddGammaInfo(G4double energy, G4ThreeVector direction);
//    
//    void SetDecayPosition(G4double x, G4double y, G4double z);
//    void AddTotalEnergy(G4double energy);
//    void AddHitPosition(G4ThreeVector pos);
//    void SetGammaHit(G4int gammaIndex, G4ThreeVector pos);
//    void SetGammaHitTime(G4int gammaIndex, G4double time); // NEW

//    void SetFormationTime(G4double t)  { fFormationTime  = t; }
//    void SetDecayTime(G4double t)      { fDecayTime      = t; }
//    void SetDecayZ(G4double z)         { fDecayZ         = z; }
//    void SetDecayDetected(G4bool b)    { fDecayDetected  = b; } 

//    G4int         GetGammaCount()    const { return fGammaCount; }
//    G4double      GetTotalEnergy()   const { return fTotalEnergy; }
//    G4double      GetDecayX()        const { return fDecayX; }
//    G4double      GetFormationTime() const { return fFormationTime; }

//    G4ThreeVector ReconstructVertex(); // NEW: Robust analytical/numerical solver

//    static const G4int kNumModules = 513;

//private:
//    std::vector<G4double> fEdepScin;
//    std::vector<G4double> fHitTime;

//    G4int         fGammaCount;
//    G4double      fGammaEnergy[3];
//    G4ThreeVector fGammaDirection[3];

//    G4int    fFirstScinID;
//    G4double fMinTime;
//    G4double fDecayX;
//    G4double fDecayY;
//    G4double fDecayZ;
//    G4double fTotalEnergy;
//    G4double fFormationTime;
//    G4double fDecayTime;
//    G4bool   fDecayDetected;   

//    G4ThreeVector fHitPos[3];
//    G4int         fHitCount;

//    G4ThreeVector fGammaHitPos[3];
//    G4double      fGammaHitTime[3]; // NEW
//    G4int         fGammaHitsRecorded; 
//    G4bool        fGammaHitSet[3];
//};

//#endif




//Try

#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include <algorithm>

class MyRunAction;

class MyEventAction : public G4UserEventAction
{
public:
    MyEventAction(MyRunAction*);
    ~MyEventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction  (const G4Event*);

    void AddEdep        (G4int scinID, G4double edep, G4double time);
    void AddGammaInfo   (G4double energy, G4ThreeVector direction);
    void SetDecayPosition(G4double x, G4double y, G4double z);
    void AddTotalEnergy (G4double energy);
    void AddHitPosition (G4ThreeVector pos);
    void SetGammaHit    (G4int gammaIndex, G4ThreeVector pos);
    void SetGammaHitTime(G4int gammaIndex, G4double time);

    void SetFormationTime(G4double t) { fFormationTime = t; }
    void SetDecayTime    (G4double t) { fDecayTime     = t; }
    void SetDecayZ       (G4double z) { fDecayZ        = z; }
    void SetDecayDetected(G4bool   b) { fDecayDetected = b; }

    G4int    GetGammaCount()    const { return fGammaCount;    }
    G4double GetTotalEnergy()   const { return fTotalEnergy;   }
    G4double GetDecayX()        const { return fDecayX;        }
    G4double GetFormationTime() const { return fFormationTime; }

    // Counter for ordering gamma hits in arrival sequence
    G4int GetNextGammaIndex() {
        if(fGammaHitsRecorded < 3) return fGammaHitsRecorded++;
        return -1;
    }
    void ResetGammaHitsRecorded() { fGammaHitsRecorded = 0; }

    G4ThreeVector ReconstructVertex();

    static const G4int kNumModules = 513;

private:
    // ── Smearing ─────────────────────────────────────────────────
    // sigma_radial = 1 cm (6mm bar depth direction)
    // sigma_Z      = 3 cm (along bar, time resolution)
    // no tangential smearing (bar identity known from electronics)
    G4ThreeVector SmearHitPosition(const G4ThreeVector& truePos);

    // ── Helper: fill H1(15-21) and H2(7-9) after trilateration ───
    void FillRecoHistograms(const G4ThreeVector& recoV,
                             G4int eventID,
                             G4AnalysisManager* man);

    // ── Scintillator arrays ───────────────────────────────────────
    std::vector<G4double> fEdepScin;
    std::vector<G4double> fHitTime;
    G4int                 fFirstScinID;
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

    // ── Raw hit positions (for 2D maps) ──────────────────────────
    G4ThreeVector fHitPos[3];
    G4int         fHitCount;

    // ── Per-gamma positions ───────────────────────────────────────
    G4ThreeVector fGammaHitPos[3];     // true MC hit positions
    G4ThreeVector fSmearedHitPos[3];   // smeared (used for reco)
    G4double      fGammaHitTime[3];
    G4bool        fGammaHitSet[3];

    G4int         fGammaHitsRecorded;
};

#endif

