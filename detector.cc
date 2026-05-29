#include "detector.hh"
#include "event.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4Step.hh"

MySensitiveDetector::MySensitiveDetector(G4String name)
    : G4VSensitiveDetector(name)
{
    collectionName.insert("JPETHits");
}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits(
    G4Step* aStep, G4TouchableHistory*)
{
    // ---------------------------------------------------------------
    // 1. Only record steps with energy deposit
    // ---------------------------------------------------------------
    G4double edep = aStep->GetTotalEnergyDeposit();
    if(edep <= 0.) return false;

    // ---------------------------------------------------------------
    // 2. Bar ID (copy number 201-512)
    // ---------------------------------------------------------------
    const G4VTouchable* touchable =
        aStep->GetPreStepPoint()->GetTouchable();
    G4int barID = touchable->GetCopyNumber();

    // ---------------------------------------------------------------
    // 3. Hit time and position
    // ---------------------------------------------------------------
    G4double      hitTime = aStep->GetPreStepPoint()
                                 ->GetGlobalTime();
    G4ThreeVector hitPos  = aStep->GetPostStepPoint()
                                 ->GetPosition();

    // ---------------------------------------------------------------
    // 4. Get track info
    // ---------------------------------------------------------------
    G4Track* track = aStep->GetTrack();

    // ---------------------------------------------------------------
    // 5. Get EventAction
    // ---------------------------------------------------------------
    MyEventAction* eventAction =
        static_cast<MyEventAction*>(
            const_cast<G4UserEventAction*>(
                G4RunManager::GetRunManager()
                    ->GetUserEventAction()));

    // ---------------------------------------------------------------
    // 6. Forward data to EventAction
    // ---------------------------------------------------------------
    if(eventAction)
    {
        // Always record energy deposit and time
        eventAction->AddEdep(barID, edep, hitTime);
        eventAction->AddHitPosition(hitPos);

        // -----------------------------------------------------------
        // SET GAMMA HIT POSITION AND TIME — KEY FIX
        // This was missing — causing smeared histograms to be empty
        //
        // Track ID mapping:
        //   trackID = 1 → primary o-Ps
        //   trackID = 2 → first gamma  → gammaIdx = 0
        //   trackID = 3 → second gamma → gammaIdx = 1
        //   trackID = 4 → third gamma  → gammaIdx = 2
        //
        // gammaIdx = trackID - 2
        // -----------------------------------------------------------
        if(track->GetDefinition()
                 ->GetParticleName() == "gamma"
           && track->GetParentID() > 0)
        {
            G4int trackID  = track->GetTrackID();
            G4int gammaIdx = trackID - 2;

            if(gammaIdx >= 0 && gammaIdx < 3)
            {
                // Set hit position for this gamma (first hit only)
                eventAction->SetGammaHit(gammaIdx, hitPos);

                // Set hit time for this gamma (earliest time wins)
                eventAction->SetGammaHitTime(gammaIdx, hitTime);
            }
        }
    }

    // ---------------------------------------------------------------
    // 7. Debug output for first 10 events
    // ---------------------------------------------------------------
    G4int eventID = G4RunManager::GetRunManager()
                        ->GetCurrentEvent()->GetEventID();
    if(eventID < 10 && edep > 0.01*keV)
    {
        G4int trackID  = track->GetTrackID();
        G4int gammaIdx = trackID - 2;

        G4cout << "SD Hit | Event: "   << eventID
               << " | Bar: "           << barID
               << " | Edep: "          << edep/keV      << " keV"
               << " | Z: "             << hitPos.z()/cm  << " cm"
               << " | trackID: "       << trackID
               << " | gammaIdx: "      << gammaIdx
               << G4endl;
    }

    return true;
}
