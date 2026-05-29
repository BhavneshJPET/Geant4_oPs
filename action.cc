//#include "action.hh"

//MyActionInitialization::MyActionInitialization()
//{}

//MyActionInitialization::~MyActionInitialization()
//{}

//void MyActionInitialization::BuildForMaster() const
//{}

//void MyActionInitialization::Build() const
//{
//	MyPrimaryGenerator *generator = new MyPrimaryGenerator();
//	SetUserAction(generator);
//	
//	MyRunAction *runAction = new MyRunAction();
//	SetUserAction(runAction);
//	
//	MyEventAction *eventAction = new MyEventAction(runAction);
//	SetUserAction(eventAction);
//	
//	MySteppingAction *steppingAction = new MySteppingAction(eventAction);
//	SetUserAction(steppingAction);
//}

//NA22

#include "action.hh"

MyActionInitialization::MyActionInitialization()
    : fGenerator(nullptr)    // ← ADD THIS
{}

MyActionInitialization::~MyActionInitialization()
{}

void MyActionInitialization::BuildForMaster() const
{}

void MyActionInitialization::Build() const
{
    // ← CHANGE: save pointer to fGenerator first
    fGenerator = new MyPrimaryGenerator();
    SetUserAction(fGenerator);

    // Everything below is UNCHANGED from your code
    MyRunAction *runAction = new MyRunAction();
    SetUserAction(runAction);

    MyEventAction *eventAction = new MyEventAction(runAction);
    SetUserAction(eventAction);

    MySteppingAction *steppingAction = new MySteppingAction(eventAction);
    SetUserAction(steppingAction);
}
