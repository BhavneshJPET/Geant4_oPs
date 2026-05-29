//#include <iostream>

//#include "G4RunManager.hh"
//#include "G4MTRunManager.hh"
//#include "G4UImanager.hh"
//#include "G4VisManager.hh"
//#include "G4VisExecutive.hh"
//#include "G4UIExecutive.hh"
//#include "QGSP_BERT.hh"

//#include "Construction.hh"
//#include "physics.hh"
//#include "action.hh"

//int main(int argc, char** argv)
//{
//    G4UIExecutive* ui = 0;

//    #ifdef G4MULTITHREADED
//      G4MTRunManager* runManager = new G4MTRunManager;
//    #else
//      G4RunManager* runManager = new G4RunManager;
//    #endif

//    runManager->SetUserInitialization(new MyDetectorConstruction());
//    runManager->SetUserInitialization(new MyPhysicsList());
//    runManager->SetUserInitialization(new MyActionInitialization());

//     G4VModularPhysicsList* physics = new QGSP_BERT();
//     physics->RegisterPhysics(new G4DecayPhysics());
//     runManager->SetUserInitialization(physics);

//    if (argc == 1)
//    {
//        ui = new G4UIExecutive(argc, argv);
//    }

//    G4VisManager *visManager = new G4VisExecutive();
//    visManager->Initialize();

//    G4UImanager *UImanager = G4UImanager::GetUIpointer();

//    if(ui)
//    {
//        UImanager->ApplyCommand("/control/execute vis.mac");
//        ui->SessionStart();
//    }
//    else
//    {
//        G4String command = "/control/execute ";
//        G4String fileName = argv[1];
//        UImanager->ApplyCommand(command+fileName);
//    }

//    return 0;
//}


// DEEP

//#include <iostream>
//#include "G4RunManager.hh"
//#include "G4UImanager.hh"
//#include "G4VisManager.hh"
//#include "G4VisExecutive.hh"
//#include "G4UIExecutive.hh"
//#include "Construction.hh"
//#include "physics.hh"
//#include "action.hh"

//int main(int argc, char** argv)
//{
//    // Create UI executive first (needed to detect interactive vs batch mode)
//    G4UIExecutive* ui = nullptr;
//    if(argc == 1)
//        ui = new G4UIExecutive(argc, argv);

//    // Use single-threaded RunManager for custom particle (o-Ps) stability.
//    // Switch to G4MTRunManager only after confirming o-Ps decays correctly.
//    G4RunManager* runManager = new G4RunManager;

//    // --- Mandatory initializations (order matters!) ---
//    // 1. Geometry
//    runManager->SetUserInitialization(new MyDetectorConstruction());

//    // 2. Physics — MyPhysicsList defines o-Ps particle + 3-gamma decay.
//    //    Do NOT add a second physics list (e.g. QGSP_BERT) here — it would
//    //    overwrite this one and o-Ps would never be found by the generator.
//    runManager->SetUserInitialization(new MyPhysicsList());

//    // 3. User actions (generator, event, stepping, run)
//    runManager->SetUserInitialization(new MyActionInitialization());

//    // --- Visualization ---
//    G4VisManager* visManager = new G4VisExecutive();
//    visManager->Initialize();

//    G4UImanager* UImanager = G4UImanager::GetUIpointer();

//    if(ui) {
//        // Interactive mode
//        UImanager->ApplyCommand("/control/execute vis.mac");
//        ui->SessionStart();
//        delete ui;
//    } else {
//        // Batch mode: pass macro file as argument, e.g. ./sim run.mac
//        G4String command  = "/control/execute ";
//        G4String fileName = argv[1];
//        UImanager->ApplyCommand(command + fileName);
//    }

//    delete visManager;
//    delete runManager;
//    return 0;
//}


//NA22

#include <iostream>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh"
#include "Construction.hh"
#include "physics.hh"
#include "action.hh"
#include "generator.hh"

int main(int argc, char** argv)
{
    G4UIExecutive* ui = nullptr;
    if(argc == 1)
        ui = new G4UIExecutive(argc, argv);

    G4RunManager* runManager = new G4RunManager;
    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new MyPhysicsList());

    MyActionInitialization* actionInit = new MyActionInitialization();
    runManager->SetUserInitialization(actionInit);
    runManager->Initialize();

    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    MyPrimaryGenerator* generator = actionInit->GetGenerator();
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // --------------------------------------------------
    // SCAN MODE: ./sim scan
    // --------------------------------------------------
    if (!ui && argc > 1 && G4String(argv[1]) == "scan")
    {
        // ❌ REMOVED: generator->SetBeamMode(BeamMode::BEAM);
        // BEAM is now the only mode — no need to set it

        std::vector<G4double> scanEnergies = {
            50  * eV,    // mean path ~6mm  — decays near entry
            75  * eV,    // mean path ~10mm — mid bar
            100 * eV,    // mean path ~13mm — spreads full bar
            150 * eV,    // mean path ~20mm — some exit
            200 * eV     // mean path ~26mm — exits bar
        };

        for (auto E : scanEnergies)
        {
            generator->SetBeamEnergy(E);
            G4cout << "\n=== Scanning beam energy: "
                   << E/eV << " eV ===" << G4endl;
            runManager->BeamOn(100000);
        }

        delete visManager;
        delete runManager;
        return 0;
    }

    // --------------------------------------------------
    // NORMAL MODE: ./sim  or  ./sim run.mac
    // --------------------------------------------------
    if(ui) {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
        delete ui;
    } else {
        G4String command  = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    delete visManager;
    delete runManager;
    return 0;
}
