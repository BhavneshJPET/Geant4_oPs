//#include "physics.hh"

//MyPhysicsList::MyPhysicsList()
//{
//	RegisterPhysics (new G4EmStandardPhysics());
//	RegisterPhysics (new G4OpticalPhysics());
//	RegisterPhysics (new G4DecayPhysics());
//	RegisterPhysics (new G4RadioactiveDecayPhysics());
//	RegisterPhysics (new G4StoppingPhysics());
//}

//MyPhysicsList::~MyPhysicsList()
//{}



//O-PS

//#include "physics.hh"
//#include "G4ParticleDefinition.hh"
//#include "G4ParticleTable.hh"
//#include "G4ProcessManager.hh"
//#include "G4Decay.hh"
//#include "G4SystemOfUnits.hh"

//MyPhysicsList::MyPhysicsList()
//{
//    RegisterPhysics(new G4EmStandardPhysics());
//    RegisterPhysics(new G4OpticalPhysics());
//    RegisterPhysics(new G4DecayPhysics());
//    RegisterPhysics(new G4RadioactiveDecayPhysics());
//    RegisterPhysics(new G4StoppingPhysics());
//}

//MyPhysicsList::~MyPhysicsList()
//{}

//// You need to override the ConstructParticle method to add o-Ps
//void MyPhysicsList::ConstructParticle()
//{
//    // Call the base class particles (electrons, gammas, etc.)
//    G4VModularPhysicsList::ConstructParticle();

//    // Define Ortho-positronium
//    // Parameters: Name, Mass, Width, Charge, Spin, Parity, C-conj, Isospin, I3, G-parity, Type, Lepton#, Baryon#, Encoding, Stable, Lifetime, DecayTable, Shortlived
// G4ParticleDefinition* ops = new G4ParticleDefinition(
//        "o-Ps", 1.02199*MeV, 0.0*MeV, 0, 
//        1, -1, -1, 0, 0, 0, 
//        "lepton", 0, 0, 0, 
//        false, 142.0*ns, NULL, false
//    );
//    
//    // 2. Define the Annihilation (Decay) into 3 Gammas
//    G4DecayTable* table = new G4DecayTable();
//    
//    // G4PhaseSpaceDecayChannel(ParentName, BranchingRatio, NumberOfDaughters, Daughter1, Daughter2, Daughter3)
//    G4VDecayChannel* mode = new G4PhaseSpaceDecayChannel("o-Ps", 1.0, 3, "gamma", "gamma", "gamma");
//    
//    table->Insert(mode);
//    ops->SetDecayTable(table);
//}

//// You need to override ConstructProcess to ensure o-Ps can actually decay
//void MyPhysicsList::ConstructProcess()
//{
//    // Call the base class processes (EM, Optical, etc.)
//    G4VModularPhysicsList::ConstructProcess();

//    G4ParticleTable* table = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition* ops = table->FindParticle("o-Ps");

//    if (ops) {
//        G4ProcessManager* pmanager = ops->GetProcessManager();
//        // Attach the decay process specifically to o-Ps
//        pmanager->AddProcess(new G4Decay(), 1, -1, 1); 
//    }
//}

//Deep

#include "physics.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"
#include "G4SystemOfUnits.hh"
#include "G4DecayTable.hh"
#include "G4PhaseSpaceDecayChannel.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"

MyPhysicsList::MyPhysicsList()
{
    // DecayPhysics adds the G4Decay process to all unstable particles.
    // EmStandardPhysics handles gamma interactions in the scintillator
    // (Compton, photoelectric, pair production).
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4EmStandardPhysics());
}

MyPhysicsList::~MyPhysicsList() {}

void MyPhysicsList::ConstructParticle()
{
    // Build all particles from registered physics modules first
    G4VModularPhysicsList::ConstructParticle();

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    //Define o-Ps
    G4ParticleDefinition* ops = particleTable->FindParticle("o-Ps");
    if (!ops) {
        ops = new G4ParticleDefinition(
            "o-Ps",                        // name
            2.0 * 0.510999 * MeV,          // mass = 2 * electron mass (1.021998 MeV)
            0.0,                           // width (natural line width — negligible)
            0.0,                           // charge
            2,                             // 2*spin  (spin = 1, so pass 2)
            -1,                            // parity
            -1,                            // C-conjugation
            0, 0, 0,                       // isospin I, I3, Y
            "boson",                       // particle type — "boson" avoids lepton
                                           // number conflicts with EM physics
            0, 0, 0,                       // lepton number, baryon number, PDG encoding
            false,                         // not stable
            142.0 * ns,                    // ortho-Ps vacuum lifetime
            nullptr,                       // decay table set below
            false                          // not a shortlived particle
        );

        // One decay channel: o-Ps → γ γ γ  (BR = 1.0)
        G4DecayTable* table = new G4DecayTable();
        table->Insert(new G4PhaseSpaceDecayChannel(
            "o-Ps", 1.0, 3, "gamma", "gamma", "gamma"));
        ops->SetDecayTable(table);

        G4cout << "\n========================================" << G4endl;
        G4cout << "o-Ps defined:" << G4endl;
        G4cout << "  mass     = " << ops->GetPDGMass()/MeV << " MeV" << G4endl;
        G4cout << "  lifetime = " << ops->GetPDGLifeTime()/ns << " ns" << G4endl;
        G4cout << "  decay    = 3 gammas (BR=1.0)" << G4endl;
        G4cout << "========================================\n" << G4endl;
    }
}

void MyPhysicsList::ConstructProcess()
{
    G4VModularPhysicsList::ConstructProcess();

    G4ParticleDefinition* ops =
        G4ParticleTable::GetParticleTable()->FindParticle("o-Ps");

    if (ops) {
        G4ProcessManager* pmanager = ops->GetProcessManager();
        if (pmanager) {
            G4Decay* decay = new G4Decay();
            pmanager->AddProcess(decay);
//            // At-rest decay (o-Ps is slow, decays essentially at rest in J-PET)
//            pmanager->SetProcessOrdering(decay, idxAtRest);
//            // Also allow in-flight decay for the small kinetic energy beam
//            //pmanager->SetProcessOrdering(decay, idxPostStep);
          pmanager->SetProcessOrdering(
                decay, idxAtRest);
           pmanager->SetProcessOrdering(
                decay, idxPostStep);

            G4cout << "Decay (PostStep) attached"
                   << " to o-Ps" << G4endl;          
        }
    }
}
