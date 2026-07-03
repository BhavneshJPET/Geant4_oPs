#include "generator.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include <cmath>

MyPrimaryGenerator::MyPrimaryGenerator()
    : fBeamStartZ      (-25.2 * cm),
      fBeamEnergy      (1.0 * eV),
      fEnergySpread    (0.05),
      fAngularDivergence(0.0),
      fBeamSigma       (3.0 * mm)
{
    fParticleGun = new G4ParticleGun(1);

    // ---------------------------------------------------------------
    // SOLID-ANGLE CONE HALF-ANGLE — derived from geometry
    //
    // Computes the half-angle of the beam cone from the source position
    // to the CORNER of the stopper aperture, so the beam is fired
    // uniformly within that solid angle and actually illuminates the
    // full grating/stopper aperture (per supervisor's instruction).
    //
    // counterZ_geometry must match the stopper's actual Z position in
    // construction.cc. If construction.cc changes the grating/counter
    // placement, this value must be updated to match.
    // ---------------------------------------------------------------
    const G4double counterZ_geometry = -12.99 * cm;
    const G4double halfAperture      =   3.5  * cm;  // stopper half-width (straight edge)
    const G4double edgeRadius        = halfAperture * std::sqrt(2.0); // corner radius
    const G4double distanceToStopper = counterZ_geometry - fBeamStartZ;

    fAngularDivergence = std::atan(edgeRadius / std::abs(distanceToStopper));

    G4cout << "[MyPrimaryGenerator] Cone half-angle = "
           << fAngularDivergence / mrad << " mrad  ("
           << "halfAperture=" << halfAperture / cm << " cm, "
           << "cornerRadius=" << edgeRadius / cm   << " cm, "
           << "distance="     << std::abs(distanceToStopper) / cm << " cm)"
           << G4endl;
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    //----------------------------------------------------------
    // STEP 1 — Get o-Ps particle definition
    //----------------------------------------------------------
    G4ParticleDefinition* ops = particleTable->FindParticle("o-Ps");
    if(!ops)
    {
        G4Exception(
            "MyPrimaryGenerator::GeneratePrimaries",
            "PARTICLE_NOT_FOUND",
            FatalException,
            "o-Ps not found! Check PhysicsList.");
    }

    //----------------------------------------------------------
    // STEP 2 — START POSITION: TRUE POINT SOURCE
    // Removing G4RandGauss ensures all rays diverge from a single 
    // sharp vertex, matching the solid-angle cone geometry perfectly.
    //----------------------------------------------------------
    G4double x_beam = 0.0; 
    G4double y_beam = 0.0; 
    G4ThreeVector opsPos(x_beam, y_beam, fBeamStartZ);

    //----------------------------------------------------------
    // STEP 3 — ENERGY
    //----------------------------------------------------------
    G4double ops_energy = std::abs(
        G4RandGauss::shoot(fBeamEnergy, fEnergySpread * fBeamEnergy));

    //----------------------------------------------------------
    // STEP 4 — DIRECTION: uniform-in-solid-angle cone sampling
    // Preserved exactly as requested to maintain full aperture illumination.
    //----------------------------------------------------------
    G4double maxTheta = fAngularDivergence;

    G4double cosTheta = 1.0 - G4UniformRand() * (1.0 - std::cos(maxTheta));
    G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    G4double phi      = G4UniformRand() * 2.0 * M_PI;

    G4double dx = sinTheta * std::cos(phi);
    G4double dy = sinTheta * std::sin(phi);
    G4double dz = cosTheta;

    G4ThreeVector opsDir(dx, dy, dz); 

    //----------------------------------------------------------
    // STEP 5 — FIRE O-PS BEAM
    //----------------------------------------------------------
    fParticleGun->SetParticleDefinition(ops);
    fParticleGun->SetParticleEnergy(ops_energy);
    fParticleGun->SetParticlePosition(opsPos);
    fParticleGun->SetParticleMomentumDirection(opsDir);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    G4int eventID = anEvent->GetEventID();
    if(eventID % 100000 == 0) {
        G4cout << "Event: " << eventID << G4endl;
    }
}
