//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "GeometryMessenger.hh"
#include "SensitiveVolume.hh"
#include <G4PVReplica.hh>
#include <G4SDManager.hh>

//------------------------------------------------------------------------------
Geometry::Geometry()
    : pos_X_detector(0.0 * cm), pos_Y_detector(0.0 * cm),
      pos_Z_detector(0.0 * cm), rot_Y_detector(0.0 * deg), ptr_SV(nullptr),
      log_pixel(nullptr)
//------------------------------------------------------------------------------
{
  messenger = std::make_unique<GeometryMessenger>(this);
}
//------------------------------------------------------------------------------
Geometry::~Geometry() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
G4VPhysicalVolume *Geometry::Construct()
//------------------------------------------------------------------------------
{
  return ConstructWorld();
}

//------------------------------------------------------------------------------
G4VPhysicalVolume *Geometry::ConstructWorld()
//------------------------------------------------------------------------------
{
  G4cout << " === Geometry::ConstructWorld() =====" << G4endl;
  // Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();

  ////////////////////////////////////////////////////////
  /// Define World Volume
  ////////////////////////////////////////////////////////
  // Define the shape of solid
  const G4double world_length = 1.1 * cm;
  G4double leng_X_World = world_length; // X-full-length of world
  G4double leng_Y_World = world_length; // Y-full-length of world
  G4double leng_Z_World = world_length; // Z-full-length of world
  G4Box *solid_world = new G4Box("solid_world", leng_X_World / 2.0,
                                 leng_Y_World / 2.0, leng_Z_World / 2.0);

  // Define logical volume
  G4Material *materi_World = materi_Man->FindOrBuildMaterial("G4_AIR");
  G4LogicalVolume *log_world =
      new G4LogicalVolume(solid_world, materi_World, "log_world");
  log_world->SetVisAttributes(G4VisAttributes::Invisible);

  // Placement of logical volume
  G4int copyNum_World = 0; // Set ID number of world
  G4PVPlacement *physVol_world = new G4PVPlacement(
      G4Transform3D(), "physVol_world", log_world, 0, false, copyNum_World);

  G4int copyNum_LogV = 1;
  ////////////////////////////////////////////////////////
  /// Pixel Detector
  ////////////////////////////////////////////////////////
  auto log_detector = ConstructPixelDetector();
  G4ThreeVector t_detector =
      G4ThreeVector(pos_X_detector, pos_Y_detector, pos_Z_detector);
  G4RotationMatrix rot_detector = G4RotationMatrix();
  rot_detector.rotateY(rot_Y_detector);
  G4Transform3D Rt_detector = G4Transform3D(rot_detector, t_detector);
  copyNum_LogV = 1000;
  new G4PVPlacement(Rt_detector, "PhysVol_detector", log_detector,
                    physVol_world, false, copyNum_LogV);

  // Return the physical world
  return physVol_world;
}
//------------------------------------------------------------------------------
G4LogicalVolume *Geometry::ConstructPixelDetector()
//------------------------------------------------------------------------------
{
  ////////////////////////////////////////////////////////
  /// Define pixel setting
  ////////////////////////////////////////////////////////
  //    G4int resolution_w = 2592;
  //    G4int resolution_h = 1944;
  //  const G4int resolution_w = 128;
  //  const G4int resolution_h = 128;
//  const G4int resolution_w = 256;
//  const G4int resolution_h = 256;
    const G4int resolution_w = 960;
    const G4int resolution_h = 540;
  //  auto unit = 5*mm; // mm
  auto unit = um;
  const G4double pixel_x = 1.75 * unit;
  const G4double pixel_y = 1.75 * unit;
  //  const G4double pixel_z = 3 * 1.75 * unit; /// 10, /100
  const G4double pixel_z = 4 * unit; /// 10, /100

  // holder to prevent e-
  const bool with_holder = true;
  const G4double holder_thickness = 2.0 * mm;
  const G4double holder_offset = 0.1 * mm;

  // Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();
  G4Material *materi_air = materi_Man->FindOrBuildMaterial("G4_AIR");
  G4Material *materi_pixel =
      materi_Man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  //  G4Material *materi_holder = materi_Man->FindOrBuildMaterial("G4_Fe");
  G4Material *materi_holder = materi_Man->FindOrBuildMaterial("G4_Al");

  ////////////////////////////////////////////////////////
  /// Define Pixel Detector
  ////////////////////////////////////////////////////////
  // -------------------------------------------
  // Whole image sensor (Container)
  G4double sensor_x = resolution_w * pixel_x;
  G4double sensor_y = resolution_h * pixel_y;
  G4double sensor_z = pixel_z;
  auto solid_image_sensor = new G4Box("solid_image_sensor", sensor_x / 2.0,
                                      sensor_y / 2.0, sensor_z / 2.0);
  // Container material should be air
  auto log_image_sensor =
      new G4LogicalVolume(solid_image_sensor, materi_air, "log_image_sensor");
  //  log_image_sensor->SetVisAttributes(G4VisAttributes::Invisible);

  // -------------------------------------------
  // Row container (create replica along y-axis)
  G4double sensor_row_x = sensor_x;
  G4double sensor_row_y = pixel_y;
  G4double sensor_row_z = sensor_z;
  auto solid_image_row = new G4Box("solid_image_row", sensor_row_x / 2.0,
                                   sensor_row_y / 2.0, sensor_row_z / 2.0);
  // Container material should be air
  auto log_image_row =
      new G4LogicalVolume(solid_image_row, materi_air, "log_image_row");
  log_image_row->SetVisAttributes(G4VisAttributes::Invisible);
  // Create replica along y-axis
  new G4PVReplica("PhysVol_image_row", log_image_row, log_image_sensor, kYAxis,
                  resolution_h, sensor_row_y);

  // -------------------------------------------
  // Pixel
  auto solid_pixel =
      new G4Box("solid_pixel", pixel_x / 2.0, pixel_y / 2.0, pixel_z / 2.0);
  log_pixel = new G4LogicalVolume(solid_pixel, materi_pixel, "log_pixel");

  // Add color
  auto color = G4Colour(1.0, 0.0, 0.0, 0.1);
  G4VisAttributes *attribute = new G4VisAttributes(color);
  attribute->SetForceSolid(true);
  log_pixel->SetVisAttributes(attribute);
  //  log_pixel->SetVisAttributes(G4VisAttributes::Invisible);

  // Placement of pixel elements to the local envelop using Replica
  new G4PVReplica("PhysVol_pixel", log_pixel, log_image_row, kXAxis,
                  resolution_w, pixel_x);

  if (!with_holder) {
    return log_image_sensor;
  }
  // -------------------------------------------
  // Holder to prevent e-
  G4double holder_x = sensor_x + 2 * holder_thickness + 2 * holder_offset;
  G4double holder_y = sensor_y + 2 * holder_thickness + 2 * holder_offset;
  G4double holder_z = sensor_z + 2 * holder_thickness + 2 * holder_offset;
  auto solid_holder =
      new G4Box("solid_holder", holder_x / 2.0, holder_y / 2.0, holder_z / 2.0);
  auto log_holder =
      new G4LogicalVolume(solid_holder, materi_holder, "log_holder");
  //  log_holder->SetVisAttributes(G4VisAttributes::Invisible);
  // Add color
  auto holder_color = G4Colour(1.0, 1.0, 1.0, 0.3);
  G4VisAttributes *holder_attribute = new G4VisAttributes(holder_color);
  holder_attribute->SetForceSolid(true);
  log_holder->SetVisAttributes(holder_attribute);

  // offset area
  auto solid_offset = new G4Box(
      "solid_offset", holder_x / 2.0 - holder_thickness,
      holder_y / 2.0 - holder_thickness, holder_z / 2.0 - holder_thickness);
  auto log_offset =
      new G4LogicalVolume(solid_offset, materi_holder, "log_offset");
  log_offset->SetVisAttributes(G4VisAttributes::Invisible);
  G4int copyNum_LogV = 0;
  new G4PVPlacement(G4Transform3D(), log_offset, "PhysVol_offset", log_holder,
                    false, copyNum_LogV, true);

  // Place pixel detector in holder
  copyNum_LogV = 1;
  new G4PVPlacement(G4Transform3D(), log_image_sensor, "PhysVol_image_sensor",
                    log_offset, false, copyNum_LogV, true);

  return log_holder;
}
//------------------------------------------------------------------------------
void Geometry::ConstructSDandField()
//------------------------------------------------------------------------------
{
  ////////////////////////////////////////////////////////
  /// Sensitive volume
  /// https://twiki.cern.ch/twiki/bin/view/Geant4/QuickMigrationGuideForGeant4V10
  ////////////////////////////////////////////////////////
  if (ptr_SV == nullptr) {
    ptr_SV = new SensitiveVolume("SensitiveVolume");
    ptr_SV->SetVerboseLevel(0);
    G4SDManager::GetSDMpointer()->AddNewDetector(ptr_SV);
  }
  // Add sensitivity
  if (log_pixel)
    SetSensitiveDetector(log_pixel, ptr_SV);
}

//------------------------------------------------------------------------------
void Geometry::SetDetectorPosition(G4double x, G4double y, G4double z)
//------------------------------------------------------------------------------
{
  // detector pose
  pos_X_detector = x; // X-location LogV detector
  pos_Y_detector = y; // Y-location LogV detector
  pos_Z_detector = z; // Z-location LogV detector
}

//------------------------------------------------------------------------------
void Geometry::SetDetectorRot(G4double x, G4double y, G4double z)
//------------------------------------------------------------------------------
{
  rot_Y_detector = y; // Y-rotation LogV detector
  x;
  z;
}

//------------------------------------------------------------------------------
void Geometry::UpdateGeometry()
//------------------------------------------------------------------------------
{
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
