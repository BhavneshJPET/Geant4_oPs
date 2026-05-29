//#include "detector.hh"

//MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
//{}

//MySensitiveDetector::~MySensitiveDetector()
//{}

//G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
//{
////   // 1. Get the Energy Deposit (Essential for the Compton Edge)
////    G4double edep = aStep->GetTotalEnergyDeposit();

////    // 2. Filter: Only record steps where energy was actually deposited
////    if(edep <= 0.) return false;

////    // 3. Collect Data
////    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
////    
////    // Identifies which J-PET scintillator bar was hit
////    G4int copyNo = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber();

////    // 4. Fill the ROOT Ntuple
////    G4AnalysisManager *man = G4AnalysisManager::Instance();
////    
////    man->FillNtupleIColumn(2, 0, evt);    // Column 0: fEvent
////    man->FillNtupleDColumn(2, 1, edep/keV);   // Column 1: fEdep (Energy in MeV)
////    man->FillNtupleIColumn(2, 2, copyNo); // Column 2: fCopyNo
////    
////    man->AddNtupleRow(2); 

//    G4double edep = aStep->GetTotalEnergyDeposit();
//    if(edep <= 0.) return false;

//    // Get the Scintillator ID (0 to 12)
//    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
//    G4int copyNo = touchable->GetCopyNumber(); 

//    MyEventAction *eventAction = (MyEventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
//    
//    // Pass both the ID and the energy
//    eventAction->AddEdep(copyNo, edep);

//    return true;
//}


// O-PS

//#include "detector.hh"

//MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name) {}
//MySensitiveDetector::~MySensitiveDetector() {}

//G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) {
//    G4double edep = aStep->GetTotalEnergyDeposit();
//    
//    // Only record if there is an actual energy deposit (Compton scattering from gammas)
//    if(edep <= 0.) return false;

//    // Get hit time (Global time from start of event)
//    G4double time = aStep->GetPreStepPoint()->GetGlobalTime();
//   // G4cout << "Hit at time: " << time/ns << " ns" << G4endl;
//    
//    // Identify which module was hit
//    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
//    G4int copyNo = touchable->GetCopyNumber(); 

//    // Send data to EventAction (Ensure your AddEdep can handle copyNo, edep, and time)
//    MyEventAction *eventAction = (MyEventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
//    eventAction->AddEdep(copyNo, edep, time); 
//    // If you add a time variable to your EventAction, use: eventAction->AddTime(time);

//    return true;
//}


//DEEP

//#include "detector.hh"
//#include "event.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4RunManager.hh"
//#include "G4VTouchable.hh"
//#include "G4Track.hh"
//#include "G4Step.hh"

//MySensitiveDetector::MySensitiveDetector(G4String name)
//    : G4VSensitiveDetector(name)
//{
//    collectionName.insert("JPETHits");
//}

//MySensitiveDetector::~MySensitiveDetector() {}

//G4bool MySensitiveDetector::ProcessHits(
//    G4Step* aStep, G4TouchableHistory*)
//{
//    // 1. Only record steps with energy deposit
//    G4double edep = aStep->GetTotalEnergyDeposit();
//    if(edep <= 0.) return false;

//    // 2. Which bar was hit? Copy number directly = barID (201-512)
//    const G4VTouchable* touchable =
//        aStep->GetPreStepPoint()->GetTouchable();
//    G4int copyNo = touchable->GetCopyNumber();
//    G4int barID  = copyNo;

//    // 3. Hit time
//    G4double hitTime =
//        aStep->GetPreStepPoint()->GetGlobalTime();

//    // 4. Hit position
//    G4ThreeVector hitPos =
//        aStep->GetPostStepPoint()->GetPosition();

//    // 5. Get EventAction
//    MyEventAction* eventAction =
//        static_cast<MyEventAction*>(
//            const_cast<G4UserEventAction*>(
//                G4RunManager::GetRunManager()
//                    ->GetUserEventAction()));

//    // 6. Forward data
//    if(eventAction)
//    {
//        // Energy + time per bar
//        eventAction->AddEdep(barID, edep, hitTime);

//        // NEW: record hit position for 2D maps + kinematics
//        eventAction->AddHitPosition(hitPos);
//        
//        // NEW: Z position for photon 1/2/3 histograms
//        eventAction->AddGammaHitZ(hitPos.z());
//    }

//    // 7. Debug output (first 50 events only)
//    G4int eventID = G4RunManager::GetRunManager()
//                        ->GetCurrentEvent()->GetEventID();
//    if(eventID < 50 && edep > 0.01*keV)
//    {
//        G4cout << "SD Hit | Event: " << eventID
//               << " | Bar: "    << barID
//               << " | Edep: "   << edep/keV      << " keV"
//               << " | Time: "   << hitTime/ns     << " ns"
//               << " | pos: ("   << hitPos.x()/cm  << ", "
//                                 << hitPos.y()/cm  << ", "
//                                 << hitPos.z()/cm  << ") cm"
//               << G4endl;
//    }

//    return true;
//}

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
