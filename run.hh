#ifndef RUN_HH
#define RUN_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"
#include <sstream>

class MyRunAction : public G4UserRunAction
{
public:
    MyRunAction(G4String filename);
    ~MyRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction  (const G4Run*);

    // Called by EventAction::EndOfEventAction for every event
    void AddEventData(G4int gammaCount,
                      G4double totalEnergy,
                      G4double decayZ);

    void AddTrueCoincidence();
    void Add2GammaEvent();

private:
    G4String fFilename;

    // Run-level accumulators (thread-safe for MT)
    G4Accumulable<G4int>    fTotalEvents;
    G4Accumulable<G4int>    fTotal3GammaEvents;
    G4Accumulable<G4int>    fTotal2GammaEvents;  // Pick-off / p-Ps events
    G4Accumulable<G4double> fTotalEnergySum;
    G4Accumulable<G4double> fDecayZSum;          // FIXED: Renamed to match Z-axis tracking in implementation
    G4Accumulable<G4int>    fTrueCoincidences;
};

#endif
