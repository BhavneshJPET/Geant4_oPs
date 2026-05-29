//#ifndef ACTION_HH
//#define ACTION_HH

//#include "G4VUserActionInitialization.hh"

//#include "generator.hh"
//#include "run.hh"
//#include "event.hh"
//#include "stepping.hh"

//class MyActionInitialization : public G4VUserActionInitialization

//{
//public: 
//	MyActionInitialization();
//	~MyActionInitialization();
//	
//	virtual void BuildForMaster() const;
//	virtual void Build() const;
//};

//#endif


//NA22

#ifndef ACTION_HH
#define ACTION_HH
#include "G4VUserActionInitialization.hh"
#include "generator.hh"
#include "run.hh"
#include "event.hh"
#include "stepping.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public: 
    MyActionInitialization();
    ~MyActionInitialization();
    
    virtual void BuildForMaster() const;
    virtual void Build() const;

    // ← ADD THIS: lets main.cc get the generator pointer
    MyPrimaryGenerator* GetGenerator() const { return fGenerator; }

private:
    // ← ADD THIS: mutable so Build() can set it even though Build() is const
    mutable MyPrimaryGenerator* fGenerator;
};
#endif
