//#ifndef PHYSICS_HH
//#define PHYSICS_HH

//#include "G4VModularPhysicsList.hh"
//#include "G4EmStandardPhysics.hh"
//#include "G4OpticalPhysics.hh"
//#include "G4DecayPhysics.hh"

//class MyPhysicsList : public G4VModularPhysicsList
//{
//public:
//    MyPhysicsList();
//    ~MyPhysicsList();
//    
//    virtual void ConstructParticle();
//    virtual void ConstructProcess();
//};

//#endif


//DEEP

#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"

// NOTE: G4OpticalPhysics was removed from the include list.
// Optical photon tracking is expensive and not needed for the
// J-PET 3-gamma coincidence simulation at this stage.
// Add it back (and register it in the constructor) only if you
// want to simulate scintillation light propagation.

class MyPhysicsList : public G4VModularPhysicsList
{
public:
    MyPhysicsList();
    ~MyPhysicsList();

    // Defines all particles including the custom o-Ps
    virtual void ConstructParticle();

    // Attaches decay process to o-Ps process manager
    virtual void ConstructProcess();
};

#endif
