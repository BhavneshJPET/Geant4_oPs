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
