#include "action.hh"

MyActionInitialization::MyActionInitialization(G4String filename)
    : fFilename(filename), fGenerator(nullptr)    // ← ADD THIS
{}

MyActionInitialization::~MyActionInitialization()
{}

void MyActionInitialization::BuildForMaster() const
{
    MyRunAction *runAction = new MyRunAction(fFilename);
    SetUserAction(runAction);
}

void MyActionInitialization::Build() const
{
    // ← CHANGE: save pointer to fGenerator first
    fGenerator = new MyPrimaryGenerator();
    SetUserAction(fGenerator);

    // Everything below is UNCHANGED from your code
    MyRunAction *runAction = new MyRunAction(fFilename);
    SetUserAction(runAction);

    MyEventAction *eventAction = new MyEventAction(runAction);
    SetUserAction(eventAction);

    MySteppingAction *steppingAction = new MySteppingAction(eventAction);
    SetUserAction(steppingAction);

    MyTrackingAction *trackingAction = new MyTrackingAction(eventAction);
    SetUserAction(trackingAction);
}
    MySteppingAction *steppingAction = new MySteppingAction(eventAction);
    SetUserAction(steppingAction);
}
