////#include "construction.hh"

////MyDetectorConstruction::MyDetectorConstruction()
////{
////    nCols = 10;
////    nRows = 10;

////    fMessenger = new G4GenericMessenger(this, "/detector/", "Detector Construction");

////    fMessenger->DeclareProperty("nCols", nCols, "Number of cols");
////    fMessenger->DeclareProperty("nRows", nRows, "Number of rows");
////    fMessenger->DeclareProperty("ischerenkov", isCherenkov, "Construct Cherenkov detector");
////    fMessenger->DeclareProperty("isscintillator", isScintillator, "Construct Scintillator");
////    //fMessenger->DeclareProperty("isTOF", isScintillator, "Construct Time Of Flight");

////    DefineMaterials();

////    isCherenkov = false;
////    isScintillator = true;
////    //isTOF = true;
////    
////    // Dimensions for the world volume
////    xWorld = 5*m;
////    yWorld = 5*m;
////    zWorld = 5*m;
////}

////MyDetectorConstruction::~MyDetectorConstruction()
////{}

////void MyDetectorConstruction::DefineMaterials()
////{
////    G4NistManager *nist = G4NistManager::Instance();

////    C = nist->FindOrBuildElement("C");
////    SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
////    Aerogel = new G4Material("Aerogel", 0.200*g/cm3, 3);
////    worldMat = nist->FindOrBuildMaterial("G4_AIR");
////    H2O = new G4Material("H2O", 1.000*g/cm3, 2);

////    SiO2->AddElement(nist->FindOrBuildElement("Si"), 1);
////    SiO2->AddElement(nist->FindOrBuildElement("O"), 2);

////    H2O->AddElement(nist->FindOrBuildElement("H"), 2);
////    H2O->AddElement(nist->FindOrBuildElement("O"), 1);

////    Aerogel->AddMaterial(SiO2, 62.5*perCent);
////    Aerogel->AddMaterial(H2O, 37.4*perCent);
////    Aerogel->AddElement(C, 0.1*perCent);

////    G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
////    G4double rindexAerogel[2] = {1.1, 1.1};
////    G4double rindexWorld[2] = {1.0, 1.0};
////    G4double rindexNaI[2] = {1.78, 1.78};
////    G4double reflectivity[2] = {1.0, 1.0};

////    G4MaterialPropertiesTable *mptAerogel = new G4MaterialPropertiesTable();
////    mptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);
////    Aerogel->SetMaterialPropertiesTable(mptAerogel);

////    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
////    mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);

////    Na = nist->FindOrBuildElement("Na");
////    I = nist->FindOrBuildElement("I");
////    NaI = new G4Material("NaI", 3.67*g/cm3, 2);
////    NaI->AddElement(Na, 1);
////    NaI->AddElement(I, 1);

////    G4double fraction[2] = {1.0, 1.0};
////    G4MaterialPropertiesTable *mptNaI = new G4MaterialPropertiesTable();
////    
////    mptNaI->AddProperty("RINDEX", energy, rindexNaI, 2);
////    mptNaI->AddProperty("SCINTILLATIONCOMPONENT1", energy, fraction, 2);
////    mptNaI->AddConstProperty("SCINTILLATIONYIELD", 38./keV);
////    mptNaI->AddConstProperty("RESOLUTIONSCALE", 1.0);
////    mptNaI->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250.*ns);
////    mptNaI->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
////    
////    NaI->SetMaterialPropertiesTable(mptNaI);
////    
////    worldMat->SetMaterialPropertiesTable(mptWorld);
////    
////    //mirrorSurface = new G4OpticalSurface("mirrorSurface");
////    
////   // mirrorSurface->SetType(dielectric_metal);
////   // mirrorSurface->SetFinish(ground);
////   // mirrorSurface->SetModel(unified);
////    
////    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();

////   // mptMirror->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
////   
////}

////void MyDetectorConstruction::ConstructScintillator()
////{
//// 
////    solidScintillator = new G4Box("solidScintillator", 5*cm, 5*cm, 6*cm);

////    logicScintillator = new G4LogicalVolume(solidScintillator, NaI, "logicalScintillator");

////    fScoringVolume = logicScintillator;

////  // new G4LogicalSkinSurface("skin", logicScintillator, mirrorSurface);

////    solidDetector = new G4Box("solidDetector", 1.*cm, 5.*cm, 6*cm);
////    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");

////    for(G4int i = 0; i < 6; i++)
////    {
////        for(G4int j = 0; j < 16; j++)
////        {
////            G4Rotate3D rotZ(j*22.5*deg, G4ThreeVector(0,0,1));
////                  
////            // Positioning the Scintillator
////            G4Translate3D transXScint(G4ThreeVector(5./tan(22.5/2*deg)*cm + 5.*cm, 0.*cm, -40.*cm + i*15*cm));
////            
////            G4Transform3D transformScint = (rotZ)*(transXScint);

////            // Positioning the Detector (SiPM/PMT) behind the Scintillator
////            G4Translate3D transXDet(G4ThreeVector(5./tan(22.5/2*deg)*cm + 6.*cm + 5.*cm, 0.*cm, -40.*cm + i*15*cm));
////            G4Transform3D transformDet = (rotZ)*(transXDet);

////            physScintillator = new G4PVPlacement(transformScint, logicScintillator, "physScintillator", logicWorld, false, j + i*16, true);
////            physDetector = new G4PVPlacement(transformDet, logicDetector, "physDetector", logicWorld, false, j + i*16, true);
////        }
////    }
////}

////void MyDetectorConstruction::ConstructCherenkov()
////{
////    solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, 0.01*m);
////    logicRadiator = new G4LogicalVolume(solidRadiator, Aerogel, "logicalRadiator");
////    fScoringVolume = logicRadiator;
////    physRadiator = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.25*m), logicRadiator, "physRadiator", logicWorld, false, 0, true);

////    solidDetector = new G4Box("solidDetector", xWorld/nRows, xWorld/nCols, 0.01*m);
////    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");

////    for(G4int i = 0; i < nRows; i++)
////    {
////        for(G4int j = 0; j < nCols; j++)
////        {
////            physDetector = new G4PVPlacement(0, G4ThreeVector(-0.5*m+(i+0.5)*m/nRows, -0.5*m+(j+0.5)*m/nCols, 0.49*m), logicDetector, "physDetector", logicWorld, false, j+i*nCols, true);
////        }
////    }
////}

////void MyDetectorConstruction::ConstructTOF()
////{
////    solidDetector = new G4Box("solidDetector", 1.*m, 1.*m, 0.1*m);
////    
////    logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
////    
////    physDetector = new G4PVPlacement(0, G4ThreeVector(0.*m, 0.*m, -4*m), logicDetector, "physDetector", logicWorld, false, 0, true);
////    
////    physDetector = new G4PVPlacement(0, G4ThreeVector(0.*m, 0.*m, 3*m), logicDetector, "physDetector", logicWorld, false, 1, true);
////}

////G4VPhysicalVolume *MyDetectorConstruction::Construct()
////{
////    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
////    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
////    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

////    if(isCherenkov)
////        ConstructCherenkov();

////    if(isScintillator)
////        ConstructScintillator();

////   // if(isTOF)
////    //    ConstructTOF();
////  //  return physWorld;
////}

////void MyDetectorConstruction::ConstructSDandField()
////{
////    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

////    if(logicDetector != NULL)
////        logicDetector->SetSensitiveDetector(sensDet);
////}




//#include "construction.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//{
//    // Define world size (50cm x 50cm x 50cm)
//    xWorld = 0.5*m;
//    yWorld = 0.5*m;
//    zWorld = 0.5*m;

//    DefineMaterials();
//}

//MyDetectorConstruction::~MyDetectorConstruction() {}

//void MyDetectorConstruction::DefineMaterials()
//{
//    G4NistManager *nist = G4NistManager::Instance();
//    worldMat = nist->FindOrBuildMaterial("G4_AIR");

//    // Define J-PET Plastic (BC-408 equivalent)
//    plasticMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // Optical Properties for Scintillation
//    G4MaterialPropertiesTable *mptPlastic = new G4MaterialPropertiesTable();
//    G4double energy[] = {2.0*eV, 3.5*eV};
//    G4double rindex[] = {1.58, 1.58};
//    
//    mptPlastic->AddProperty("RINDEX", energy, rindex, 2);
//    mptPlastic->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);
//    mptPlastic->AddConstProperty("RESOLUTIONSCALE", 1.0);
//    mptPlastic->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1*ns);
//    
//    plasticMat->SetMaterialPropertiesTable(mptPlastic);
//}

//G4VPhysicalVolume *MyDetectorConstruction::Construct()
//{
//    // 1. Create World
//    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
//    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
//    physWorld = new G4PVPlacement(0, G4ThreeVector(0,0,0), logicWorld, "physWorld", 0, false, 0, true);

//    // 2. Define Scintillator Bar Dimensions (Half-lengths for G4Box)
//    G4double scinX = 6.0/2 * mm;    // Thickness
//    G4double scinY = 24.0/2 * mm;   // Width
//    G4double scinZ = 500.0/2 * mm;  // Length
//    G4double spacing = 3.0 * mm;

//    solidScintillator = new G4Box("solidScintillator", scinX, scinY, scinZ);
//    logicScintillator = new G4LogicalVolume(solidScintillator, plasticMat, "logicScintillator");

//    // Set this as the scoring volume
//    fScoringVolume = logicScintillator;

//    // 3. Place 13 bars in a linear module
//    G4double barPitch = (2 * scinY) + spacing; // Distance center-to-center
//    G4double totalWidth = (13 * 2 * scinY) + (12 * spacing);
//    G4double startY = -totalWidth/2.0 + scinY; // Start at the edge to center the whole group

//    for(G4int i = 0; i < 13; i++)
//    {
//        G4double yPos = startY + (i * barPitch);
//        new G4PVPlacement(0, 
//                          G4ThreeVector(0, yPos, 0), 
//                          logicScintillator, 
//                          "physScintillator", 
//                          logicWorld, 
//                          false, 
//                          i,     // Copy number (important for identifying which bar was hit)
//                          true); 
//    }

//    return physWorld;
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
//    if(logicScintillator)
//        logicScintillator->SetSensitiveDetector(sensDet);
//}

// 1 and 4 Modules
//#include "Construction.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"
//#include "G4Tubs.hh"
//#include "G4Box.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4PVPlacement.hh"
//#include "G4LogicalVolume.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//: fScintLogical(0) 
//{}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//  // --- Materials ---
//  G4NistManager *nist = G4NistManager::Instance();
//  G4Material* Gal_mat = nist->FindOrBuildMaterial("G4_Galactic");
//  G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//  // --- Visualization Attributes ---
////  G4VisAttributes* visAttScin = new G4VisAttributes(G4Colour(0.1, 0.2, 0.8));
//    G4VisAttributes* visAttScin = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
//  visAttScin->SetForceSolid(true);

//  // --- World Volume ---
//  // Using a Tube (G4Tubs) as the world container
//  G4double wRadius = 300*cm;
//  G4double wLength = 530*cm;
//  
//  G4VSolid* wSolid = new G4Tubs("WorldSolid", 0, wRadius, wLength/2, 0, 360*deg);
//  G4LogicalVolume* wLogic = new G4LogicalVolume(wSolid, Gal_mat, "WorldLogic");
//  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "WorldPhys", 0, false, 0, true);

//  // --- Scintillator Bar Geometry ---
//  // Dimensions from your requirements: 500mm x 24mm x 6mm
//  G4double scinHalfX = 25.0*mm /2.0;   // Thickness
//  G4double scinHalfY = 6.0* mm/2.0;  // Width
//  G4double scinHalfZ = 500.0* mm/2.0; // Length

//  G4Box* scinBox = new G4Box("ScintillatorBox", scinHalfX, scinHalfY, scinHalfZ);
//  fScintLogical = new G4LogicalVolume(scinBox, fScinMaterial, "ScintillatorLV");
//  fScintLogical->SetVisAttributes(visAttScin);

//  // --- Module Placement Logic ---
//  G4double gap = 1.0 * mm; // 3mm spacing
//  G4double fullScinY = 2 * scinHalfY; 
//  G4double moduleTotalY = 13 * (fullScinY + gap) - gap; 
//  G4double moduleHalfY = moduleTotalY / 2.0;
//  // Define the 4 module locations (Centers of the 13-bar stacks)
//  std::vector<G4ThreeVector> modulePositions;
//  modulePositions.push_back(G4ThreeVector(0, 0, 0)); // FOR 1 MODULE 
////  modulePositions.push_back(G4ThreeVector(16*cm, 0,  45*cm));
////  modulePositions.push_back(G4ThreeVector(24*cm, 0,  45*cm));
////  modulePositions.push_back(G4ThreeVector(16*cm, 0, -45*cm));
////  modulePositions.push_back(G4ThreeVector(24*cm, 0, -45*cm));

//  // Loop to place 13 bars in each of the 4 locations
//  for (size_t m = 0; m < modulePositions.size(); m++) {
//    G4ThreeVector modCenter = modulePositions[m];
//    
//    for (G4int j = 0; j < 13; j++) {
//      // Calculate vertical offset for each bar
//      G4double localY = -moduleHalfY + scinHalfY + j * (fullScinY + gap);
//      G4ThreeVector scintPos = modCenter + G4ThreeVector(0, localY, 0);
//      
//      // Unique copy number: ModuleIndex * 100 + BarIndex
//      new G4PVPlacement(0, scintPos, fScintLogical, "ScintillatorPhys", wLogic, false, m*100 + j, true);
//    }
//  }

//  return physWorld;
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//  MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
//  if (fScintLogical)
//    fScintLogical->SetSensitiveDetector(scintSD);
//}



//#include "construction.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4LogicalVolume.hh"
//#include "G4PVPlacement.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//: fScintLogical(0)
//{}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//    G4NistManager *nist = G4NistManager::Instance();

//    // --- 1. Materials ---
//    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
//    // This is the specific J-PET plastic material
//    G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // --- 2. World Volume ---
//    // Large enough to hold the 42.5cm radius ring and 50cm AFOV
//    G4double wRadius = 60*cm;
//    G4double wLength = 100*cm;
//    G4VSolid* wSolid = new G4Tubs("WorldSolid", 0, wRadius, wLength/2, 0, 360*deg);
//    G4LogicalVolume* wLogic = new G4LogicalVolume(wSolid, worldMat, "WorldLogic");
//    
//    // Make the world invisible for a cleaner view
//    wLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
//    
//    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "WorldPhys", 0, false, 0, true);

//    // --- 3. Scintillator Bar Geometry ---
//    // Dimensions: 6mm thick x 24mm wide x 500mm long (50cm AFOV)
//    G4double scinX = 6.0 * mm;
//    G4double scinY = 24.0 * mm;
//    G4double scinZ = 500.0 * mm;

//    G4Box* scinBox = new G4Box("ScintillatorBox", scinX/2., scinY/2., scinZ/2.);
//    
//    // IMPORTANT: Using fScinMaterial here (Vinyltoluene)
//    fScintLogical = new G4LogicalVolume(scinBox, fScinMaterial, "ScintillatorLV");

//    // --- 4. Visualization (Orange) ---
//    G4VisAttributes* orange = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
//    orange->SetForceSolid(true);
//    fScintLogical->SetVisAttributes(orange);

//// --- 24-Module Ring Placement ---
//  G4int nModules = 24;
//  G4double radius = 42.5 * cm; 
//  G4double angularSpacing = 360.0 * deg / nModules;
//  G4double barGap = 3.0 * mm;
//  
//  G4double moduleHeight = (13 * scinY) + (12 * barGap);
//  G4double moduleHalfHeight = moduleHeight / 2.0;

//  for (G4int m = 0; m < nModules; m++) {
//      G4double phi = m * angularSpacing;

//      // 1. Create the Rotation Matrix for the module
//      // We use -phi because Geant4 rotations are often defined actively
//      G4RotationMatrix* rotM = new G4RotationMatrix();
//      rotM->rotateZ(phi); 

//      for (G4int j = 0; j < 13; j++) {
//          // 2. Calculate the "unrotated" position of the bar
//          G4double localY = -moduleHalfHeight + scinY/2.0 + j * (scinY + barGap);
//          
//          // 3. Create a position vector: Start at Radius, move by localY
//          // This creates a 'flat' module at the 3 o'clock position
//          G4ThreeVector barPos(radius, localY, 0);
//          
//          // 4. Rotate the entire position vector by phi
//          // This swings the bar into its correct spot in the circle
//          barPos.rotateZ(phi);

//          // 5. Place the bar with the rotation matrix
//          new G4PVPlacement(rotM,          // The bar tilts to face center
//                            barPos,        // The center of the bar in the ring
//                            fScintLogical, 
//                            "ScintillatorPhys", 
//                            wLogic, 
//                            false, 
//                            m * 100 + j, 
//                            true);
//      }
//  }

//    return physWorld;
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    // Attach the Sensitive Detector to record hits
//    MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
//    if (fScintLogical)
//        fScintLogical->SetSensitiveDetector(scintSD);
//}



////24
//#include "construction.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4LogicalVolume.hh"
//#include "G4PVPlacement.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//: fScintLogical(0), fScinLogInModule(0), wLogic(0), physWorld(0)
//{
//    // Default geometry: 24 modules in one ring
//    fGeoKind = Geo24ModulesLayer; 
//}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//    G4NistManager *nist = G4NistManager::Instance();

//    // --- 1. Materials ---
//    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
//    G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // --- 2. World Volume ---
//    G4double wRadius = 60*cm;
//    G4double wLength = 120*cm;
//    G4VSolid* wSolid = new G4Tubs("WorldSolid", 0, wRadius, wLength/2, 0, 360*deg);
//    wLogic = new G4LogicalVolume(wSolid, worldMat, "WorldLogic");
//    wLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
//    
//    physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "WorldPhys", 0, false, 0, true);

//    // --- 3. Run Modular Construction ---
//    ConstructScintillatorsModularLayer();

//    return physWorld;
//}

//void MyDetectorConstruction::ConstructScintillatorsModularLayer()
//{
//    G4NistManager *nist = G4NistManager::Instance();
//    G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // Scintillator Dimensions: 6x24x500 mm (50cm AFOV)
//    G4Box* scinBoxInModule = new G4Box("scinBoxInModule", 3.0*mm, 12.0*mm, 250.0*mm);

//    fScinLogInModule = new G4LogicalVolume(scinBoxInModule, fScinMaterial, "scinBoxInModule");

//    // Set Orange Visualization
//    G4VisAttributes* boxVisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
//    boxVisAtt->SetForceSolid(true);
//    fScinLogInModule->SetVisAttributes(boxVisAtt);

//    // Radii arrays from Sushil's data (cm)
//    const G4double radius_24[13] = {38.416, 38.346, 38.289, 38.244, 38.212, 38.192, 38.186, 38.192, 38.212, 38.244, 38.289, 38.346, 38.416};
//    const G4double radius_8[13]  = {13.4037, 13.2011, 13.0330, 12.9007, 12.8055, 12.7479, 12.7287, 12.7479, 12.8055, 12.9007, 13.0330, 13.2011, 13.4037};
//    const G4double radius_16[13] = {25.8015, 25.69680, 25.61085, 25.54379, 25.49579, 25.4669, 25.45733, 25.4669, 25.49579, 25.54379, 25.61085, 25.6968, 25.8015};

//    std::vector<G4double> radius_dynamic(13, 0.0);
//    G4int icopyI = 201; 

//    // Angular displacements (radians)
//    G4double angDisp_8 = 0.0531204920; 
//    G4double angDisp_16 = 0.0272515011;
//    G4double angDisp_24 = 0.01815;     

//    if (fGeoKind == Geo24ModulesLayer) {
//        for(int i=0; i<13; i++) radius_dynamic[i] = radius_24[i];
//        ConstructLayers(radius_dynamic, 24, angDisp_24, icopyI);
//    } 
//    else if (fGeoKind == Geo24ModulesLayerDistributed) {
//        // Inner Ring (8 modules)
//        for(int i=0; i<13; i++) radius_dynamic[i] = radius_8[i];
//        ConstructLayers(radius_dynamic, 8, angDisp_8, icopyI);
//        
//        // Outer Ring (16 modules)
//        icopyI += 8 * 13;
//        for(int i=0; i<13; i++) radius_dynamic[i] = radius_16[i];
//        ConstructLayers(radius_dynamic, 16, angDisp_16, icopyI);
//    }
//}

//void MyDetectorConstruction::ConstructLayers(std::vector<G4double>& radius_dynamic, G4int numberofModules, G4double angDisp_dynamic, G4int& icopyI)
//{
//    for (int i = 0; i < numberofModules; i++){
//        // phi is the center of the module
//        G4double phi = (i * 2 * M_PI / numberofModules);
//        
//        for (int j = -6; j < 7; j++) {
//            // phi1 is the specific angle for bar j within the module
//            G4double phi1 = phi + j * angDisp_dynamic;
//            G4double radius_new = radius_dynamic[j + 6] * cm;
//            
//            G4RotationMatrix* rot = new G4RotationMatrix();
//            rot->rotateZ(phi); // Rotate module to face center
//            
//            // Calculate X and Y based on the dynamic radius and phi1
//            G4ThreeVector loc = G4ThreeVector(radius_new * cos(phi1), radius_new * sin(phi1), 0.0);
//            
//            G4int copyNo = icopyI + i * 13 + j + 6;
//            G4String name = "scin_" + std::to_string(copyNo);

//            new G4PVPlacement(rot, loc, fScinLogInModule, name, wLogic, false, copyNo, true);
//        }
//    }
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
//    // Attach the SD to fScinLogInModule as it's the one we're using now
//    if (fScinLogInModule)
//        fScinLogInModule->SetSensitiveDetector(scintSD);
//}



////8+16
//#include "construction.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4LogicalVolume.hh"
//#include "G4PVPlacement.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//: fScintLogical(0), fScinLogInModule(0), wLogic(0), physWorld(0)
//{
//    // Set to Distributed for the 8+16 setup
//    fGeoKind = Geo24ModulesLayerDistributed; 
//}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//    G4NistManager *nist = G4NistManager::Instance();

//    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
//    // Ensure this material is defined in your NIST manager
//    G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // World Volume
//    G4double wRadius = 60*cm;
//    G4double wLength = 120*cm;
//    G4VSolid* wSolid = new G4Tubs("WorldSolid", 0, wRadius, wLength/2, 0, 360*deg);
//    wLogic = new G4LogicalVolume(wSolid, worldMat, "WorldLogic");
//    wLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
//    
//    physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "WorldPhys", 0, false, 0, true);

//    // Start the modular construction
//    ConstructScintillatorsModularLayer();

//    return physWorld;
//}

//void MyDetectorConstruction::ConstructScintillatorsModularLayer()
//{
//    G4NistManager *nist = G4NistManager::Instance();
//    G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // Single bar: 6x24x500 mm
//    G4Box* scinBoxInModule = new G4Box("scinBoxInModule", 3.0*mm, 12.0*mm, 250.0*mm);
//    fScinLogInModule = new G4LogicalVolume(scinBoxInModule, fScinMaterial, "scinBoxInModule");

//    // Orange Color
//    G4VisAttributes* boxVisAtt = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
//    boxVisAtt->SetForceSolid(true);
//    fScinLogInModule->SetVisAttributes(boxVisAtt);

//    // Radii data (cm)
//    const G4double radius_8[13]  = {13.4037, 13.2011, 13.0330, 12.9007, 12.8055, 12.7479, 12.7287, 12.7479, 12.8055, 12.9007, 13.0330, 13.2011, 13.4037};
//    const G4double radius_16[13] = {25.8015, 25.69680, 25.61085, 25.54379, 25.49579, 25.4669, 25.45733, 25.4669, 25.49579, 25.54379, 25.61085, 25.6968, 25.8015};
//    const G4double radius_24[13] = {38.416, 38.346, 38.289, 38.244, 38.212, 38.192, 38.186, 38.192, 38.212, 38.244, 38.289, 38.346, 38.416};

//    std::vector<G4double> radius_dynamic(13, 0.0);
//    G4int icopyI = 201; 

//    // Angular displacements (radians)
//    G4double angDisp_8 = 0.0531204920; 
//    G4double angDisp_16 = 0.0272515011;
//    G4double angDisp_24 = 0.01815;

//    if (fGeoKind == Geo24ModulesLayer) {
//        // Single 24-module ring
//        for(int i=0; i<13; i++) radius_dynamic[i] = radius_24[i];
//        ConstructLayers(radius_dynamic, 24, angDisp_24, icopyI);
//    } 
//    else if (fGeoKind == Geo24ModulesLayerDistributed) {
//        // --- 8-MODULE INNER RING ---
//        for(int i=0; i<13; i++) radius_dynamic[i] = radius_8[i];
//        ConstructLayers(radius_dynamic, 8, angDisp_8, icopyI);
//        
//        // --- 16-MODULE OUTER RING ---
//        // Shift ID: 8 modules * 13 bars = 104 bars already placed
//        icopyI += 8 * 13; 
//        for(int i=0; i<13; i++) radius_dynamic[i] = radius_16[i];
//        ConstructLayers(radius_dynamic, 16, angDisp_16, icopyI);
//    }
//}

//void MyDetectorConstruction::ConstructLayers(std::vector<G4double>& radius_dynamic, G4int numberofModules, G4double angDisp_dynamic, G4int& icopyI)
//{
//    for (int i = 0; i < numberofModules; i++){
//        G4double phi = (i * 2 * M_PI / numberofModules);
//        
//        for (int j = -6; j < 7; j++) {
//            G4double phi1 = phi + j * angDisp_dynamic;
//            G4double radius_new = radius_dynamic[j + 6] * cm;
//            
//            G4RotationMatrix* rot = new G4RotationMatrix();
//            rot->rotateZ(phi); 
//            
//            G4ThreeVector loc = G4ThreeVector(radius_new * cos(phi1), radius_new * sin(phi1), 0.0);
//            
//            G4int copyNo = icopyI + i * 13 + j + 6;
//            G4String name = "scin_" + std::to_string(copyNo);

//            new G4PVPlacement(rot, loc, fScinLogInModule, name, wLogic, false, copyNo, true);
//        }
//    }
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
//    if (fScinLogInModule)
//        fScinLogInModule->SetSensitiveDetector(scintSD);
//}




////3+3

//#include "construction.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4LogicalVolume.hh"
//#include "G4PVPlacement.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//: fScintLogical(0), fScinLogInModule(0), fScinLogInModuleInner(0), wLogic(0), physWorld(0)
//{
//    // Set to 3-Quadrant Stacked for your current research
//    fGeoKind = Geo3QuadrantStacked; 
//}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//    G4NistManager *nist = G4NistManager::Instance();
//    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");

//    // World Volume
//    G4double wRadius = 60*cm;
//    G4double wLength = 120*cm;
//    G4VSolid* wSolid = new G4Tubs("WorldSolid", 0, wRadius, wLength/2, 0, 360*deg);
//    wLogic = new G4LogicalVolume(wSolid, worldMat, "WorldLogic");
//    wLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
//    
//    physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "WorldPhys", 0, false, 0, true);

//    ConstructScintillatorsModularLayer();

//    return physWorld;
//}

//void MyDetectorConstruction::ConstructScintillatorsModularLayer()
//{
//    G4NistManager *nist = G4NistManager::Instance();
//    G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // Scintillator Dimensions: 6x24x500 mm
//    G4Box* scinBoxInModule = new G4Box("scinBoxInModule", 3.0*mm, 12.0*mm, 250.0*mm);

//    // --- Visualization Setup ---
//    fScinLogInModule = new G4LogicalVolume(scinBoxInModule, fScinMaterial, "scinBoxOuter");
//    G4VisAttributes* orangeVis = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
//    orangeVis->SetForceSolid(true);
//    fScinLogInModule->SetVisAttributes(orangeVis);

//    fScinLogInModuleInner = new G4LogicalVolume(scinBoxInModule, fScinMaterial, "scinBoxInner");
//    G4VisAttributes* redVis = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
//    redVis->SetForceSolid(true);
//    fScinLogInModuleInner->SetVisAttributes(redVis);

//    G4int icopyI = 0;

//    // --- Geometry Selection Logic ---
//    if (fGeoKind == Geo24ModulesLayer) {
//        ConstructLayers(fScinLogInModule, 38.186*cm, 24, 0.0, icopyI);
//    } 
//    else if (fGeoKind == Geo24ModulesLayerDistributed) {
//        ConstructLayers(fScinLogInModuleInner, 12.728*cm, 8, 0.0, icopyI);
//        icopyI += 8 * 13;
//        ConstructLayers(fScinLogInModule, 25.457*cm, 16, 0.0, icopyI);
//    }
//    else if (fGeoKind == Geo3QuadrantStacked) {
//        G4double angles[3] = {0*deg, 120*deg, 240*deg};
//        icopyI = 600;

//        for (int i = 0; i < 3; i++) {
//            // Inner Module (Red) placed at 20cm center
//            ConstructLayers(fScinLogInModuleInner, 20.0*cm, 1, angles[i], icopyI); 
//            // Outer Module (Orange) placed at 25cm center (5cm gap)
//            ConstructLayers(fScinLogInModule, 25.0*cm, 1, angles[i], icopyI);
//        }
//    }
//}

//void MyDetectorConstruction::ConstructLayers(G4LogicalVolume* logVol, G4double centerRadius, G4int numberofModules, G4double manualPhi, G4int& icopyI)
//{
// // 24mm bar width + 3mm gap = 27mm center-to-center distance
//    G4double barStep = 27.0 * mm; 

//    for (int i = 0; i < numberofModules; i++){
//        // Use manualPhi for the 3-quadrant setup (where numberofModules is 1)
//        G4double phi = (numberofModules == 1) ? manualPhi : (i * 2 * M_PI / numberofModules);
//        
//        // 1. Create rotation for the bars so they face the center (0,0,0)
//        G4RotationMatrix* rot = new G4RotationMatrix();
//        rot->rotateZ(phi); 

//        for (int j = -6; j < 7; j++) {
//            // 2. Define the LOCAL position of each bar relative to the module center
//            // centerRadius is the distance from world (0,0,0) to the module center
//            // j * barStep is the lateral shift (up/down) along the module face
//            G4ThreeVector loc(centerRadius, j * barStep, 0.0);
//            
//            // 3. TRANSFORM local to global: Rotate the position vector by phi
//            // This ensures the middle bar (j=0) stays exactly on the axis line
//            loc.rotateZ(phi);
//            
//            G4int copyNo = icopyI + j + 6;
//            G4String name = "scin_" + std::to_string(copyNo);

//            // 4. Place the physical volume
//            new G4PVPlacement(rot, loc, logVol, name, wLogic, false, copyNo, true);
//        }
//        icopyI += 13;
//    }
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
//    if (fScinLogInModule) fScinLogInModule->SetSensitiveDetector(scintSD);
//    if (fScinLogInModuleInner) fScinLogInModuleInner->SetSensitiveDetector(scintSD);
//}


// DEEP

//#include "Construction.hh"
//#include "detector.hh"
//#include "G4SDManager.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"
//#include "G4Tubs.hh"
//#include "G4Box.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4PVPlacement.hh"
//#include "G4LogicalVolume.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//    : fScintLogical(nullptr)
//{}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//    // --- Materials ---
//    G4NistManager* nist = G4NistManager::Instance();
//    G4Material* galactic   = nist->FindOrBuildMaterial("G4_Galactic");
//    G4Material* scintMat   = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

//    // --- Visualization ---
//    G4VisAttributes* visScint = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
//    visScint->SetForceSolid(true);

//    // --- World volume (cylindrical, large enough to contain the module) ---
//    G4double worldR = 300. * cm;
//    G4double worldL = 530. * cm;
//    G4VSolid*        worldSolid = new G4Tubs("WorldSolid", 0, worldR, worldL/2., 0, 360.*deg);
//    G4LogicalVolume* worldLogic = new G4LogicalVolume(worldSolid, galactic, "WorldLogic");
//    G4VPhysicalVolume* physWorld = new G4PVPlacement(
//        nullptr, G4ThreeVector(), worldLogic, "WorldPhys", nullptr, false, 0, true);

//    // Na-22 SOURCE — small yellow disc
//G4Tubs* sourceSolid =
//    new G4Tubs("Source",
//               0.,        // inner radius
//               2.*mm,     // outer radius = 2mm
//               0.1*mm,    // half thickness
//               0.,        // start angle
//               360.*deg); // full circle

//G4LogicalVolume* sourceLV =
//    new G4LogicalVolume(sourceSolid,
//                        worldMat,  // or Na material
//                        "Source");

//// Yellow colour
//G4VisAttributes* sourceVis =
//    new G4VisAttributes(G4Colour(1., 1., 0.));
//sourceVis->SetForceSolid(true);
//sourceLV->SetVisAttributes(sourceVis);

//new G4PVPlacement(0,
//    G4ThreeVector(0., 0., -30.*cm),  // fSourceZ
//    sourceLV,
//    "Source",
//    worldLV,
//    false, 0);
//    
//    // SiO2 CONVERTER — thin blue rectangle
//G4Box* converterSolid =
//    new G4Box("Converter",
//              5.*mm,    // half x
//              5.*mm,    // half y
//              1.*mm);   // half z = 2mm thick

//G4LogicalVolume* converterLV =
//    new G4LogicalVolume(converterSolid,
//                        worldMat,  // or SiO2 material
//                        "Converter");

//// Blue colour
//G4VisAttributes* convVis =
//    new G4VisAttributes(G4Colour(0., 0., 1.));
//convVis->SetForceSolid(true);
//converterLV->SetVisAttributes(convVis);

//new G4PVPlacement(0,
//    G4ThreeVector(0., 0., -28.*cm),  // fConverterZ
//    converterLV,
//    "Converter",
//    worldLV,
//    false, 0);

//    // --- Scintillator bar geometry ---
//    // J-PET single strip: 500 mm long, 24 mm wide, 6 mm thick
//    // Half-lengths for G4Box constructor
//    G4double hX = 24. * mm / 2.;   // thickness half
//    G4double hY = 6.*mm / 2.;   // width half
//    G4double hZ = 500.0 * mm / 2.;  // length half (along beam axis Z)

//    G4Box*           scintBox   = new G4Box("ScintillatorBox", hX, hY, hZ);
//    fScintLogical = new G4LogicalVolume(scintBox, scintMat, "ScintillatorLV");
//    fScintLogical->SetVisAttributes(visScint);

//    // --- Module placement: 1 module of 13 bars stacked in Y ---
//    G4double gap          = 1.0 * mm;
//    G4double barFullY     = 2. * hY;
//    G4double moduleTotalY = 13. * (barFullY + gap) - gap;
//    G4double moduleHalfY  = moduleTotalY / 2.;

//    // Single module center — extend modulePositions vector to add more modules later
//    std::vector<G4ThreeVector> modulePositions;
//    modulePositions.emplace_back(G4ThreeVector(0., 0., 0.));
//    // Uncomment to add more modules:
//    // modulePositions.emplace_back(G4ThreeVector(16.*cm, 0.,  45.*cm));
//    // modulePositions.emplace_back(G4ThreeVector(24.*cm, 0.,  45.*cm));

//    for(size_t m = 0; m < modulePositions.size(); m++) {
//        G4ThreeVector modCenter = modulePositions[m];
//        for(G4int j = 0; j < 13; j++) {
//            // Y position of bar j within the module
//            G4double localY = -moduleHalfY + hY + j * (barFullY + gap);
//            G4ThreeVector barPos = modCenter + G4ThreeVector(0., localY, 0.);

//            // Copy number encodes module and bar: module*100 + bar (e.g. bar 5 of module 0 = 5)
//            G4int copyNo = static_cast<G4int>(m) * 100 + j;

//            new G4PVPlacement(
//                nullptr,          // no rotation
//                barPos,
//                fScintLogical,
//                "ScintillatorPhys",
//                worldLogic,
//                false,
//                copyNo,
//                true);            // check overlaps
//        }
//    }

//    return physWorld;
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    // Create the sensitive detector
//    MySensitiveDetector* scintSD = new MySensitiveDetector("ScintillatorSD");

//    // CRITICAL: register with G4SDManager so GEANT4 routes hits to it.
//    // Without this line ProcessHits() is never called even if the SD is
//    // attached to the logical volume below.
//    G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);

//    // Attach SD to every copy of ScintillatorLV
//    if(fScintLogical)
//        fScintLogical->SetSensitiveDetector(scintSD);
//}


//SEEK

//#include "Construction.hh"
//#include "G4SDManager.hh"
//#include "G4RotationMatrix.hh"
//#include "G4VisAttributes.hh"
//#include "G4Tubs.hh"
//#include "G4Box.hh"
//#include "G4SystemOfUnits.hh"
//#include "G4NistManager.hh"
//#include "G4PVPlacement.hh"
//#include "G4LogicalVolume.hh"
//#include "G4UserLimits.hh"
//#include "detector.hh"
//#include "G4PVReplica.hh"

//MyDetectorConstruction::MyDetectorConstruction()
//    : fScintLogical(nullptr)
//{}

//MyDetectorConstruction::~MyDetectorConstruction()
//{}

//G4VPhysicalVolume* MyDetectorConstruction::Construct()
//{
//    // --- Materials ---
//    G4NistManager* nist = G4NistManager::Instance();
//    G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic");
//    G4Material* scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
//    G4Material* siMat    = nist->FindOrBuildMaterial("G4_Si");

//    // --- World Volume ---
//    G4double worldR = 300. * cm;
//    G4double worldL = 530. * cm;
//    G4VSolid* worldSolid = new G4Tubs("WorldSolid", 0, worldR, worldL/2., 0, 360.*deg);
//    G4LogicalVolume* worldLogic = new G4LogicalVolume(worldSolid, galactic, "WorldLogic");
//    G4VPhysicalVolume* physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), worldLogic, "WorldPhys", nullptr, false, 0, true);

//    // --- 1. J-PET Modules (Blue Scintillators) ---
//    G4double hX = 24.*mm/2.; G4double hY = 6.*mm/2.; G4double hZ = 500.*mm/2.;
//    G4Box* scintBox = new G4Box("ScintillatorBox", hX, hY, hZ);
//    fScintLogical = new G4LogicalVolume(scintBox, scintMat, "ScintillatorLV");

//    G4VisAttributes* visScint = new G4VisAttributes(G4Colour(0.1, 0.2, 0.8)); // Blue
//    visScint->SetForceSolid(true);
//    fScintLogical->SetVisAttributes(visScint);

//    G4double gap = 1.0*mm;
//    G4double barFullY = 2.*hY;
//    G4double moduleHalfY = (13.*(barFullY + gap) - gap) / 2.;

//    // Coordinates matching your colleague's multi-module setup
//    std::vector<G4ThreeVector> modulePositions = {
//        G4ThreeVector(20.8*cm, 0,  30*cm), G4ThreeVector(30.8*cm, 0,  30*cm),
//        G4ThreeVector(20.8*cm, 0, -30*cm), G4ThreeVector(30.8*cm, 0, -30*cm)
//    };

//    for(size_t m = 0; m < modulePositions.size(); m++) {
//        for(G4int j = 0; j < 13; j++) {
//            G4double localY = -moduleHalfY + hY + j*(barFullY + gap);
//            G4ThreeVector barPos = modulePositions[m] + G4ThreeVector(0., localY, 0.);
//            new G4PVPlacement(nullptr, barPos, fScintLogical, "ScintillatorPhys", worldLogic, false, m*100 + j, true);
//        }
//    }

//    // --- 2. Moiré Gratings (Orange Squares) ---
//G4double grating_halfX = 7.0 * cm / 2.0;
//G4double grating_halfY = 7.0 * cm / 2.0;
//G4double grating_halfZ = 250.0 * micrometer / 2.0;

//G4double pitch = 100.0 * micrometer;
//G4double opening_width = 40.0 * micrometer;
//G4double wall_width = pitch - opening_width;

//// Mother Volume: Use the 'galactic' material you defined above
//G4Box* gratingMotherBox = new G4Box("GratingMotherBox", grating_halfX, grating_halfY, grating_halfZ);
//G4LogicalVolume* gratingMotherLog = new G4LogicalVolume(gratingMotherBox, galactic, "GratingMotherLog");

//// The Slice: Use 'galactic'
//G4Box* slice_box = new G4Box("SliceBox", grating_halfX, pitch / 2.0, grating_halfZ);
//G4LogicalVolume* slice_log = new G4LogicalVolume(slice_box, galactic, "SliceLog");

//G4int num_replicas = G4int((2.0 * grating_halfY) / pitch);
//new G4PVReplica("GratingReplica", slice_log, gratingMotherLog, kYAxis, num_replicas, pitch);

//// The Silicon Wall: Use 'siMat'
//G4Box* wall_box = new G4Box("WallBox", grating_halfX, wall_width / 2.0, grating_halfZ);
//fGratingWallLogical = new G4LogicalVolume(wall_box, siMat, "WallLog");

//// The Opening: Use 'galactic'
//G4Box* opening_box = new G4Box("OpeningBox", grating_halfX, opening_width / 2.0, grating_halfZ);
//fGratingOpeningLogical = new G4LogicalVolume(opening_box, galactic, "OpeningLog");

//// Position wall and opening
//new G4PVPlacement(nullptr, G4ThreeVector(0, (pitch/2.0 - wall_width/2.0), 0), fGratingWallLogical, "Wall", slice_log, false, 0, true);
//new G4PVPlacement(nullptr, G4ThreeVector(0, (-pitch/2.0 + opening_width/2.0), 0), fGratingOpeningLogical, "Opening", slice_log, false, 0, true);

//// ... (Aesthetics and Placement remain the same) ...

//// --- 3. Solid Counter (Red Square) ---
//G4double counterXY = 7.0*cm/2.0;
//G4double counterZ  = 0.1*mm/2.0;
//G4Box* counterBox = new G4Box("CounterBox", counterXY, counterXY, counterZ);
//// Use 'siMat' here as well
//fSolidCounterLogical = new G4LogicalVolume(counterBox, siMat, "CounterLog");
//fSolidCounterLogical->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
//new G4PVPlacement(nullptr, G4ThreeVector(0.1*cm, 0, 50.6*cm), fSolidCounterLogical, "SolidCounter", worldLogic, false, 0, true);
//    // --- User Limits ---
//    G4UserLimits* stepLimits = new G4UserLimits(4.0*micrometer);
//    fGratingWallLogical->SetUserLimits(stepLimits);
//    fGratingOpeningLogical->SetUserLimits(stepLimits);
//    fSolidCounterLogical->SetUserLimits(stepLimits);

//    return physWorld;;
//}

//void MyDetectorConstruction::ConstructSDandField()
//{
//    MySensitiveDetector* masterSD = new MySensitiveDetector("MasterSD");
//    G4SDManager::GetSDMpointer()->AddNewDetector(masterSD);

//    // Blue Scintillators
//    if(fScintLogical) fScintLogical->SetSensitiveDetector(masterSD);

//    // Orange Grating Bars (The Silicon parts)
//    if(fGratingWallLogical) fGratingWallLogical->SetSensitiveDetector(masterSD);

//    // Grating Slits (Optional: if you want to record passage through slits)
//    if(fGratingOpeningLogical) fGratingOpeningLogical->SetSensitiveDetector(masterSD);

//    // Red Final Counter (Crucial for Moiré pattern analysis)
//    if(fSolidCounterLogical) fSolidCounterLogical->SetSensitiveDetector(masterSD);
//}


#include "Construction.hh"
#include "detector.hh"
#include "G4SDManager.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include <cmath>
#include <string>

MyDetectorConstruction::MyDetectorConstruction()
    : fScintLogical(nullptr)
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    // ---------------------------------------------------------------
    // Materials
    // ---------------------------------------------------------------
    G4NistManager* nist     = G4NistManager::Instance();
    G4Material*    galactic = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material*    scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    // ---------------------------------------------------------------
    // World volume — cylinder large enough for full modular J-PET
    // Modular layer radius ~38.4 cm + 500 mm bar half-length along Z
    // ---------------------------------------------------------------
    G4double worldR     = 150. * cm;
    G4double worldHalfL = 60.  * cm;
    G4VSolid* worldSolid = new G4Tubs(
        "WorldSolid", 0., worldR, worldHalfL, 0., 360.*deg);
    G4LogicalVolume* worldLogic = new G4LogicalVolume(
        worldSolid, galactic, "WorldLogic");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(
        nullptr, G4ThreeVector(), worldLogic, "WorldPhys",
        nullptr, false, 0, true);

    // ---------------------------------------------------------------
    // Scintillator bar — J-PET strip: 500 mm long, 24 mm wide, 6 mm thick
    // Bars are placed with their LENGTH along Z axis
    // ---------------------------------------------------------------
    G4double hX = 24.  * mm / 2.;   // half-width  (radial direction)
    G4double hY =  6.  * mm / 2.;   // half-depth  (tangential direction)
    G4double hZ = 500. * mm / 2.;   // half-length (along Z beam axis)

    G4Box* scintBox = new G4Box("ScintillatorBox", hX, hY, hZ);
    fScintLogical   = new G4LogicalVolume(scintBox, scintMat, "ScintillatorLV");

    G4VisAttributes* visScint = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
    visScint->SetForceSolid(true);
    fScintLogical->SetVisAttributes(visScint);

    // ---------------------------------------------------------------
    // Modular J-PET: 24 modules arranged in a full ring
    // Each module has 13 bars
    // Radii taken from official J-PET code (DetectorConstants)
    // radius0 = 38.186 cm, angular displacement = 1.04 deg = 0.01815 rad
    // ---------------------------------------------------------------
    const G4double radius_24[13] = {
        38.416, 38.346, 38.289, 38.244, 38.212,
        38.192, 38.186, 38.192, 38.212, 38.244,
        38.289, 38.346, 38.416
    };                                      // all in cm

    const G4double angDisp = 0.01815;       // 1.04 deg in radians
    const G4int    nModules = 24;
    G4int          copyNo   = 201;          // matches official J-PET numbering

    for(G4int i = 0; i < nModules; i++)
    {
        // Azimuthal angle of this module
        G4double phi = i * 2. * M_PI / nModules;

        for(G4int j = -6; j < 7; j++)      // j = -6 .. +6 → 13 bars
        {
            G4double phi1   = phi + j * angDisp;
            G4double radius = radius_24[j + 6] * cm;

            // Bar centre position
            G4ThreeVector loc(
                radius * std::cos(phi1),
                radius * std::sin(phi1),
                0.0);

            // Rotate bar so its face points toward the ring centre
            G4RotationMatrix rot;
            rot.rotateZ(phi);

            G4Transform3D transform(rot, loc);

            new G4PVPlacement(
                transform,
                fScintLogical,
                "ScintillatorPhys_" + std::to_string(copyNo),
                worldLogic,
                true,
                copyNo,
                true);      // check overlaps

            copyNo++;
        }
    }

    // ---------------------------------------------------------------
    // Total bars placed: 24 modules × 13 bars = 312 bars
    // Copy numbers: 201 – 512
    // ---------------------------------------------------------------

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector* scintSD = new MySensitiveDetector("ScintillatorSD");

    // Register with G4SDManager — without this ProcessHits() is never called
    G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);

    // Attach SD to every copy of ScintillatorLV
    if(fScintLogical)
        fScintLogical->SetSensitiveDetector(scintSD);
}
