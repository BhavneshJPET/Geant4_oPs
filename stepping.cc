#include "stepping.hh"
#include "event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "Randomize.hh"
#include <cmath>

#include "G4EventManager.hh"
#include "G4StackManager.hh"
#include "G4DynamicParticle.hh"
#include "G4Gamma.hh"
#include "G4RandomDirection.hh"
#include "G4AnalysisManager.hh"

MySteppingAction::MySteppingAction(MyEventAction* eventAction)
    : fEventAction(eventAction)
{}

MySteppingAction::~MySteppingAction() {}

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track        = step->GetTrack();
    G4String particleName = track->GetDefinition()->GetParticleName();

    // -------------------------------------------------------
    // 1. O-PS STEPS & PICK-OFF ANNIHILATION
    // -------------------------------------------------------
    if(particleName == "o-Ps")
    {
        if(track->GetCurrentStepNumber() == 1 && fEventAction)
        {
            fEventAction->SetFormationTime(
                step->GetPreStepPoint()->GetGlobalTime());
        }

        G4VPhysicalVolume* postVol =
            step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
        G4VPhysicalVolume* preVol =
            step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

        // ===================================================================
        // BLOCK A: TRUTH POSITION RECORDING (unchanged)
        // ===================================================================
        if(track->GetTrackID() == 1 && postVol)
        {
            G4String postVolName = postVol->GetName();
            G4AnalysisManager* man = G4AnalysisManager::Instance();
            G4ThreeVector fpos = step->GetPostStepPoint()->GetPosition();

            G4bool wasAlreadyThere = preVol && (preVol->GetName() == postVolName);

            if(!wasAlreadyThere)
            {
                if(G4StrUtil::contains(postVolName, "Opening_1"))
                {
                    G4int id = man->GetH1Id("fPatternOpening1");
                    if(id >= 0) man->FillH1(id, fpos.y() / cm);
                }
                else if(G4StrUtil::contains(postVolName, "Opening_2"))
                {
                    G4int id = man->GetH1Id("fPatternOpening2");
                    if(id >= 0) man->FillH1(id, fpos.y() / cm);
                }
                else if(postVolName == "SolidCounterPhys")
                {
                    G4int id1Y = man->GetH1Id("fTruePassageY");
                    if(id1Y >= 0) man->FillH1(id1Y, fpos.y() / cm);
                    G4int id1X = man->GetH1Id("fTruePassageX");
                    if(id1X >= 0) man->FillH1(id1X, fpos.x() / cm);
                    G4int id2 = man->GetH2Id("fTruePassageXY");
                    if(id2 >= 0) man->FillH2(id2, fpos.x() / cm, fpos.y() / cm);
                }
            }
        }

        // ===================================================================
        // BLOCK B: PICK-OFF ANNIHILATION
        // ===================================================================
        G4bool enteringTarget = false;
        G4int  targetVolID    = 0;

        if(postVol)
        {
            G4String postVolName = postVol->GetName();

            G4bool isWall1   = G4StrUtil::contains(postVolName, "Wall_1");
            G4bool isWall2   = G4StrUtil::contains(postVolName, "Wall_2");
            G4bool isCounter = (postVolName == "SolidCounterPhys");
            G4bool isAl      = (postVolName == "AlAbsorberPhys");

            G4bool isTargetVol  = (isWall1 || isWall2 || isCounter || isAl);
            G4bool wasInSameVol = preVol && (preVol->GetName() == postVolName);

            if(isTargetVol && !wasInSameVol)
            {
                enteringTarget = true;
                if(isCounter)    targetVolID = 3;
                else if(isWall1) targetVolID = 1;
                else if(isWall2) targetVolID = 2;
                else if(isAl)    targetVolID = 4;
            }
        }

        if(enteringTarget)
        {
            G4ThreeVector pos  = step->GetPostStepPoint()->GetPosition();
            G4double      time = step->GetPostStepPoint()->GetGlobalTime();
            G4int         oPsTrackID = track->GetTrackID();

            // ---------------------------------------------------------------
            // CRITICAL FIX: Set pick-off metadata BEFORE pushing tracks.
            // PreUserTrackingAction fires immediately on PushOneTrack, so
            // fPickOffVolumeID and fPickOffParentID must already be set by
            // the time the new gamma tracks are registered — otherwise the
            // slot-assignment guard sees stale values (0) and skips assignment.
            // ---------------------------------------------------------------
            if(fEventAction)
            {
                fEventAction->SetPickOffParentID(oPsTrackID);
                fEventAction->SetPickOffVolumeID(targetVolID);

                fEventAction->SetDecayZ(pos.z());
                fEventAction->SetDecayDetected(true);
                fEventAction->SetDecayPosition(pos.x(), pos.y(), pos.z());
                fEventAction->SetDecayTime(time);

                fEventAction->AddGammaInfo(511.*keV, G4ThreeVector(0,0,1)); // placeholder; real dir set below
                fEventAction->AddGammaInfo(511.*keV, G4ThreeVector(0,0,-1));
            }

            G4ThreeVector dir1 = G4RandomDirection();
            G4ThreeVector dir2 = -dir1;

            G4DynamicParticle* g1 = new G4DynamicParticle(G4Gamma::Gamma(), dir1, 511.*keV);
            G4DynamicParticle* g2 = new G4DynamicParticle(G4Gamma::Gamma(), dir2, 511.*keV);

            G4Track* t1 = new G4Track(g1, time, pos);
            G4Track* t2 = new G4Track(g2, time, pos);

            if(fEventAction)
            {
                t1->SetTrackID(fEventAction->GetNextFreeTrackID());
                t2->SetTrackID(fEventAction->GetNextFreeTrackID());
            }

            t1->SetParentID(oPsTrackID);
            t2->SetParentID(oPsTrackID);

            // Push AFTER metadata is set so TrackingAction sees correct values
            G4EventManager::GetEventManager()->GetStackManager()->PushOneTrack(t1);
            G4EventManager::GetEventManager()->GetStackManager()->PushOneTrack(t2);

            track->SetTrackStatus(fStopAndKill);

            return;
        }

        // ===================================================================
        // BLOCK C: Natural o-Ps 3-gamma decay in vacuum
        // ===================================================================
        if(track->GetTrackStatus() == fStopAndKill && fEventAction)
        {
            G4ThreeVector pos      = step->GetPostStepPoint()->GetPosition();
            G4double      decayTime = step->GetPostStepPoint()->GetGlobalTime();
            fEventAction->SetDecayZ(pos.z());
            fEventAction->SetDecayDetected(true);
            fEventAction->SetDecayPosition(pos.x(), pos.y(), pos.z());
            fEventAction->SetDecayTime(decayTime);
        }

        return;
    }

    // -------------------------------------------------------
    // 2. NATURAL GAMMA STEPS FILTER
    // -------------------------------------------------------
    if(particleName == "gamma" && track->GetParentID() > 0)
    {
        if(track->GetCreatorProcess() != nullptr)
        {
            G4String creator = track->GetCreatorProcess()->GetProcessName();
            G4bool fromDecay = (creator == "Decay" || creator == "DecayWithSpin");

            if(fromDecay && track->GetCurrentStepNumber() == 1 && fEventAction)
            {
                fEventAction->AddGammaInfo(
                    track->GetTotalEnergy(),
                    track->GetMomentumDirection());
            }
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
    G4int    trackID      = aTrack->GetTrackID();
    G4int    parentID     = aTrack->GetParentID();
    G4String particleName = aTrack->GetDefinition()->GetParticleName();

    if(fEventAction)
    {
        fEventAction->StoreParentID(trackID, parentID);

        if(fEventAction->GetGammaIndex(trackID) == -1)
        {
            if(particleName == "gamma")
            {
                // Case A: Natural 3-gamma decay (slots 0, 1, 2)
                if(parentID == 1)
                {
                    G4int slot = fEventAction->GetNextNaturalGammaSlot();
                    if(slot >= 0 && slot < 3)
                        fEventAction->SetGammaIndex(trackID, slot);
                }
                // Case B: Pick-off from ANY target volume (wall or counter)
                // fPickOffVolumeID and fPickOffParentID are now guaranteed to be
                // set before PushOneTrack, so this guard is always evaluated
                // with the correct values.
                else if(parentID == fEventAction->GetPickOffParentID() && parentID > 0)
                {
                    G4int volID = fEventAction->GetPickOffVolumeID();

                    // Accept wall pick-off (1,2) AND counter pick-off (3)
                    // for fringe pattern reconstruction. Skip Al absorber (4)
                    // as it is used only for validation, not fringe imaging.
                    if(volID == 1 || volID == 2 || volID == 3)
                    {
                        G4int slot = fEventAction->GetNextPickOffSlot();
                        if(slot >= 0 && slot < 2)
                        {
                            fEventAction->SetGammaIndex(trackID, slot);
                            G4cout << "[TrackingAction] Pick-off gamma trackID="
                                   << trackID << " volID=" << volID
                                   << " assigned slot=" << slot << G4endl;
                        }
                    }
                }
            }
            // Secondary tracks inherit parent gamma index
            else if(parentID > 1)
            {
                G4int parentIdx = fEventAction->GetGammaIndex(parentID);
                if(parentIdx != -1)
                    fEventAction->SetGammaIndex(trackID, parentIdx);
            }
        }
    }
}
