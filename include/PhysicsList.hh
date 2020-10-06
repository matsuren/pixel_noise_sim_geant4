#include "G4VUserPhysicsList.hh"
#include "globals.hh"
#include <G4VPhysicsConstructor.hh>

class PhysicsList : public G4VUserPhysicsList {
public:
  PhysicsList();
  ~PhysicsList();

protected:
  // Construct particle and physics process
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();

protected:
  G4VPhysicsConstructor *physics_constructor;
};
