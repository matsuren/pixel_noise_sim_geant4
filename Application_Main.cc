//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "UserActionInitialization.hh"

#include <chrono>

#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
//#include "Shielding.hh"
//#include "FTFP_BERT.hh"
#include "PhysicsList.hh"

//-------------------------------------------------------------------------------
int main(int argc, char **argv)
//-------------------------------------------------------------------------------
{
  // Set random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  G4RunManager *runManager = new G4RunManager;

  // Set up mandatory user initialization: Geometry
  runManager->SetUserInitialization(new Geometry);

  // Set up mandatory user initialization: Physics-List
  runManager->SetUserInitialization(new PhysicsList);
  //  runManager->SetUserInitialization(new Shielding);
  //  runManager->SetUserInitialization(new FTFP_BERT);

  // Set up user initialization: User Actions
  runManager->SetUserInitialization(new UserActionInitialization);

  // Initialize G4 kernel
  runManager->Initialize();

  // Create visualization environment
  auto visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get UI (User Interface) manager
  G4UImanager *uiManager = G4UImanager::GetUIpointer();

  // Switch batch or interactive mode
  if (argc == 1) { // Interactive mode
    auto uiExec = new G4UIExecutive(argc, argv);
    uiManager->ApplyCommand("/control/execute init_setup.mac");
    // Enable analysis
    uiManager->ApplyCommand("/analysis/setActivation 0");
    uiManager->ApplyCommand("/analysis/setFileName detect_result");

    // Start GUI
    uiExec->SessionStart();
    delete uiExec;
  } else { // Batch mode
    auto start = std::chrono::steady_clock::now();
    G4String beam_cnt_str = argv[1];
    std::string fname = "detect_result_256_" + beam_cnt_str + "_co";
    uiManager->ApplyCommand("/control/execute primary_generator_gps_co.mac");
    //    std::string fname = "detect_result_256_" + beam_cnt_str + "_cs";
    //    uiManager->ApplyCommand("/control/execute
    //    primary_generator_gps_cs.mac");
    uiManager->ApplyCommand("/analysis/setFileName " + fname);
    uiManager->ApplyCommand("/control/execute init_setup.mac");
    uiManager->ApplyCommand("/analysis/setActivation 0");

    // verbose
    uiManager->ApplyCommand("/tracking/verbose 0");
    uiManager->ApplyCommand("/analysis/verbose 0");

    uiManager->ApplyCommand("/run/beamOn " + beam_cnt_str);
    auto end = std::chrono::steady_clock::now();
    G4cout
        << "// *******************************************//\n"
        << "// Output:" << fname << ".csv, Time elapsed:"
        << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
        << "[s] \n// *******************************************//" << G4endl;
  }

  // Job termination
  delete visManager;
  delete runManager;

  return 0;
}