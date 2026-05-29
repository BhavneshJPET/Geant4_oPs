//#include "generator.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//{
//    // Create 1 particle per event
//    fParticleGun = new G4ParticleGun(1);

//    // Default settings (will be overridden by your run.mac)
//    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition *particle = particleTable->FindParticle("gamma");

//    fParticleGun->SetParticleDefinition(particle);
//    fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
//    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,1.,0.)); // Aiming at modules
//    fParticleGun->SetParticleEnergy(511.*keV);
//}

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
//{
//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}


// O-PS [Manual annihilation]

//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "Randomize.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//{
//    fParticleGun = new G4ParticleGun(1);

//    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition *particle = particleTable->FindParticle("o-Ps");

//    if(!particle) particle = particleTable->FindParticle("gamma");

//    fParticleGun->SetParticleDefinition(particle);
//} 

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//} // <--- MAKE SURE THIS BRACE IS HERE

//void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
//{
//    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition *gamma = particleTable->FindParticle("gamma");
//    fParticleGun->SetParticleDefinition(gamma);

//    // 1. Ortho-Positronium 3-Gamma Energy Sharing (Ore-Powell Approximation)
//    G4double E1, E2, E3;
//    do {
//        E1 = G4UniformRand() * 511. * keV;
//        E2 = G4UniformRand() * 511. * keV;
//        E3 = 1022. * keV - E1 - E2;
//    } while (E3 <= 0 || E3 >= 511. * keV); 

//    G4double energies[3] = {E1, E2, E3};

//    // 2. Fire the 3 photons from the center
//    for (int i = 0; i < 3; i++) {
//        fParticleGun->SetParticleEnergy(energies[i]);

//        // Random Isotropic Direction
//        G4double cosTheta = 2.0 * G4UniformRand() - 1.0;
//        G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
//        G4double phi = 2.0 * M_PI * G4UniformRand();

//        // G4ThreeVector dir(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
//        	G4ThreeVector dir(1.0, 0.0, 0.0);
//        fParticleGun->SetParticleMomentumDirection(dir);

//        // Position: 0,0,0 (Centred in your J-PET module)
//        fParticleGun->SetParticlePosition(G4ThreeVector(-10., 0., 0.));

//        fParticleGun->GeneratePrimaryVertex(anEvent);
//    }
//}

// O-PS [ Actual annihilation]

//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"
//#include "G4ParticleGun.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//{
//    fParticleGun = new G4ParticleGun(1);
//} 

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
//{
//    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition *ops = particleTable->FindParticle("o-Ps");

//    // If o-Ps isn't found for some reason, fallback to gamma to prevent crash
//    if(!ops) {
//        G4Exception("MyPrimaryGenerator::GeneratePrimaries", "PARTICLE_NOT_FOUND", 
//                    FatalException, "o-Ps particle was not found in the physics list!");
//}
//    fParticleGun->SetParticleDefinition(ops);

//    // o-Ps annihilation usually occurs at rest or very low energy
//    fParticleGun->SetParticleEnergy(10.0 *eV);

//    // Set position (e.g., 10cm from center along X-axis)
//    fParticleGun->SetParticlePosition(G4ThreeVector(-10.*cm, 0.*cm, 0.*cm));

//    // For a decay at rest, direction is arbitrary, but required by Geant4
//    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1.0, 0.0, 0.0));

//    // Generate the primary vertex
//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}

//DEEP
//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"
//#include "G4ParticleGun.hh"
//#include "Randomize.hh"
//#include "G4RandomDirection.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//    : fBeamEnergy(100.0 * eV)   // near-rest: o-Ps thermalizes quickly in matter,
//                                 // so 100 eV kinetic energy is physically reasonable
//                                 // for a positronium beam in J-PET geometry
//{
//    fParticleGun = new G4ParticleGun(1);
//}

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
//{
//    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition* ops = particleTable->FindParticle("o-Ps");

//    // Hard stop if o-Ps is missing — means physics list was not set up correctly
//    if (!ops) {
//        G4Exception("MyPrimaryGenerator::GeneratePrimaries",
//            "PARTICLE_NOT_FOUND", FatalException,
//            "o-Ps particle not found! Check MyPhysicsList::ConstructParticle()");
//    }

//    fParticleGun->SetParticleDefinition(ops);

//    // SetParticleEnergy sets KINETIC energy for massive particles.
//    // At 100 eV the o-Ps is nearly at rest — it will decay essentially
//    // at the origin, which is correct for a J-PET point source.
//    fParticleGun->SetParticleEnergy(fBeamEnergy);

//    // Source at detector center
//    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));

//    // Isotropic random direction — the 3 decay gammas will spread in
//    // all directions from the decay vertex regardless of this direction
//    // since G4PhaseSpaceDecayChannel samples directions independently
//    fParticleGun->SetParticleMomentumDirection(G4RandomDirection());

//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}



// DIRECTED BEAM

//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"
//#include "G4ParticleGun.hh"
//#include "Randomize.hh"
//#include "G4RandomDirection.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//    : fBeamEnergy(0.1 * keV)    // 0.1 keV — slow enough that o-Ps decays
//                                 // within a few cm INSIDE the 500mm scintillator
//                                 // Mean decay path at 1 keV ~ few cm
//{
//    fParticleGun = new G4ParticleGun(1);
//}

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
//{
//    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//    G4ParticleDefinition* ops = particleTable->FindParticle("o-Ps");

//    if (!ops) {
//        G4Exception("MyPrimaryGenerator::GeneratePrimaries",
//            "PARTICLE_NOT_FOUND", FatalException,
//            "o-Ps particle not found! Check MyPhysicsList::ConstructParticle()");
//    }

//    fParticleGun->SetParticleDefinition(ops);
//    fParticleGun->SetParticleEnergy(fBeamEnergy);

//    // ---------------------------------------------------------------
//    // J-PET Positronium Beam Geometry
//    //
//    // Scintillator bars are 500mm long centered at origin:
//    //   bar starts at Z = -25cm
//    //   bar center  at Z =   0cm
//    //   bar ends    at Z = +25cm
//    //
//    // Gun is placed 1cm BEFORE the scintillator face at Z = -26cm.
//    // At 1 keV kinetic energy, o-Ps is slow — mean decay path is
//    // a few cm — so it decays INSIDE the scintillator module.
//    //
//    // Z=-26cm   Z=-25cm         Z=0          Z=+25cm
//    //   |           |             |               |
//    // [gun] →1cm→ [bar face]  [bar center]   [bar end]
//    //               ←——— o-Ps decays somewhere here ———→
//    //                    → 3 gammas hit bar walls
//    // ---------------------------------------------------------------

//    // 1cm before scintillator face (face is at Z = -25cm)
//    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -27.*cm));

//    // Beam along +Z — along the long axis of the scintillator bars
//    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}

//// NA22

//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"
//#include "G4ParticleGun.hh"
//#include "Randomize.hh"
//#include "Randomize.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//{
//    fParticleGun = new G4ParticleGun(1);
//}

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
//{
//    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

//    // ---------------------------------------------------------------
//    // Na-22 SOURCE GEOMETRY
//    // Na-22 source is a small spot — typically 1-2 mm diameter foil
//    // Placed at the center of the J-PET detector (origin)
//    // ---------------------------------------------------------------

//    // Source spot size: Gaussian with sigma = 0.5 mm (1mm diameter source)
//    G4double sourceSigma = 0.5 * mm;
//    G4double x_source = G4RandGauss::shoot(0.0, sourceSigma);
//    G4double y_source = G4RandGauss::shoot(0.0, sourceSigma);
//    G4double z_source = -27.0 * cm; // source at center

//    G4ThreeVector sourcePosition(x_source, y_source, z_source);

//    // ---------------------------------------------------------------
//    // PARTICLE 1: Prompt 1274 keV gamma from Ne-22* de-excitation
//    // This is emitted simultaneously with the positron
//    // Direction: random (isotropic)
//    // ---------------------------------------------------------------
//    G4ParticleDefinition* gamma = particleTable->FindParticle("gamma");

//    // Random isotropic direction for prompt gamma
//    G4double cosTheta1 = 2.0 * G4UniformRand() - 1.0;
//    G4double sinTheta1 = std::sqrt(1.0 - cosTheta1 * cosTheta1);
//    G4double phi1      = 2.0 * CLHEP::pi * G4UniformRand();
//    G4ThreeVector promptGammaDir(sinTheta1 * std::cos(phi1),
//                                 sinTheta1 * std::sin(phi1),
//                                 cosTheta1);

//    fParticleGun->SetParticleDefinition(gamma);
//    fParticleGun->SetParticleEnergy(1274.0 * keV);
//    fParticleGun->SetParticlePosition(sourcePosition);
//    fParticleGun->SetParticleMomentumDirection(promptGammaDir);
//    fParticleGun->GeneratePrimaryVertex(anEvent);

//    // ---------------------------------------------------------------
//    // PARTICLE 2: o-Ps
//    // In J-PET experiments, a porous material (like aerogel or 
//    // polymer film) surrounds the source to PROMOTE o-Ps formation.
//    // o-Ps is emitted from the material surface with a few eV 
//    // kinetic energy — typically modelled as:
//    //   - Maxwell-Boltzmann energy between 0.1 and 3 eV (room temp)
//    //   - Isotropic direction from source
//    // ---------------------------------------------------------------
//    G4ParticleDefinition* ops = particleTable->FindParticle("o-Ps");
//    if (!ops) {
//        G4Exception("MyPrimaryGenerator::GeneratePrimaries",
//            "PARTICLE_NOT_FOUND", FatalException,
//            "o-Ps not found! Check physics list.");
//    }

//    // Realistic o-Ps kinetic energy: thermal ~room temperature
//    // kT at 300K ~ 0.026 eV, but o-Ps in porous media 
//    // has effective temperature of ~100-500 K
//    G4double kT      = 0.05 * eV;   // ~600 K effective temperature
//    G4double minE    = 0.01 * eV;
//    G4double maxE    = 3.0  * eV;
//    G4double ops_energy = SampleMaxwellBoltzmann(minE, maxE, kT);

//    // Random isotropic direction for o-Ps
//    G4double cosTheta2 = 2.0 * G4UniformRand() - 1.0;
//    G4double sinTheta2 = std::sqrt(1.0 - cosTheta2 * cosTheta2);
//    G4double phi2      = 2.0 * CLHEP::pi * G4UniformRand();
//    G4ThreeVector opsDir(sinTheta2 * std::cos(phi2),
//                         sinTheta2 * std::sin(phi2),
//                         cosTheta2);

//    fParticleGun->SetParticleDefinition(ops);
//    fParticleGun->SetParticleEnergy(ops_energy);
//    fParticleGun->SetParticlePosition(sourcePosition);
//    fParticleGun->SetParticleMomentumDirection(opsDir);
//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}

//// ---------------------------------------------------------------
//// Maxwell-Boltzmann energy sampler
//// Same method your colleague uses for antihydrogen
//// ---------------------------------------------------------------
//G4double MyPrimaryGenerator::SampleMaxwellBoltzmann(G4double minE,
//                                                     G4double maxE,
//                                                     G4double kT)
//{
//    G4double E_peak  = kT / 2.0;
//    G4double maxProb;

//    if (E_peak >= minE && E_peak <= maxE) {
//        maxProb = std::sqrt(E_peak) * std::exp(-E_peak / kT);
//    } else {
//        G4double probMin = std::sqrt(minE) * std::exp(-minE / kT);
//        G4double probMax = std::sqrt(maxE) * std::exp(-maxE / kT);
//        maxProb = std::max(probMin, probMax);
//    }

//    while (true) {
//        G4double E    = minE + G4UniformRand() * (maxE - minE);
//        G4double prob = std::sqrt(E) * std::exp(-E / kT);
//        if (G4UniformRand() * maxProb <= prob)
//            return E;
//    }
//}


//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"
//#include "G4ParticleGun.hh"
//#include "Randomize.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//    //----------------------------------------------------------
//    // Na-22 SOURCE
//    //----------------------------------------------------------
//    : fSourceSigma       (0.5  * mm),    // Na-22 foil spot ~1mm diameter
//      fSourceZ           (-30.0 * cm),   // Na-22 foil position

//    //----------------------------------------------------------
//    // POROUS SiO2 CONVERTER
//    // e+ from Na-22 enters here, thermalizes, forms o-Ps
//    // o-Ps exits from the BACK FACE of converter
//    //----------------------------------------------------------
//      fConverterZ        (-28.0 * cm),   // converter front face
//      fConverterThickness(2.0  * mm),    // 2mm thick porous SiO2
//                                         // back face = -27.8 cm

//    //----------------------------------------------------------
//    // BEAM PARAMETERS
//    //----------------------------------------------------------
//      fBeamEnergy        (100.0 * eV),   // DEFAULT scan energy
//                                         // change via SetBeamEnergy()
//      fEnergySpread      (0.05),         // 5% energy spread dE/E
//                                         // realistic for converter
//      fAngularDivergence (5.0  * mrad),  // beam cone half-angle
//                                         // from converter geometry

//    //----------------------------------------------------------
//    // THERMAL PARAMETERS (kept for THERMAL mode)
//    //----------------------------------------------------------
//      fOPsMinEnergy      (0.01 * eV),
//      fOPsMaxEnergy      (3.0  * eV),
//      fOPsKT             (0.05 * eV),

//      fBeamMode          (BeamMode::BEAM) // default: beam mode
//{
//    fParticleGun = new G4ParticleGun(1);
//}

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
//{
//    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

//    // ==========================================================
//    // STEP 1: Na-22 source position
//    // ==========================================================
//    G4double x_src = G4RandGauss::shoot(0.0, fSourceSigma);
//    G4double y_src = G4RandGauss::shoot(0.0, fSourceSigma);
//    G4ThreeVector sourcePos(x_src, y_src, fSourceZ);

//    // ==========================================================
//    // STEP 3: o-Ps BEAM
//    // ==========================================================
//    G4ParticleDefinition* ops = particleTable->FindParticle("o-Ps");
//    if (!ops) {
//        G4Exception("MyPrimaryGenerator::GeneratePrimaries",
//            "PARTICLE_NOT_FOUND", FatalException,
//            "o-Ps not found! Check MyPhysicsList::ConstructParticle()");
//    }

//    // ----------------------------------------------------------
//    // o-Ps START POSITION = converter back face
//    // x,y: source spot + small diffusion spread inside pores
//    // z:   converter back face
//    // ----------------------------------------------------------
//    G4double conv_backface = fConverterZ + fConverterThickness; // -27.8 cm
//    G4double x_ops = x_src + G4RandGauss::shoot(0.0, 0.3*mm); // diffusion
//    G4double y_ops = y_src + G4RandGauss::shoot(0.0, 0.3*mm); // diffusion
//    G4ThreeVector opsPos(x_ops, y_ops, conv_backface);

//    // ----------------------------------------------------------
//    // o-Ps ENERGY — depends on beam mode
//    // ----------------------------------------------------------
//    G4double ops_energy = 0.0;

//    if (fBeamMode == BeamMode::THERMAL)
//    {
//        // Original thermal mode — MB distribution
//        // Use this to VERIFY your setup before enabling beam
//        ops_energy = SampleMaxwellBoltzmann(fOPsMinEnergy,
//                                            fOPsMaxEnergy,
//                                            fOPsKT);
//    }
//    else // BeamMode::BEAM
//    {
//        // BEAM MODE — Gaussian spread around scan energy
//        // fEnergySpread = 5% → realistic for re-accelerated beam
//        ops_energy = std::abs(
//            G4RandGauss::shoot(fBeamEnergy,
//                               fEnergySpread * fBeamEnergy));
//    }

//    // ----------------------------------------------------------
//    // o-Ps DIRECTION — forward +Z with small angular divergence
//    // This is what makes it a BEAM vs isotropic source
//    // divergence = 5 mrad → beam stays ~1mm wide over 20cm
//    // ----------------------------------------------------------
////    G4double dx = G4RandGauss::shoot(0.0, fAngularDivergence);
////    G4double dy = G4RandGauss::shoot(0.0, fAngularDivergence);
////    G4ThreeVector opsDir(dx, dy, 1.0);
////    opsDir = opsDir.unit(); // normalize — critical!

//      G4ThreeVector opsDir(0.0, 0.0, 1.0);

//    fParticleGun->SetParticleDefinition(ops);
//    fParticleGun->SetParticleEnergy(ops_energy);
//    fParticleGun->SetParticlePosition(opsPos);
//    fParticleGun->SetParticleMomentumDirection(opsDir);
//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}

//// ==========================================================
//// Maxwell-Boltzmann sampler — UNCHANGED, kept for THERMAL mode
//// ==========================================================
//G4double MyPrimaryGenerator::SampleMaxwellBoltzmann(G4double minE,
//                                                     G4double maxE,
//                                                     G4double kT)
//{
//    G4double E_peak = kT / 2.0;
//    G4double maxProb;

//    if (E_peak >= minE && E_peak <= maxE) {
//        maxProb = std::sqrt(E_peak) * std::exp(-E_peak / kT);
//    } else {
//        G4double probMin = std::sqrt(minE) * std::exp(-minE / kT);
//        G4double probMax = std::sqrt(maxE) * std::exp(-maxE / kT);
//        maxProb = std::max(probMin, probMax);
//    }

//    while (true) {
//        G4double E    = minE + G4UniformRand() * (maxE - minE);
//        G4double prob = std::sqrt(E) * std::exp(-E / kT);
//        if (G4UniformRand() * maxProb <= prob)
//            return E;
//    }
//}

//#include "generator.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4ParticleTable.hh"
//#include "G4Event.hh"
//#include "G4ParticleGun.hh"
//#include "Randomize.hh"

//MyPrimaryGenerator::MyPrimaryGenerator()
//    //----------------------------------------------------------
//    // O-PS BEAM START POSITION
//    // = converter exit face z = -27.8 cm
//    // Na-22 and SiO2 NOT simulated —
//    // o-Ps defined directly in PhysicsList
//    // Ref: Cassidy et al., PRA 81, 012715 (2010)
//    //----------------------------------------------------------
//    : fBeamStartZ      (-27.8 * cm),   // converter exit face

//    //----------------------------------------------------------
//    // O-PS BEAM PROPERTIES
//    // Ref: Cassidy & Mills, Nature 449, 195 (2007)
//    //      Nagashima et al., PRA 52, 258 (1995)
//    //----------------------------------------------------------
//      fBeamEnergy      (100.0 * eV),   // re-accelerated beam
//      fEnergySpread    (0.05),         // 5% energy resolution
//      fAngularDivergence(5.0 * mrad),  // pore collimation
//      fBeamSigma       (0.5  * mm)     // transverse beam size
//{
//    fParticleGun = new G4ParticleGun(1);
//}

//MyPrimaryGenerator::~MyPrimaryGenerator()
//{
//    delete fParticleGun;
//}

//void MyPrimaryGenerator::GeneratePrimaries(
//    G4Event* anEvent)
//{
//    G4ParticleTable* particleTable =
//        G4ParticleTable::GetParticleTable();

//    //----------------------------------------------------------
//    // STEP 1 — Get o-Ps particle definition
//    // o-Ps is defined in PhysicsList
//    // This is the PRIMARY beam particle
//    // Na-22 and SiO2 NOT needed here
//    //----------------------------------------------------------
//    G4ParticleDefinition* ops =
//        particleTable->FindParticle("o-Ps");

//    if(!ops)
//    {
//        G4Exception(
//            "MyPrimaryGenerator::GeneratePrimaries",
//            "PARTICLE_NOT_FOUND",
//            FatalException,
//            "o-Ps not found! Check PhysicsList.");
//    }

//    //----------------------------------------------------------
//    // STEP 2 — O-PS START POSITION
//    // Starts at converter exit face z = -27.8 cm
//    // x,y: Gaussian spread representing
//    //      beam transverse profile
//    // This represents the combined effect of:
//    // - Na-22 source spot
//    // - SiO2 pore diffusion
//    // (both implicit in fBeamSigma)
//    //----------------------------------------------------------
//    G4double x_beam = G4RandGauss::shoot(
//        0.0, fBeamSigma);
//    G4double y_beam = G4RandGauss::shoot(
//        0.0, fBeamSigma);

//    G4ThreeVector opsPos(
//        x_beam,
//        y_beam,
//        fBeamStartZ);   // z = -27.8 cm

//    //----------------------------------------------------------
//    // STEP 3 — O-PS BEAM ENERGY
//    // 100 eV = re-accelerated beam energy
//    // 5% spread = realistic energy resolution
//    // Gaussian sampling around central energy
//    //----------------------------------------------------------
//    G4double ops_energy = std::abs(
//        G4RandGauss::shoot(
//            fBeamEnergy,
//            fEnergySpread * fBeamEnergy));

//    //----------------------------------------------------------
//    // STEP 4 — O-PS BEAM DIRECTION
//    // Forward +Z with 5 mrad angular divergence
//    // Represents pore collimation effect of SiO2
//    // MUST normalize direction vector!
//    //----------------------------------------------------------
//    G4double dx = G4RandGauss::shoot(
//        0.0, fAngularDivergence);
//    G4double dy = G4RandGauss::shoot(
//        0.0, fAngularDivergence);
//    G4ThreeVector opsDir(dx, dy, 1.0);
//    opsDir = opsDir.unit();  // normalize — critical!

//    //----------------------------------------------------------
//    // STEP 5 — FIRE O-PS BEAM
//    // G4ParticleGun fires o-Ps into simulation
//    // All beam properties set here
//    //----------------------------------------------------------
//    fParticleGun->SetParticleDefinition(ops);
//    fParticleGun->SetParticleEnergy(ops_energy);
//    fParticleGun->SetParticlePosition(opsPos);
//    fParticleGun->SetParticleMomentumDirection(opsDir);
//    fParticleGun->GeneratePrimaryVertex(anEvent);
//}

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
