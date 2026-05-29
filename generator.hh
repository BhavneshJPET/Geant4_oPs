#ifndef MYPRIMARYGENERATOR_HH
#define MYPRIMARYGENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    MyPrimaryGenerator();
    ~MyPrimaryGenerator();
    void GeneratePrimaries(G4Event* anEvent) override;

    // Energy scanning — change without recompiling
    void     SetBeamEnergy(G4double E) { fBeamEnergy = E; }
    G4double GetBeamEnergy()           { return fBeamEnergy; }

private:
    G4ParticleGun* fParticleGun;

    //----------------------------------------------------------
    // O-PS BEAM START POSITION
    // Represents converter exit face
    // Na-22 and SiO2 NOT needed —
    // o-Ps is defined directly in PhysicsList
    //----------------------------------------------------------
    G4double fBeamStartZ;        // converter exit = -27.8 cm

    //----------------------------------------------------------
    // O-PS BEAM PROPERTIES
    // Ref: Cassidy et al., PRA 81, 012715 (2010)
    //      Nagashima et al., PRA 52, 258 (1995)
    //----------------------------------------------------------
    G4double fBeamEnergy;        // central energy  = 100 eV
    G4double fEnergySpread;      // relative spread = 5%
    G4double fAngularDivergence; // cone half-angle = 5 mrad
    G4double fBeamSigma;         // transverse spread = 0.5 mm
};

#endif
