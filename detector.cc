#include "detector.hh"
#include "event.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4AnalysisManager.hh"

// =========================================================================
// 1. SCINTILLATOR SENSITIVE DETECTOR
// =========================================================================

MySensitiveDetector::MySensitiveDetector(G4String name)
    : G4VSensitiveDetector(name)
{
    collectionName.insert("JPETHits");
}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4double edep = aStep->GetTotalEnergyDeposit();
    if(edep <= 0.) return false;

    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int barID = touchable->GetCopyNumber();

    G4double      hitTime = aStep->GetPreStepPoint()->GetGlobalTime();
    G4ThreeVector hitPos  = aStep->GetPreStepPoint()->GetPosition();

    G4Track* track = aStep->GetTrack();

    MyEventAction* eventAction = static_cast<MyEventAction*>(
        const_cast<G4UserEventAction*>(
            G4RunManager::GetRunManager()->GetUserEventAction()));

    if(eventAction)
    {
        eventAction->AddEdep(barID, edep, hitTime);
        eventAction->AddHitPosition(hitPos);

        // Walk up the tracking chain to find the primary gamma index
        G4int gammaIdx = eventAction->GetGammaIndex(track->GetTrackID());

        if(gammaIdx < 0)
            gammaIdx = eventAction->GetGammaIndex(track->GetParentID());

        if(gammaIdx < 0)
        {
            G4int grandParentID = eventAction->GetStoredParentID(track->GetParentID());
            if(grandParentID > 0)
                gammaIdx = eventAction->GetGammaIndex(grandParentID);
        }

        if(gammaIdx >= 0 && gammaIdx < 3)
        {
            // FIXED: Removed the incorrect fPickOffVolumeID != 3 guard that was
            // here previously. That guard silently rejected all wall pick-off hits
            // (volID 1 and 2) and — due to the race condition with SetPickOffVolumeID
            // being called after PushOneTrack — also rejected counter hits.
            // Now we accept any gamma with a valid slot assignment (set by
            // TrackingAction), regardless of which target volume it came from.
            eventAction->SetGammaHit(gammaIdx, hitPos);
            eventAction->SetGammaHitTime(gammaIdx, hitTime);
        }
    }

    return true;
}

// =========================================================================
// 2. ALUMINUM ABSORBER SENSITIVE DETECTOR
// =========================================================================

MyAlAbsorberSD::MyAlAbsorberSD(G4String name)
    : G4VSensitiveDetector(name)
{}

MyAlAbsorberSD::~MyAlAbsorberSD() {}

G4bool MyAlAbsorberSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4Track* track = aStep->GetTrack();
    G4String pName = track->GetDefinition()->GetParticleName();

    if(pName == "o-Ps")
    {
        track->SetTrackStatus(fStopAndKill);
        return true;
    }

    return false;
}

// =========================================================================
// 3. GRATING WALLS SENSITIVE DETECTOR
// =========================================================================

MyGratingSD::MyGratingSD(G4String name)
    : G4VSensitiveDetector(name)
{}

MyGratingSD::~MyGratingSD() {}

G4bool MyGratingSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4Track* track = aStep->GetTrack();
    G4String pName = track->GetDefinition()->GetParticleName();

    if(pName == "o-Ps")
        return true;

    return false;
}

// =========================================================================
// 4. SOLID COUNTER SENSITIVE DETECTOR
// =========================================================================

MyCounterSD::MyCounterSD(G4String name)
    : G4VSensitiveDetector(name)
{}

MyCounterSD::~MyCounterSD() {}

G4bool MyCounterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4Track* track = aStep->GetTrack();
    G4String pName = track->GetDefinition()->GetParticleName();

    if(pName == "gamma")
    {
        G4double edep = aStep->GetTotalEnergyDeposit();
        if(edep <= 0.) return false;
        return true;
    }

    return false;
}
