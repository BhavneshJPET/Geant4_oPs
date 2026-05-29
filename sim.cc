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
