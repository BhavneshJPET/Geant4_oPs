#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"

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
