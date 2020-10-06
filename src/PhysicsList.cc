#include "PhysicsList.hh"

#include "G4EmLowEPPhysics.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ProductionCutsTable.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VUserPhysicsList() {
  physics_constructor = new G4EmLowEPPhysics();
  SetVerboseLevel(2);
}

PhysicsList::~PhysicsList() { delete physics_constructor; }

void PhysicsList::ConstructParticle() {
  physics_constructor->ConstructParticle();
}

void PhysicsList::ConstructProcess() {
  AddTransportation();
  physics_constructor->ConstructProcess();
}

void PhysicsList::SetCuts() {
  if (verboseLevel > 1) {
    G4cout << "PhysicsList::SetCuts:" << G4endl;
  }

  //  const G4double cutForGamma = 10 * nm;
  //  const G4double cutForElectron = 10 * nm;
  //
  //  SetCutValue(cutForGamma, "gamma");
  //  SetCutValue(cutForElectron, "e-");
  //  SetCutValue(cutForElectron, "e+");
  //
  //  G4double lowLimit = 400 * eV;
  //  G4double highLimit = 100. * MeV;
  //
  //  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowLimit,
  //                                                                  highLimit);
  if (verboseLevel > 1) {
    DumpCutValuesTable();
  }
}
