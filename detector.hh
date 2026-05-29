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
};

#endif
