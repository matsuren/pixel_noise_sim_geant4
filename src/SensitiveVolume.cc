//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SensitiveVolume.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "SensitiveVolume.hh"
#include "Analysis.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"

//------------------------------------------------------------------------------
SensitiveVolume::SensitiveVolume(G4String name) : G4VSensitiveDetector(name) {}
//------------------------------------------------------------------------------
SensitiveVolume::~SensitiveVolume() {}
//------------------------------------------------------------------------------
void SensitiveVolume::Initialize(G4HCofThisEvent * /* HCE */) {

  // get event id
  auto event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
  event_id = event->GetEventID();
  if (event_id % 100000 == 0) {
    G4cout << "== Initialize. Event id:" << event_id << G4endl;
  }
}
//------------------------------------------------------------------------------
void SensitiveVolume::EndOfEvent(G4HCofThisEvent *) {
  //  // save
  //  if (sum_eDep / eV > 1.0 * eV) {
  //    // save results
  //    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  //#ifdef ANALYSIS_HIST
  //    analysisManager->FillH1(0, sum_eDep);
  //    analysisManager->FillH1(1, sum_stepLength);
  //#else
  //    analysisManager->FillNtupleDColumn(0, sum_eDep);
  //    analysisManager->FillNtupleDColumn(1, sum_stepLength);
  //    analysisManager->AddNtupleRow();
  //#endif
  //  }
}

//------------------------------------------------------------------------------
G4bool SensitiveVolume::ProcessHits(G4Step *aStep, G4TouchableHistory *) {

  G4StepPoint *pre = aStep->GetPreStepPoint();
  // G4int copyNo = pre->GetPhysicalVolume()->GetCopyNo();

  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.0)
    return true;

  // get name
  auto physvol_name = pre->GetTouchable()->GetVolume()->GetName();
  auto particle_name = aStep->GetTrack()->GetDefinition()->GetParticleName();
  if (physvol_name == "PhysVol_pixel" && edep != 0.0) {
    G4int loc_x = pre->GetTouchableHandle()->GetReplicaNumber(0);
    G4int loc_y = pre->GetTouchableHandle()->GetReplicaNumber(1);
    if (verboseLevel > 0) {
      G4cout << "# Event id:" << event_id << ", particle:" << particle_name
             << ", Pixel hit location: (x, y)=(" << loc_x << ", " << loc_y
             << "), Energy: " << edep / keV << " keV " << G4endl;
    }

    // save result
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(0, event_id);
    analysisManager->FillNtupleDColumn(1, loc_x);
    analysisManager->FillNtupleDColumn(2, loc_y);
    analysisManager->FillNtupleDColumn(3, edep / keV);
    analysisManager->AddNtupleRow();

  } else {
    if (verboseLevel > 0) {
      G4cout << " ## --- ProcessHits --- ## " << edep / keV << " keV" << G4endl;
    }
  }

  return true;
}
