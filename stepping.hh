#ifndef STEPPING_HH
#define STEPPING_HH

#include "G4UserSteppingAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

// Forward declaration — avoids circular include with event.hh
class MyEventAction;

class MySteppingAction : public G4UserSteppingAction
{
public:
    explicit MySteppingAction(MyEventAction* eventAction);
    ~MySteppingAction();

    virtual void UserSteppingAction(const G4Step* step);

private:
    MyEventAction* fEventAction;   // non-owning pointer, owned by ActionInitialization
};

class MyTrackingAction : public G4UserTrackingAction
{
public:
    explicit MyTrackingAction(MyEventAction* eventAction);
    virtual ~MyTrackingAction();

    virtual void PreUserTrackingAction(const G4Track* aTrack) override;

private:
    MyEventAction* fEventAction;
};

#endif
#ifndef STEPPING_HH
#define STEPPING_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

// Forward declaration — avoids circular include with event.hh
class MyEventAction;

class MySteppingAction : public G4UserSteppingAction
{
public:
    explicit MySteppingAction(MyEventAction* eventAction);
    ~MySteppingAction();

    virtual void UserSteppingAction(const G4Step* step);

private:
    MyEventAction* fEventAction;   // non-owning pointer, owned by ActionInitialization
};

#endif
