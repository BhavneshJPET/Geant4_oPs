//#include "stepping.hh"

//MySteppingAction::MySteppingAction(MyEventAction *eventAction)
//{
//    fEventAction = eventAction;
//}

//MySteppingAction::~MySteppingAction()
//{}

//void MySteppingAction::UserSteppingAction(const G4Step *step)
//{   
////    G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
////    
////    const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
////    
////    G4LogicalVolume *fScoringVolume = detectorConstruction->GetScintLogical();
////    
////    if(volume != fScoringVolume)
////        return;
////    
////    G4double edep = step->GetTotalEnergyDeposit();
////fEventAction->AddEdep(edep);
//}


// O-PS

//#include "stepping.hh"
//#include "G4RunManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4UnitsTable.hh"

//// FIX: Added the constructor that was causing the linker error
//MySteppingAction::MySteppingAction(MyEventAction* eventAction)
//{
//    fEventAction = eventAction;
//}

//// FIX: Added the destructor
//MySteppingAction::~MySteppingAction()
//{}

//void MySteppingAction::UserSteppingAction(const G4Step *step) {
//    G4Track *track = step->GetTrack();

//    // 1. Logic for tracking the o-Ps Beam
//    if (track->GetDefinition()->GetParticleName() == "o-Ps") {
//        
//        // Check if it enters your J-PET module
//        G4VPhysicalVolume *volume = step->GetPreStepPoint()->GetPhysicalVolume();
//        if (volume && volume->GetName() == "ScintillatorPhys") { 
//            // This is where you could record the entrance time for J-PET
//        }

//        // 2. Check if o-Ps just decayed
//        if (track->GetTrackStatus() == fStopAndKill) {
//            G4ThreeVector decayPos = track->GetPosition();
//            G4double decayTime = track->GetGlobalTime();
//            
//            // Send the X-position (the beam axis) to your EventAction
//            fEventAction->AddDecayPosition(decayPos.x());
//            

//		
//            // For debugging: print where the o-Ps vanished
//            // G4cout << "o-Ps decayed at: " << G4BestUnit(decayPos, "Length") << G4endl;
//        }
//    }
//}


//DIRECT SAMPLING

//#include "stepping.hh"
//#include "event.hh"
//#include "G4RunManager.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4Track.hh"
//#include "G4StepPoint.hh"
//#include "Randomize.hh"    // for G4UniformRand()
//#include <cmath>            // for std::log()

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
//        G4StepPoint* postPoint   = step->GetPostStepPoint();
//        G4String     processName = "None";
//        if(postPoint->GetProcessDefinedStep())
//            processName = postPoint->GetProcessDefinedStep()
//                                   ->GetProcessName();

//        G4int eventID = G4RunManager::GetRunManager()
//                            ->GetCurrentEvent()->GetEventID();

//        // ---------------------------------------------------
//        // FORMATION TIME + MANUAL LIFETIME SAMPLING
//        // Only on step 1 — when o-Ps is first created
//        // ---------------------------------------------------
//        if(track->GetCurrentStepNumber() == 1 && fEventAction)
//        {
//            // Formation time = exact birth time of o-Ps
//            G4double formationTime = track->GetGlobalTime();
//            fEventAction->SetFormationTime(formationTime);

//            // -----------------------------------------------
//            // MANUAL EXPONENTIAL DECAY SAMPLING
//            // Formula: t = -τ × ln(u)
//            // where u = uniform random number in (0,1)
//            // τ = 142 ns (o-Ps vacuum lifetime)
//            // This correctly produces exponential distribution
//            // with mean = τ = 142 ns
//            // -----------------------------------------------
//            const G4double tau = 142.0 * ns;
//            G4double u = G4UniformRand();
//            while(u <= 0.) u = G4UniformRand(); // protect against log(0)
//            G4double sampledLifetime = -tau * std::log(u);

//            // Decay time = formation + sampled lifetime
//            G4double decayTime = formationTime + sampledLifetime;
//            fEventAction->SetDecayTime(decayTime);

//            if(eventID < 10)
//            {
//                G4cout << "*** o-Ps FORMED"
//                       << " | formation t    = " << formationTime/ns    << " ns"
//                       << " | sampled lifetime = " << sampledLifetime/ns << " ns"
//                       << " | will decay at  = " << decayTime/ns        << " ns"
//                       << G4endl;
//            }
//        }

//        // ---------------------------------------------------
//        // DECAY POSITION
//        // Record where o-Ps actually decays
//        // ---------------------------------------------------
//        if(processName == "Decay" && fEventAction)
//        {
//            fEventAction->AddDecayPosition(
//                step->GetPostStepPoint()->GetPosition().z());
//  
//        
//        
//            if(eventID < 10)
//                G4cout << "*** o-Ps DECAYED"
//                       << " | actual Geant4 decay t = "
//                       << step->GetPostStepPoint()->GetGlobalTime()/ns
//                       << " ns" << G4endl;
//        }

//        return;
//    }

//    // -------------------------------------------------------
//    // GAMMA STEPS
//    // Capture each decay gamma exactly ONCE on its first step
//    // -------------------------------------------------------
//    if(particleName == "gamma"
//       && track->GetCurrentStepNumber() == 1
//       && track->GetParentID() > 0
//       && track->GetCreatorProcess() != nullptr)
//    {
//        G4String creator = track->GetCreatorProcess()->GetProcessName();

//        if((creator == "Decay" || creator == "DecayWithSpin")
//           && fEventAction)
//        {
//            G4int eventID = G4RunManager::GetRunManager()
//                                ->GetCurrentEvent()->GetEventID();
//            if(eventID < 10)
//            {
//                G4cout << "  Decay gamma born"
//                       << " | E = "   << track->GetTotalEnergy()/keV << " keV"
//                       << " | dir = ("
//                       << track->GetMomentumDirection().x() << ", "
//                       << track->GetMomentumDirection().y() << ", "
//                       << track->GetMomentumDirection().z() << ")"
//                       << G4endl;
//            }
//            fEventAction->AddGammaInfo(
//                track->GetTotalEnergy(),
//                track->GetMomentumDirection());
//        }
//    }
//}


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

//        // Formation: manual lifetime sampling at t=0
//        if(track->GetCurrentStepNumber() == 1 && fEventAction)
//        {
//            fEventAction->SetFormationTime(0.0);

//            // Manual sampling: t = -tau * ln(uniform random)
//            G4double tau      = 142.0 * ns;
//            G4double lifetime = -tau * std::log(G4UniformRand());
//            fEventAction->SetDecayTime(lifetime);

//            if(eventID < 10)
//                G4cout << "*** o-Ps FORMED"
//                       << " | sampled lifetime = "
//                       << lifetime/ns << " ns" << G4endl;
//        }

//        // Decay: record Z position + set detected flag
//        G4StepPoint* postPoint   = step->GetPostStepPoint();
//        G4String     processName = "None";
//        if(postPoint->GetProcessDefinedStep())
//            processName = postPoint
//                ->GetProcessDefinedStep()->GetProcessName();

//        if(track->GetTrackStatus() == fStopAndKill
//   && fEventAction)
//{
//    G4ThreeVector pos =
//        step->GetPostStepPoint()->GetPosition();

//    G4double decayZ = pos.z();

//    fEventAction->SetDecayZ(decayZ);

//    fEventAction->SetDecayDetected(true);

//    fEventAction->SetDecayPosition(
//        pos.x(),
//        pos.y(),
//        pos.z());
//}
//        return;
//    }

//    // -------------------------------------------------------
//    // GAMMA STEPS
//    // -------------------------------------------------------
//    if(particleName == "gamma"
//       && track->GetParentID() > 0
//       && track->GetCreatorProcess() != nullptr)
//    {
//        G4String creator = track->GetCreatorProcess()->GetProcessName();
//        if(creator != "Decay" && creator != "DecayWithSpin")
//            return;

//        G4int eventID = G4RunManager::GetRunManager()
//                            ->GetCurrentEvent()->GetEventID();

//        // Record gamma energy+direction at birth
//        if(track->GetCurrentStepNumber() == 1 && fEventAction)
//        {
//            fEventAction->AddGammaInfo(
//                track->GetTotalEnergy(),
//                track->GetMomentumDirection());

//            if(eventID < 10)
//                G4cout << "  Decay gamma born | E="
//                       << track->GetTotalEnergy()/keV
//                       << " keV" << G4endl;
//        }

//        // Record first hit in scintillator
//        G4double edep = step->GetTotalEnergyDeposit();
//        if(edep > 0. && fEventAction)
//        {
//            G4String volName = "";
//            if(step->GetPreStepPoint()->GetPhysicalVolume())
//                volName = step->GetPreStepPoint()
//                    ->GetPhysicalVolume()->GetName();

//            if(volName.find("ScintillatorPhys") != G4String::npos)
//            {
//                G4int trackID    = track->GetTrackID();
//                G4int gammaIndex = (trackID - 2) % 3;
//                if(gammaIndex < 0) gammaIndex = 0;
//                if(gammaIndex > 2) gammaIndex = 2;

//                G4ThreeVector hitPos =
//                    step->GetPreStepPoint()->GetPosition();

//                fEventAction->SetGammaHit(gammaIndex, hitPos);

//                if(eventID < 10)
//                    G4cout << "  Gamma " << gammaIndex
//                           << " hit | Z=" << hitPos.z()/cm
//                           << " cm" << G4endl;
//            }
//        }
//    }
//}




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
