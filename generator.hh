//#ifndef GENERATOR_HH
//#define GENERATOR_HH

//#include "G4VUserPrimaryGeneratorAction.hh"

//#include "G4ParticleGun.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4IonTable.hh"
//#include "G4Geantino.hh"
//#include "Randomize.hh"

//class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
//{
//public:
//    MyPrimaryGenerator();
//    ~MyPrimaryGenerator();

//    virtual void GeneratePrimaries(G4Event*);

//private:
//    G4ParticleGun *fParticleGun;
//};

//DEEP

//#ifndef GENERATOR_HH
//#define GENERATOR_HH

//#include "G4VUserPrimaryGeneratorAction.hh"
//#include "G4ParticleGun.hh"
//#include "G4Event.hh"
//#include "G4SystemOfUnits.hh"

//class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
//{
//public:
//    MyPrimaryGenerator();
//    ~MyPrimaryGenerator();

//    virtual void GeneratePrimaries(G4Event* anEvent);

//    // Beam parameter setters — call before /run/beamOn
//    void SetBeamEnergy(G4double energy)     { fBeamEnergy     = energy; }

//    // Getters for inspection / UI commands
//    G4double GetBeamEnergy()     const { return fBeamEnergy; }

//private:
//    G4ParticleGun* fParticleGun;
//    G4double       fBeamEnergy;      // kinetic energy of the o-Ps (default 100 eV)

//    // NOTE: fBeamDivergence was removed — the beam direction is sampled
//    // isotropically via G4RandomDirection() in generator.cc, which is the
//    // correct model for a point Ps source. Add back if you later want a
//    // collimated beam with a defined angular spread.
//};

//#endif


//NA22

//#ifndef MYPRIMARYGENERATOR_HH
//#define MYPRIMARYGENERATOR_HH

//#include "G4VUserPrimaryGeneratorAction.hh"
//#include "globals.hh"

//class G4ParticleGun;
//class G4Event;

//enum class BeamMode { THERMAL, BEAM };

//class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
//{
//public:
//    MyPrimaryGenerator();
//    ~MyPrimaryGenerator();
//    void GeneratePrimaries(G4Event* anEvent) override;

//    // Call these from your macro messenger or main()
//    // to scan energies without recompiling
//    void SetBeamEnergy(G4double E)  { fBeamEnergy = E; }
//    void SetBeamMode(BeamMode mode) { fBeamMode   = mode; }

//private:
//    G4ParticleGun* fParticleGun;

//    // Source
//    G4double fSourceSigma;
//    G4double fSourceZ;

//    // Converter
//    G4double fConverterZ;
//    G4double fConverterThickness;

//    // Beam
//    G4double  fBeamEnergy;
//    G4double  fEnergySpread;
//    G4double  fAngularDivergence;
//    BeamMode  fBeamMode;

//    // Thermal (MB)
//    G4double fOPsMinEnergy;
//    G4double fOPsMaxEnergy;
//    G4double fOPsKT;

//    G4double SampleMaxwellBoltzmann(G4double minE,
//                                    G4double maxE,
//                                    G4double kT);
//};

//#endif

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
