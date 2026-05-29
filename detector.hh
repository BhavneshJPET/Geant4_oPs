#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4SystemOfUnits.hh"

// Forward declaration — avoids pulling in all of event.hh here
// (event.hh is included in detector.cc where it is actually needed)
class MyEventAction;

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
    explicit MySensitiveDetector(G4String name);
    ~MySensitiveDetector();

    // Called by GEANT4 for every step inside the sensitive volume
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);

    // NOTE: G4MaterialPropertyVector* quEff was removed — it was declared
    // but never initialized or used, which would cause a dangling pointer.
    // Add it back with proper initialization if you later implement
    // quantum efficiency weighting for optical photons.
};

#endif
