#include "generator.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include <cmath>

MyPrimaryGenerator::MyPrimaryGenerator()
    //----------------------------------------------------------
    // O-PS BEAM — Modular J-PET configuration
    // Beam starts at z = -25.2 cm (252 mm from center)
    // Travels in +Z direction, decays inside the detector ring
    //----------------------------------------------------------
    : fBeamStartZ      (-25.2 * cm),   // ✅ UPDATED: was -30.0 cm
      fBeamEnergy      (100.0 * eV),   // kinetic energy
      fEnergySpread    (0.05),         // 5% energy resolution
      fAngularDivergence(5.0 * mrad),  // pore collimation
      fBeamSigma       (0.5  * mm)     // transverse beam size
{
    fParticleGun = new G4ParticleGun(1);
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    G4ParticleTable* particleTable =
        G4ParticleTable::GetParticleTable();

    //----------------------------------------------------------
    // STEP 1 — Get o-Ps particle definition
    //----------------------------------------------------------
    G4ParticleDefinition* ops =
        particleTable->FindParticle("o-Ps");
    if(!ops)
    {
        G4Exception(
            "MyPrimaryGenerator::GeneratePrimaries",
            "PARTICLE_NOT_FOUND",
            FatalException,
            "o-Ps not found! Check PhysicsList.");
    }

    //----------------------------------------------------------
    // STEP 2 — START POSITION
    // Beam starts at z = -25.2 cm (252 mm from center)
    // Small Gaussian spread in x, y (beam spot size)
    //----------------------------------------------------------
    G4double x_beam = G4RandGauss::shoot(0.0, fBeamSigma);
    G4double y_beam = G4RandGauss::shoot(0.0, fBeamSigma);
    G4ThreeVector opsPos(x_beam, y_beam, fBeamStartZ);

    //----------------------------------------------------------
    // STEP 3 — ENERGY
    // Gaussian spread around mean energy
    //----------------------------------------------------------
    G4double ops_energy = std::abs(
        G4RandGauss::shoot(
            fBeamEnergy,
            fEnergySpread * fBeamEnergy));

    //----------------------------------------------------------
    // STEP 4 — DIRECTION: along +Z axis
    // dx, dy are tiny Gaussian tilts (5 mrad divergence)
    // dz = 1.0 means beam goes in +Z direction
    // Direction is dimensionless — no units!
    //----------------------------------------------------------
    G4double dx = G4RandGauss::shoot(0.0, fAngularDivergence);
    G4double dy = G4RandGauss::shoot(0.0, fAngularDivergence);
    G4double dz = 1.0;
    G4ThreeVector opsDir(dx, dy, dz);
    opsDir = opsDir.unit();  // normalize

    //----------------------------------------------------------
    // STEP 5 — FIRE O-PS BEAM
    //----------------------------------------------------------
    fParticleGun->SetParticleDefinition(ops);
    fParticleGun->SetParticleEnergy(ops_energy);
    fParticleGun->SetParticlePosition(opsPos);
    fParticleGun->SetParticleMomentumDirection(opsDir);
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
