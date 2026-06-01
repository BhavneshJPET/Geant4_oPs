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
    virtual void EndOfRunAction(const G4Run*);

    // Called by EventAction::EndOfEventAction for every event.
    // Accumulates statistics only for genuine 3-gamma o-Ps events.
    void AddEventData(G4int gammaCount,
                      G4double totalEnergy,
                      G4double decayZ);

    void AddTrueCoincidence() { fTrueCoincidences += 1; }

private:
    G4String fFilename;
    // Run-level accumulators (thread-safe for MT)
    G4Accumulable<G4int>    fTotalEvents;
    G4Accumulable<G4int>    fTotal3GammaEvents;  // number of events with exactly 3 decay gammas
    G4Accumulable<G4double> fTotalEnergySum;     // sum of 3-gamma total energies (for average)
    G4Accumulable<G4double> fDecayXSum;          // sum of decay X positions (for average)
    G4Accumulable<G4int>    fTrueCoincidences;   // number of events where all 3 gammas hit
};

#endif
