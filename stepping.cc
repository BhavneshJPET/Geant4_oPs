#include "stepping.hh"
#include "event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "Randomize.hh"
#include <cmath>

MySteppingAction::MySteppingAction(MyEventAction* eventAction)
    : fEventAction(eventAction)
{}

MySteppingAction::~MySteppingAction() {}

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track        = step->GetTrack();
    G4String particleName = track->GetDefinition()->GetParticleName();

    // -------------------------------------------------------
    // O-PS STEPS
    // -------------------------------------------------------
    if(particleName == "o-Ps")
    {
        G4int eventID = G4RunManager::GetRunManager()
                            ->GetCurrentEvent()->GetEventID();

        if(track->GetCurrentStepNumber() == 1 && fEventAction)
        {
            fEventAction->SetFormationTime(0.0);

            G4double tau      = 142.0 * ns;
            G4double lifetime = -tau * std::log(G4UniformRand());
            fEventAction->SetDecayTime(lifetime);

            if(eventID < 10)
                G4cout << "*** o-Ps FORMED | sampled lifetime = " << lifetime/ns << " ns" << G4endl;
        }

        if(track->GetTrackStatus() == fStopAndKill && fEventAction)
        {
            G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
            fEventAction->SetDecayZ(pos.z());
            fEventAction->SetDecayDetected(true);
            fEventAction->SetDecayPosition(pos.x(), pos.y(), pos.z());
        }
        return;
    }

    // -------------------------------------------------------
    // GAMMA STEPS
    // -------------------------------------------------------
    if(particleName == "gamma"
       && track->GetParentID() > 0
       && track->GetCreatorProcess() != nullptr)
    {
        G4String creator = track->GetCreatorProcess()->GetProcessName();
        if(creator != "Decay" && creator != "DecayWithSpin")
            return;

        G4int eventID = G4RunManager::GetRunManager()
                            ->GetCurrentEvent()->GetEventID();

        if(track->GetCurrentStepNumber() == 1 && fEventAction)
        {
            fEventAction->AddGammaInfo(
                track->GetTotalEnergy(),
                track->GetMomentumDirection());
        }

        // Record first hit in scintillator
        G4double edep = step->GetTotalEnergyDeposit();
        if(edep > 0. && fEventAction)
        {
            G4String volName = "";
            if(step->GetPreStepPoint()->GetPhysicalVolume())
                volName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

            if(volName.find("ScintillatorPhys") != G4String::npos)
            {
                G4ThreeVector hitPos = step->GetPreStepPoint()->GetPosition();
    G4double hitTime     = step->GetPreStepPoint()->GetGlobalTime();

    // Use arrival-order index — NOT trackID arithmetic
    // GetNextGammaIndex() returns 0,1,2 in order of first arrival
    // and returns -1 once 3 gammas already recorded
    G4int gammaIndex = fEventAction->GetNextGammaIndex();

    if(gammaIndex >= 0) {
        fEventAction->SetGammaHit(gammaIndex, hitPos);
        fEventAction->SetGammaHitTime(gammaIndex, hitTime);

        if(eventID < 10)
            G4cout << "  Gamma " << gammaIndex
                   << " hit | Z=" << hitPos.z()/cm
                   << " cm | T=" << hitTime/ns << " ns" << G4endl;
                }
            }
        }
    }
}





//Try


//#include "stepping.hh"
//#include "event.hh"
//#include "G4RunManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4Track.hh"
//#include "G4StepPoint.hh"
//#include "Randomize.hh"
//#include <cmath>

//MySteppingAction::MySteppingAction(MyEventAction* eventAction)
//    : fEventAction(eventAction)
//{}

//MySteppingAction::~MySteppingAction() {}

//void MySteppingAction::UserSteppingAction(const G4Step* step)
//{
//    G4Track* track        = step->GetTrack();
//    G4String particleName = track->GetDefinition()->GetParticleName();

//    // -------------------------------------------------------
//    // O-PS STEPS
//    // -------------------------------------------------------
//    if(particleName == "o-Ps")
//    {
//        G4int eventID = G4RunManager::GetRunManager()
//                            ->GetCurrentEvent()->GetEventID();

//        if(track->GetCurrentStepNumber() == 1 && fEventAction)
//        {
//            fEventAction->SetFormationTime(0.0);

//            G4double tau      = 142.0 * ns;
//            G4double lifetime = -tau * std::log(G4UniformRand());
//            fEventAction->SetDecayTime(lifetime);

//            if(eventID < 10)
//                G4cout << "*** o-Ps FORMED | sampled lifetime = " << lifetime/ns << " ns" << G4endl;
//        }

//        if(track->GetTrackStatus() == fStopAndKill && fEventAction)
//        {
//            G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
//            fEventAction->SetDecayZ(pos.z());
//            fEventAction->SetDecayDetected(true);
//            fEventAction->SetDecayPosition(pos.x(), pos.y(), pos.z());
//        }
//        return;
//    }

//    // -------------------------------------------------------
//    // GAMMA STEPS
//    // -------------------------------------------------------
//    if(particleName == "gamma"
//   && track->GetParentID() > 0
//   && track->GetCreatorProcess() != nullptr)
//{
//    G4String creator = track->GetCreatorProcess()->GetProcessName();
//    if(creator != "Decay" && creator != "DecayWithSpin")
//        return;

//    G4int eventID = G4RunManager::GetRunManager()
//                        ->GetCurrentEvent()->GetEventID();

//    // Store gamma direction at creation (step 1)
//    if(track->GetCurrentStepNumber() == 1 && fEventAction)
//    {
//        fEventAction->AddGammaInfo(
//            track->GetTotalEnergy(),
//            track->GetMomentumDirection());
//    }

//    // Record first hit in scintillator
//    G4double edep = step->GetTotalEnergyDeposit();
//    if(edep > 0. && fEventAction)
//    {
//        G4String volName = "";
//        if(step->GetPreStepPoint()->GetPhysicalVolume())
//            volName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

//        if(volName.find("ScintillatorPhys") != G4String::npos)
//        {
//            // Get next available gamma index (0, 1, or 2 in order of first hit)
//            G4int gammaIndex = fEventAction->GetNextGammaIndex();
//            
//            if(gammaIndex >= 0 && gammaIndex < 3)
//            {
//                G4ThreeVector hitPos = step->GetPreStepPoint()->GetPosition();
//                G4double hitTime = step->GetPreStepPoint()->GetGlobalTime();
//                
//                fEventAction->SetGammaHit(gammaIndex, hitPos);
//                fEventAction->SetGammaHitTime(gammaIndex, hitTime);
//                
//                if(eventID < 10)
//                {
//                    G4cout << "  Gamma " << gammaIndex 
//                           << " hit | Z=" << hitPos.z()/cm 
//                           << " cm | T=" << hitTime/ns << " ns" 
//                           << G4endl;
//                }
//            }
//        }
//    }
//}
//}
