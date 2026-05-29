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
