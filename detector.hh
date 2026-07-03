#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4SystemOfUnits.hh"

class MyEventAction;

// ---------------------------------------------------------------
// 1. Scintillator SD (J-PET Bars)
// ---------------------------------------------------------------
class MySensitiveDetector : public G4VSensitiveDetector
{
public:
    explicit MySensitiveDetector(G4String name);
    virtual ~MySensitiveDetector() override; 
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
};

// ---------------------------------------------------------------
// 2. Al Absorber SD
// ---------------------------------------------------------------
class MyAlAbsorberSD : public G4VSensitiveDetector
{
public:
    explicit MyAlAbsorberSD(G4String name);
    virtual ~MyAlAbsorberSD() override; 
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
};

// ---------------------------------------------------------------
// 3. Grating Walls SD (Silicon Masks)
// ---------------------------------------------------------------
class MyGratingSD : public G4VSensitiveDetector
{
public:
    explicit MyGratingSD(G4String name);
    virtual ~MyGratingSD() override;
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
};

// ---------------------------------------------------------------
// 4. Solid Counter SD (Stopper/Screen Plane)
// ---------------------------------------------------------------
class MyCounterSD : public G4VSensitiveDetector
{
public:
    explicit MyCounterSD(G4String name);
    virtual ~MyCounterSD() override;
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
};

#endif
