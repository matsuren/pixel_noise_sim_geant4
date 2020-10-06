//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SensitiveVolume.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

#include "G4VSensitiveDetector.hh"
class G4Step;

//------------------------------------------------------------------------------
class SensitiveVolume : public G4VSensitiveDetector
//------------------------------------------------------------------------------
{
public:
  SensitiveVolume(G4String);
  ~SensitiveVolume();

  void Initialize(G4HCofThisEvent *);
  G4bool ProcessHits(G4Step *, G4TouchableHistory *);
  void EndOfEvent(G4HCofThisEvent *);

private:
  G4int event_id;
};
