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
        if(track->GetCurrentStepNumber() == 1 && fEventAction)
        {
            fEventAction->SetFormationTime(step->GetPreStepPoint()->GetGlobalTime());
        }

        if(track->GetTrackStatus() == fStopAndKill && fEventAction)
        {
            G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
            G4double decayTime = step->GetPostStepPoint()->GetGlobalTime();
            fEventAction->SetDecayZ(pos.z());
            fEventAction->SetDecayDetected(true);
            fEventAction->SetDecayPosition(pos.x(), pos.y(), pos.z());
            fEventAction->SetDecayTime(decayTime);
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

        if(track->GetCurrentStepNumber() == 1 && fEventAction)
        {
            fEventAction->AddGammaInfo(
                track->GetTotalEnergy(),
                track->GetMomentumDirection());
        }
    }
}

// ===========================================================================
// MyTrackingAction Implementation
// ===========================================================================

MyTrackingAction::MyTrackingAction(MyEventAction* eventAction)
    : fEventAction(eventAction)
{}

MyTrackingAction::~MyTrackingAction() {}

void MyTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
    G4int trackID         = aTrack->GetTrackID();
    G4int parentID        = aTrack->GetParentID();
    G4String particleName = aTrack->GetDefinition()->GetParticleName();

    if(fEventAction)
    {
        if(fEventAction->GetGammaIndex(trackID) == -1)
        {
            if(parentID == 1 && particleName == "gamma")
            {
                G4int gammaIdx = trackID - 2;
                if(gammaIdx >= 0 && gammaIdx < 3)
                {
                    fEventAction->SetGammaIndex(trackID, gammaIdx);
                }
            }
            else if(parentID > 1)
            {
                G4int parentIdx = fEventAction->GetGammaIndex(parentID);
                if(parentIdx != -1)
                {
                    fEventAction->SetGammaIndex(trackID, parentIdx);
                }
            }
        }
    }
}

