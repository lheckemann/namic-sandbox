
#include "vtkMRMLPerkStationModuleNode.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "itkSpatialOrientationAdapter.h"

#include "vtkIndent.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkStringArray.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLScalarVolumeNode.h"

#include "vtkSlicerLogic.h"

#include "PerkStationCommon.h"


// Internal file IO helper functions. -----------------------------------------

void
WriteBool( std::ostream& out, vtkIndent indent, bool var, std::string name )
{
  out << indent << name << "=\"" << var << "\"" << std::endl;
}



void
WriteDouble( std::ostream& out, vtkIndent indent, double var, std::string name )
{
  out << indent << name << "=\"" << var << "\"" << std::endl;
}



void
WriteInt( std::ostream& out, vtkIndent indent, int var, std::string name )
{
  out << indent << name << "=\"" << var << "\"" << std::endl;
}



void
WriteDoubleVector( std::ostream& out, vtkIndent indent, double* var,
                   std::string name, int n )
{
  out << indent << name << "=\"";
  for ( int i = 0; i < n; ++ i )
    {
    out << var[ i ] << " ";
    }
  out << "\"" << std::endl;
}



std::map< std::string, std::string >
ReadAttributes( std::istream& in )
{
  std::map< std::string, std::string > map;
  map.clear();
  
  std::string line;
  in.clear();   // Make sure that the whole file will be read.
  in.seekg( 0 );
  while ( std::getline( in, line ) )
    {
    std::string::size_type eqPos;
    eqPos = line.find( "=" );
    if ( eqPos == std::string::npos ) continue; // No "=" in line.
    
    std::string name = line.substr( 1, eqPos - 1 );
    std::string value = line.substr( eqPos + 2, line.size() - eqPos - 3 );
    
    map.insert( std::make_pair( name, value ) );
    }
    
  return map;
}


void
StringToBool( std::string str, bool& var )
{
  std::stringstream ss( str );
  ss >> var;
}

bool
StringToBool( std::string str )
{
  bool var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}


void
StringToInt( std::string str, int& var )
{
  std::stringstream ss( str );
  ss >> var;
}

void
StringToInt( std::string str, unsigned int& var )
{
  std::stringstream ss( str );
  ss >> var;
}

int
StringToInt( std::string str )
{
  int var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}


void
StringToDouble( std::string str, double& var )
{
  std::stringstream ss( str );
  ss >> var;
}

double
StringToDouble( std::string str )
{
  double var;
  std::stringstream ss( str );
  ss >> var;
  return var;
}


void
StringToDoubleVector( std::string str, double* var, int n )
{
  std::stringstream ss( str );
  for ( int i = 0; i < n; ++ i )
    ss >> var[ i ];
}



// Separate attName = Needle123_Length to a sectionInd=123 and sectionName=Length
bool GetAttNameSection( const std::string& attName, const std::string& groupName,
                        unsigned int &sectionInd, std::string &sectionName )
{
  int groupNameLen = groupName.length();
  if ( attName.compare( 0, groupNameLen, groupName ) != 0 )
  {
    return false; // group name doesn't match
  }
    // search for the first separator character after the group name
  std::string::size_type separatorPos = attName.find( '_', groupNameLen );
  if ( separatorPos == std::string::npos )
  {
    return false;  // separator not found
  }
  sectionName = attName.substr ( separatorPos+1, attName.length() - separatorPos + 1 );
  if ( sectionName.empty() )
  {
    return false;  // failed to read sectionName
  }
  std::string indexString = attName.substr ( groupNameLen, separatorPos - groupNameLen );
  std::stringstream ss;
  ss << indexString;
  ss.clear();
  ss >> sectionInd;
  if ( ! ss )
  {
    return false; // failed to read sectionIndex
  }
  return true;
}

// ----------------------------------------------------------------------------




vtkMRMLPerkStationModuleNode*
vtkMRMLPerkStationModuleNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPerkStationModuleNode");
  if(ret)
    {
      return ( vtkMRMLPerkStationModuleNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLPerkStationModuleNode;
}


//----------------------------------------------------------------------------
vtkMRMLNode*
vtkMRMLPerkStationModuleNode
::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPerkStationModuleNode");
  if(ret)
    {
      return ( vtkMRMLPerkStationModuleNode* )ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLPerkStationModuleNode;
}



/**
 * Constructor.
 */
vtkMRMLPerkStationModuleNode
::vtkMRMLPerkStationModuleNode()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  
  
    // Hardware list.
  
  this->HardwareListFileName = "OverlayHardwareList.xml";
  this->UpdateHardwareListFile();
  
  
  this->HardwareList.clear();
  
  OverlayHardware siemens;
    siemens.FlipHorizontal = true;
    siemens.FlipVertical = false;
    siemens.Name = "Siemens MR compatible";
    siemens.SizeX = 360.0;
    siemens.SizeY = 290.0;
  this->HardwareList.push_back( siemens );
  
  OverlayHardware viewsonic;
    viewsonic.FlipHorizontal = true;
    viewsonic.FlipVertical = false;
    viewsonic.Name = "PerkStation ViewSonic";
    viewsonic.SizeX = 305.0;
    viewsonic.SizeY = 228.0;
  this->HardwareList.push_back( viewsonic );
  
  OverlayHardware perkdell;
    perkdell.FlipHorizontal = false;
    perkdell.FlipVertical = false;
    perkdell.Name = "PerkStation External Dell";
    perkdell.SizeX = 360.0;
    perkdell.SizeY = 340.0;
  this->HardwareList.push_back( perkdell );
  
  OverlayHardware acer;
    acer.FlipHorizontal = false;
    acer.FlipVertical = false;
    acer.Name = "Acer desktop";
    acer.SizeX = 433.0;
    acer.SizeY = 270.9;
  this->HardwareList.push_back( acer );
  
  OverlayHardware dell;
    dell.FlipHorizontal = false;
    dell.FlipVertical = false;
    dell.Name = "Dell desktop";
    dell.SizeX = 531.0;
    dell.SizeY = 299.0;
  this->HardwareList.push_back( dell );
  
  this->HardwareIndex = 0;
  
  
  // Calibration parameters ---------------------------------------------------
  
  this->CurrentCalibration = -1; // No calibration;
  this->CalibrationUID = 0;
  this->CurrentSliceOffset = 0.0;
  this->PatientPosition = PPNA;
  this->TableAtOverlay = 0.0;
  
  
    // Plan parameters --------------------------------------------------------
  
  this->TiltAngle = 0;
  this->SliceToRAS = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->PlanList.clear();
  this->PlanUID = 1;
  this->CurrentPlanIndex = -1;
  
    // Insertion parameters ---------------------------------------------------
  
  this->TrackerToPhantomMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->PhantomToImageRASMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  this->ToolTipOffset[ 0 ] = 0.0;
  this->ToolTipOffset[ 1 ] = 0.0;
  this->ToolTipOffset[ 2 ] = 0.0;
  
  this->ReferenceBodyToolPort = 0;
  this->NeedleToolPort        = 1;
  
  
    // Common parameters ------------------------------------------------------
  
  this->PlanningVolumeRef   = NULL;
  this->ValidationVolumeRef = NULL;
  this->VolumeInUse         = NULL;
  
  PlanningVolumeNode        = NULL;
  ValidationVolumeNode      = NULL;
  
  this->TimeOnCalibrateStep = 0.0;
  this->TimeOnPlanStep      = 0.0;
  this->TimeOnInsertStep    = 0.0;
  this->TimeOnValidateStep  = 0.0;
  
  
  // this->InitializeFiducialListNode();
  
    
    // Synchronize it with enum WORKPHASE...
  
  this->StepList = vtkSmartPointer< vtkStringArray >::New();
    this->StepList->InsertNextValue( "Calibration" );
    this->StepList->InsertNextValue( "Planning" );
    this->StepList->InsertNextValue( "Insertion" );
    this->StepList->InsertNextValue( "Validation" );
  
  this->CurrentStep  = 0;
  this->PreviousStep = WORKPHASE_CALIBRATION;
}



vtkMRMLPerkStationModuleNode
::~vtkMRMLPerkStationModuleNode()
{
  if ( this->PlanningVolumeNode )
    {
    vtkSetMRMLNodeMacro( this->PlanningVolumeNode, NULL );
    }
  
  if ( this->ValidationVolumeNode )
    {
    vtkSetMRMLNodeMacro( this->ValidationVolumeNode, NULL );
    }
  
  
    // Calibration list.
  
  for ( std::vector< OverlayCalibration* >::iterator it = this->CalibrationList.begin();
        it != this->CalibrationList.end(); ++ it )
    {
    if ( (*it) != NULL )
      {
      delete (*it);
      (*it) = NULL;
      }
    }
}



void
vtkMRMLPerkStationModuleNode
::WriteXML( ostream& of, int nIndent )
{  
  Superclass::WriteXML( of, nIndent );
  of << std::endl;
  
    // Write all MRML node attributes into output stream
  
  vtkIndent indent( nIndent );
  
  
    // Global parameters.
  
  of << indent << "PlanUID=\"" << this->PlanUID << "\"" << std::endl;
  of << indent << "HardwareIndex=\"" << this->HardwareIndex << "\"" << std::endl;
  
  
    // Calibration list.
  
  of << indent << "CurrentCalibration=\"" << this->CurrentCalibration << "\"" << std::endl;
  for ( unsigned int calInd = 0; calInd < this->CalibrationList.size(); ++ calInd )
    {
    of << indent << "Calibration" << calInd << "_Name=\""
       << this->CalibrationList[ calInd ]->Name << "\"" << std::endl;
    of << indent << "Calibration" << calInd << "_SecondMonitorVerticalFlip=\""
       << this->CalibrationList[ calInd ]->SecondMonitorVerticalFlip << "\"" << std::endl;
    of << indent << "Calibration" << calInd << "_SecondMonitorHorizontalFlip=\""
       << this->CalibrationList[ calInd ]->SecondMonitorHorizontalFlip << "\"" << std::endl;
    
    of << indent << "Calibration" << calInd << "_SecondMonitorTranslation=\"";
    of << this->CalibrationList[ calInd ]->SecondMonitorTranslation[ 0 ] << " ";
    of << this->CalibrationList[ calInd ]->SecondMonitorTranslation[ 1 ] << "\"";
    of << std::endl;
    
    of << indent << "Calibration" << calInd << "_SecondMonitorRotation=\""
       << this->CalibrationList[ calInd ]->SecondMonitorRotation << "\"" << std::endl;
    
    of << indent << "Calibration" << calInd << "_SecondMonitorRotationCenter=\"";
    of << this->CalibrationList[ calInd ]->SecondMonitorRotationCenter[ 0 ] << " ";
    of << this->CalibrationList[ calInd ]->SecondMonitorRotationCenter[ 1 ] << "\"";
    of << std::endl;
    }
  
  of << indent << "TableAtOverlay=\"" << this->TableAtOverlay << "\"" << std::endl;
    
    // Plan list.
  
  of << indent << "CurrentPlanIndex=\"" << this->CurrentPlanIndex << "\"" << std::endl;
  for ( unsigned int planInd =0;
        planInd < this->PlanList.size();
        planInd ++ )
    {
    of << indent << "Plan" << planInd << "_Name=\""
       << this->PlanList[ planInd ]->GetName() << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_PlanningVolumeRef=\""
       << this->PlanList[ planInd ]->GetPlanningVolumeRef() << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_EntryRASLocation=\""
       << this->PlanList[ planInd ]->GetEntryPointRAS()[ 0 ] << " "
       << this->PlanList[ planInd ]->GetEntryPointRAS()[ 1 ] << " "
       << this->PlanList[ planInd ]->GetEntryPointRAS()[ 2 ] << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_TargetRASLocation=\""
       << this->PlanList[ planInd ]->GetTargetPointRAS()[ 0 ] << " "
       << this->PlanList[ planInd ]->GetTargetPointRAS()[ 1 ] << " "
       << this->PlanList[ planInd ]->GetTargetPointRAS()[ 2 ] << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_Validated=\""
       << this->PlanList[ planInd ]->GetValidated() << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_ValidationVolumeRef=\""
       << this->PlanList[ planInd ]->GetValidationVolumeRef() << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_ValidationEntryRASLocation=\""
       << this->PlanList[ planInd ]->GetValidationEntryPointRAS()[ 0 ] << " "
       << this->PlanList[ planInd ]->GetValidationEntryPointRAS()[ 1 ] << " "
       << this->PlanList[ planInd ]->GetValidationEntryPointRAS()[ 2 ] << "\"" << std::endl;
    of << indent << "Plan" << planInd << "_ValidationTargetRASLocation=\""
       << this->PlanList[ planInd ]->GetValidationTargetPointRAS()[ 0 ] << " "
       << this->PlanList[ planInd ]->GetValidationTargetPointRAS()[ 1 ] << " "
       << this->PlanList[ planInd ]->GetValidationTargetPointRAS()[ 2 ] << "\"" << std::endl;
    }
  
    // Times.
  
  WriteDouble( of, indent, this->TimeOnCalibrateStep, "TimeOnCalibrateStep" );
  WriteDouble( of, indent, this->TimeOnPlanStep, "TimeOnPlanStep" );
  WriteDouble( of, indent, this->TimeOnInsertStep, "TimeOnInsertStep" );
  WriteDouble( of, indent, this->TimeOnValidateStep, "TimeOnValidateStep" );
}



void
vtkMRMLPerkStationModuleNode
::ReadXMLAttributes( const char** atts )
{
  vtkMRMLNode::ReadXMLAttributes( atts );

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  
  while ( *atts != NULL )
    {
    attName = *( atts++ );
    attValue = *( atts++ );
    
    unsigned int sectionInd = 0;
    std::string sectionName;
    
    
      // Global parameters.
    
    if ( ! strcmp( attName, "PlanUID" ) ) StringToInt( std::string( attValue ), this->PlanUID );
    else if ( ! strcmp( attName, "HardwareIndex" ) ) StringToInt( std::string( attValue ), this->HardwareIndex );
    
  
      // Calibration list.
    
    if ( GetAttNameSection( attName, "Calibration", sectionInd, sectionName ) )
      {
      if ( sectionInd >= this->CalibrationList.size() ) {
        this->CalibrationList.resize( sectionInd + 1 );
        this->CalibrationList[ sectionInd ] = new OverlayCalibration;
        }
      OverlayCalibration* calib = this->CalibrationList[ sectionInd ];
      
      if ( ! sectionName.compare( "Name" ) ) {
        calib->Name = ( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "SecondMonitorVerticalFlip" ) ) {
        calib->SecondMonitorVerticalFlip = StringToBool( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "SecondMonitorHorizontalFlip" ) ) {
        calib->SecondMonitorHorizontalFlip = StringToBool( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "SecondMonitorTranslation" ) ) {
        double tx[ 2 ];
        StringToDoubleVector( attValue, tx, 2 );
        calib->SecondMonitorTranslation[ 0 ] = tx[ 0 ];
        calib->SecondMonitorTranslation[ 1 ] = tx[ 1 ];
        }
      else if ( ! sectionName.compare( "SecondMonitorRotation" ) ) {
        calib->SecondMonitorRotation = StringToDouble( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "SecondMonitorRotationCenter" ) ) {
        double rc[ 2 ];
        StringToDoubleVector( attValue, rc, 2 );
        calib->SecondMonitorRotationCenter[ 0 ] = rc[ 0 ];
        calib->SecondMonitorRotationCenter[ 1 ] = rc[ 1 ];
        }
      }
    
    if ( ! strcmp( attName, "TableAtOverlay" ) ) {
      StringToDouble( std::string( attValue ), this->TableAtOverlay );
      }
    
    
      // Plan list.
    
    if ( GetAttNameSection( attName, "Plan", sectionInd, sectionName ) )
      {
      if ( sectionInd >= this->PlanList.size() ) {
        this->PlanList.resize( sectionInd + 1 );
        this->PlanList[ sectionInd ] = vtkPerkStationPlan::New();
        }
      vtkPerkStationPlan* plan = this->PlanList[ sectionInd ];
      
      if ( ! sectionName.compare( "Name" ) ) {
        plan->SetName( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "PlanningVolumeRef" ) ) {
        plan->SetPlanningVolumeRef( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "EntryRASLocation" ) ) {
        double loc[ 3 ];
        StringToDoubleVector( attValue, loc, 3 );
        plan->SetEntryPointRAS( loc );
        }
      else if ( ! sectionName.compare( "TargetRASLocation" ) ) {
        double loc[ 3 ];
        StringToDoubleVector( attValue, loc, 3 );
        plan->SetTargetPointRAS( loc );
        }
      else if ( ! sectionName.compare( "Validated" ) ) {
        bool value;
        StringToBool( attValue, value );
        plan->SetValidated( value );
        }
      else if ( ! sectionName.compare( "ValidationVolumeRef" ) ) {
        plan->SetValidationVolumeRef( std::string( attValue ) );
        }
      else if ( ! sectionName.compare( "ValidationEntryRASLocation" ) ) {
        double loc[ 3 ];
        StringToDoubleVector( attValue, loc, 3 );
        plan->SetValidationEntryPointRAS( loc );
        }
      else if ( ! sectionName.compare( "ValidationTargetRASLocation" ) ) {
        double loc[ 3 ];
        StringToDoubleVector( attValue, loc, 3 );
        plan->SetValidationTargetPointRAS( loc );
        }
      } // if ( GetAttNameSection( attName, "Plan", sectionInd, sectionName ) )
    
    
      // Common parameters.
    
    if ( ! strcmp( attName, "TimeOnCalibrateStep" ) ) {
      StringToDouble( std::string( attValue ), this->TimeOnCalibrateStep );
      }
    else if ( ! strcmp( attName, "TimeOnPlanStep" ) ) {
      StringToDouble( std::string( attValue ), this->TimeOnPlanStep );
      }
    else if ( ! strcmp( attName, "TimeOnInsertStep" ) ) {
      StringToDouble( std::string( attValue ), this->TimeOnInsertStep );
      }
    else if ( ! strcmp( attName, "TimeOnValidateStep" ) ) {
      StringToDouble( std::string( attValue ), this->TimeOnValidateStep );
      }
    
    } // while ( *atts != NULL )
}



// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLPerkStationModuleNode
::Copy( vtkMRMLNode *anode )
{
  Superclass::Copy( anode );
  vtkMRMLPerkStationModuleNode *node = (vtkMRMLPerkStationModuleNode *) anode;
  
  this->SetPlanningVolumeRef( node->PlanningVolumeRef );
  this->SetValidationVolumeRef( node->ValidationVolumeRef );
  this->SetPlanningVolumeNode( node->GetPlanningVolumeNode( ));
  this->SetValidationVolumeNode( node->GetValidationVolumeNode() );
  
  
    // Work phases.
  
  this->StepList->Reset();
  for ( int i = 0; i > node->StepList->GetSize(); i ++ )
  {
    this->StepList->SetValue( i, node->StepList->GetValue( i ) );
  }
  
  this->CurrentStep = node->CurrentStep;
  this->PreviousStep = node->PreviousStep;
}



double
vtkMRMLPerkStationModuleNode
::GetSecondMonitorRotation() const
{
  if ( this->CurrentCalibration < 0 ) return 0;
  return this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotation;
}



void
vtkMRMLPerkStationModuleNode
::SetSecondMonitorRotation( const double rot )
{
  if ( this->CurrentCalibration < 0 ) return;
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotation = rot;
}



void
vtkMRMLPerkStationModuleNode
::GetSecondMonitorRotationCenter( double* rotCent ) const
{
  if ( this->CurrentCalibration < 0 ) return;
  rotCent[ 0 ] = this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotationCenter[ 0 ];
  rotCent[ 1 ] = this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotationCenter[ 1 ];
}



void
vtkMRMLPerkStationModuleNode
::SetSecondMonitorRotationCenter( const double* rotCent )
{
  if ( this->CurrentCalibration < 0 ) return;
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotationCenter[ 0 ] = rotCent[ 0 ];
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotationCenter[ 1 ] = rotCent[ 1 ];
}



bool
vtkMRMLPerkStationModuleNode
::GetSecondMonitorVerticalFlip() const
{
  bool ret = false;
  if ( this->CurrentCalibration >= 0 )
    {
    ret = this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorVerticalFlip;
    }
  return ret;
}



void
vtkMRMLPerkStationModuleNode
::SetSecondMonitorVerticalFlip( bool flip )
{
  if ( this->CurrentCalibration < 0 ) return;
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorVerticalFlip = flip;
}



bool
vtkMRMLPerkStationModuleNode
::GetSecondMonitorHorizontalFlip() const
{
  bool ret = false;
  if ( this->CurrentCalibration >= 0 )
    {
    ret = this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorHorizontalFlip;
    }
  return ret;
}



void
vtkMRMLPerkStationModuleNode
::SetSecondMonitorHorizontalFlip( bool flip )
{
  if ( this->CurrentCalibration < 0 ) return;
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorHorizontalFlip = flip;
}



void
vtkMRMLPerkStationModuleNode
::GetSecondMonitorTranslation( double* tx ) const
{
  if ( this->CurrentCalibration < 0 ) return;
  tx[ 0 ] = this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorTranslation[ 0 ];
  tx[ 1 ] = this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorTranslation[ 1 ];
}



void
vtkMRMLPerkStationModuleNode
::SetSecondMonitorTranslation( const double* tx )
{
  if ( this->CurrentCalibration < 0 ) return;
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorTranslation[ 0 ] = tx[ 0 ];
  this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorTranslation[ 1 ] = tx[ 1 ];
}



double
vtkMRMLPerkStationModuleNode
::GetTableAtOverlay() const
{
  return this->TableAtOverlay;
}



void
vtkMRMLPerkStationModuleNode
::SetTableAtOverlay( double pos )
{
  this->TableAtOverlay = pos;
}



std::vector< OverlayHardware >
vtkMRMLPerkStationModuleNode
::GetHardwareList()
{
  return this->HardwareList;
}



void
vtkMRMLPerkStationModuleNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf( os, indent );

  os << indent << "PlanningVolumeRef: " << 
   ( this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)" ) << "\n";
 
}



void vtkMRMLPerkStationModuleNode::UpdateReferenceID( const char *oldID, const char *newID )
{
  Superclass::UpdateReferenceID( oldID, newID );
  
  
  if ( this->PlanningVolumeRef && ! strcmp( oldID, this->PlanningVolumeRef ) )
    {
    this->SetPlanningVolumeRef( newID );
    }
  
  if ( this->ValidationVolumeRef && ! strcmp( oldID, this->ValidationVolumeRef ) )
    {
    this->SetValidationVolumeRef( newID );
    }
  
  /*
  if (!strcmp(oldID, this->InputVolumeRef))
    {
    this->SetInputVolumeRef(newID);
    }*/
}


/*
void
vtkMRMLPerkStationModuleNode
::InitializeFiducialListNode()
{
  this->PlanMRMLFiducialListNode = vtkSmartPointer< vtkMRMLFiducialListNode >::New();
  
  this->PlanMRMLFiducialListNode->SetName( "PerkStationFiducialList" );
  this->PlanMRMLFiducialListNode->SetDescription(
    "Created by PERK Station Module; marks entry point and target point" );
  this->PlanMRMLFiducialListNode->SetColor( 0.5, 1, 0.5 );
  this->PlanMRMLFiducialListNode->SetGlyphType( vtkMRMLFiducialListNode::Diamond3D );
  this->PlanMRMLFiducialListNode->SetOpacity( 0.7 );
  this->PlanMRMLFiducialListNode->SetAllFiducialsVisibility( true );
  this->PlanMRMLFiducialListNode->SetSymbolScale( 6 );
  this->PlanMRMLFiducialListNode->SetTextScale( 8 );
  this->PlanMRMLFiducialListNode->SetSaveWithScene( 0 );
}
*/


vtkMRMLScalarVolumeNode*
vtkMRMLPerkStationModuleNode
::GetActiveVolumeNode()
{
  if ( ! this->VolumeInUse ) return NULL;
  
  if ( strcmpi( this->VolumeInUse, "Planning" ) == 0 )
    {
    return this->PlanningVolumeNode;
    }
  else if ( strcmpi( this->VolumeInUse, "Validation" ) == 0 )
    {
    return this->ValidationVolumeNode;
    }
  else
    {
    return NULL;
    }
}



void
vtkMRMLPerkStationModuleNode
::UpdateHardwareCalibration( double mmX, double mmY )
{
  this->HardwareList[ this->HardwareIndex ].SizeX = mmX;
  this->HardwareList[ this->HardwareIndex ].SizeY = mmY;
}



void
vtkMRMLPerkStationModuleNode
::GetSecondMonitorPhysicalSize( double* mm ) const
{
  mm[ 0 ] = this->HardwareList[ this->HardwareIndex ].SizeX;
  mm[ 1 ] = this->HardwareList[ this->HardwareIndex ].SizeY;
}



unsigned int
vtkMRMLPerkStationModuleNode
::AddCalibration( OverlayCalibration* newCalibration )
{
  unsigned int index = this->CalibrationList.size();
  this->CalibrationList.push_back( newCalibration );
  return index;
}



int
vtkMRMLPerkStationModuleNode
::RemoveCalibrationAtIndex( unsigned int index )
{
  if ( index >= this->CalibrationList.size() )
    {
    return 0;
    }
  OverlayCalibration* calibration = GetCalibrationAtIndex( index );
  if ( calibration != NULL )
  {
    delete calibration;
  }
  this->CalibrationList.erase( this->CalibrationList.begin() + index );
  return 1;
}



OverlayCalibration*
vtkMRMLPerkStationModuleNode
::GetCalibrationAtIndex( unsigned int index )
{
  if ( index < this->CalibrationList.size() )
    {
    return this->CalibrationList[ index ];
    }
  else
    {
    return NULL;
    }
}



int
vtkMRMLPerkStationModuleNode
::SetCurrentCalibrationIndex( int index )
{
  if ( index >= (int)this->CalibrationList.size() )
    {
    return this->CurrentCalibration; // invalid index, do not change current
    }
  this->CurrentCalibration = index;
  
  this->Modified();
  // this->InvokeEvent( vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent );
  
  return this->CurrentCalibration;
}



void
vtkMRMLPerkStationModuleNode
::GetPlanEntryPoint( double* point, int index ) const
{
  if ( index == -1 ) index = this->CurrentPlanIndex;
  
  int nPlans = this->GetNumberOfPlans();
  if ( index < 0 || index >= nPlans ) return;
  
  this->PlanList[ index ]->GetEntryPointRAS( point );
}



void
vtkMRMLPerkStationModuleNode
::SetPlanEntryPoint( const double* point )
{
  if ( this->CurrentPlanIndex < 0 ) return;
  this->PlanList[ this->CurrentPlanIndex ]->SetEntryPointRAS(
    point[ 0 ], point[ 1 ], point[ 2 ] );
}



void
vtkMRMLPerkStationModuleNode
::GetPlanTargetPoint( double* point, int index ) const
{
  if ( index == -1 ) index = this->CurrentPlanIndex;
  
  if ( index < 0 || index >= ( this->GetNumberOfPlans() ) )
    {
    return;
    }
  
  this->PlanList[ index ]->GetTargetPointRAS( point );
}



void
vtkMRMLPerkStationModuleNode
::SetPlanTargetPoint( const double* point )
{
  if ( this->CurrentPlanIndex < 0 ) return;
  this->PlanList[ this->CurrentPlanIndex ]->SetTargetPointRAS(
    point[ 0 ], point[ 1 ], point[ 2 ] );
}



void
vtkMRMLPerkStationModuleNode
::AddNewPlan()
{
  std::stringstream ss;
    ss << "Plan" << this->PlanUID;
  ++ this->PlanUID;
  
  vtkSmartPointer< vtkPerkStationPlan > plan = vtkSmartPointer< vtkPerkStationPlan >::New();
    plan->SetName( ss.str() );
    plan->SetPlanningVolumeRef( std::string( this->PlanningVolumeRef ) );
  
  unsigned int index = this->AddPlan( plan );
  this->SetCurrentPlanIndex( index );
}



vtkMRMLScalarVolumeNode*
vtkMRMLPerkStationModuleNode
::GetPlanningVolumeNode()
{
  return this->PlanningVolumeNode;
}

  

void
vtkMRMLPerkStationModuleNode
::SetPlanningVolumeNode( vtkMRMLScalarVolumeNode *planVolNode )
{
  if ( planVolNode != NULL )
    {
    vtkSetMRMLNodeMacro( this->PlanningVolumeNode, planVolNode );
    // this->PlanningVolumeNode = planVolNode;
    this->PlanningVolumeRef = this->PlanningVolumeNode->GetID();
    }
}



void
vtkMRMLPerkStationModuleNode
::SetValidationVolumeNode( vtkMRMLScalarVolumeNode* validationVolNode )
{
  if ( validationVolNode != NULL )
    {
    vtkSetMRMLNodeMacro( this->ValidationVolumeNode, validationVolNode );
    this->ValidationVolumeRef = this->ValidationVolumeNode->GetID();
    }
}



/**
 * @returns Number of workphase steps.
 */
int
vtkMRMLPerkStationModuleNode
::GetNumberOfSteps()
{
  return this->StepList->GetNumberOfValues();
}



/**
 * @returns Current step ID.
 */
int
vtkMRMLPerkStationModuleNode
::GetCurrentStep()
{
  return this->CurrentStep;
}



/**
 * @returns Previous step ID.
 */
int
vtkMRMLPerkStationModuleNode
::GetPreviousStep()
{
  return this->PreviousStep;
}
  


/**
 * Modifies the workphase step.
 */
int
vtkMRMLPerkStationModuleNode
::SwitchStep( int newStep )
{
  if ( newStep < 0 )
    {
    return 0;
    }
  
    // Don't allow planning without planning volume.
  if (    newStep == WORKPHASE_PLANNING
       && this->PlanningVolumeNode == NULL )
    {
    return 0;
    }
  
    // Don't allow validation without validation volume.
  if (    newStep == WORKPHASE_VALIDATION
       && this->ValidationVolumeNode == NULL )
    {
    return 0;
    }
  
  
  this->PreviousStep = this->CurrentStep;
  this->CurrentStep = newStep;
  
  
    // Update active volume.
  
  if ( this->CurrentStep == WORKPHASE_VALIDATION )
    {
    this->SetVolumeInUse( "Validation" );
    }
  else
    {
    if ( this->PlanningVolumeNode )
      {
      this->SetVolumeInUse( "Planning" );
      }
    }
  
  return 1;
}



/**
 * @returns Name of work phase step i.
 */
const char*
vtkMRMLPerkStationModuleNode
::GetStepName( int i )
{
  if ( i >= 0 && i < this->StepList->GetNumberOfValues() )
    {
      return this->StepList->GetValue( i );
    }
  else
    {
    return NULL;
    }
}



/**
 * @returns Patient Position as read from the input DICOM information.
 */
PatientPositionEnum
vtkMRMLPerkStationModuleNode
::GetPatientPosition()
{  
  if ( this->PlanningVolumeNode == NULL )
  {
    return PPNA;
  }

  // remaining information to be had from the meta data dictionary     
  const itk::MetaDataDictionary &volDictionary =
    this->PlanningVolumeNode->GetMetaDataDictionary();
  std::string tagValue; 

  // patient position uid
  tagValue.clear();
  
  itk::ExposeMetaData< std::string >( volDictionary, "0018|5100", tagValue );
  
    // Remove spaces.
  std::stringstream ss;
  for ( unsigned int i = 0; i < tagValue.size(); i ++ )
    {
    if ( tagValue[ i ] != ' ' )
      {
      ss << tagValue[ i ];
      }
    }
  std::string tagValueTrimmed = ss.str();
  
  if ( tagValueTrimmed == "HFP" ) this->PatientPosition = HFP;
  else if ( tagValueTrimmed == "HFS" ) this->PatientPosition = HFS;
  else if ( tagValueTrimmed == "HFDR" ) this->PatientPosition = HFDR;
  else if ( tagValueTrimmed == "HFDL" ) this->PatientPosition = HFDL;
  else if ( tagValueTrimmed == "FFDR" ) this->PatientPosition = FFDR;
  else if ( tagValueTrimmed == "FFDL" ) this->PatientPosition = FFDL;
  else if ( tagValueTrimmed == "FFP" ) this->PatientPosition = FFP;
  else if ( tagValueTrimmed == "FFS" ) this->PatientPosition = FFS;
  
  return this->PatientPosition;
}


/**
 * @returns The required table position for the current insertion slice.
 */
double
vtkMRMLPerkStationModuleNode
::GetCurrentTablePosition()
{
    // Offset direction positive if the patient lies head-first,
    // because that is positive in RAS system.
  
  if ( this->CurrentCalibration < 0 ) return 0.0;
  
  double offsetDirection = 1.0;
  if (    this->PatientPosition == FFDR
       || this->PatientPosition == FFDL
       || this->PatientPosition == FFP
       || this->PatientPosition == FFS )
    {
    offsetDirection = - 1.0;
    }
  
  double directedOffset = this->CurrentSliceOffset * offsetDirection;
  
  return this->TableAtOverlay + directedOffset;
}




bool
vtkMRMLPerkStationModuleNode
::GetFinalHorizontalFlip()
{
  bool ret = false;
  if ( this->HardwareIndex >= 0 )
    {
    ret = this->HardwareList[ this->HardwareIndex ].FlipHorizontal;
    }
  if (    this->CurrentCalibration >= 0
       && this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorHorizontalFlip )
    {
    ret = ! ret;
    }
  return ret;
}



bool
vtkMRMLPerkStationModuleNode
::GetFinalVerticalFlip()
{
  bool ret = false;
  if ( this->HardwareIndex >= 0 )
    {
    ret = this->HardwareList[ this->HardwareIndex ].FlipVertical;
    }
  if (    this->CurrentCalibration >= 0
       && this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorVerticalFlip )
    {
    ret = ! ret;
    }
  return ret;
}



/**
 * @returns Euclidean distance between plan and validation entry points.
 */
double
vtkMRMLPerkStationModuleNode
::GetEntryPointError( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double planEntry[ 3 ];
  this->GetPlanEntryPoint( planEntry, index );
  
  double validationEntry[ 3 ];
  this->GetValidationEntryPoint( validationEntry, index );
  
  double sum = 0.0;
  
  for ( int i = 0; i < 3; ++ i )
    {
    sum += (   ( validationEntry[ i ] - planEntry[ i ] )
             * ( validationEntry[ i ] - planEntry[ i ] ) );
    }
  
  return std::sqrt( sum );
}



double
vtkMRMLPerkStationModuleNode
::GetEntryPointErrorR( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double planEntry[ 3 ];
  this->GetPlanEntryPoint( planEntry, index );
  
  double validationEntry[ 3 ];
  this->GetValidationEntryPoint( validationEntry, index );
  
  return ( validationEntry[ 0 ] - planEntry[ 0 ] );
}



double
vtkMRMLPerkStationModuleNode
::GetEntryPointErrorA( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double planEntry[ 3 ];
  this->GetPlanEntryPoint( planEntry, index );
  
  double validationEntry[ 3 ];
  this->GetValidationEntryPoint( validationEntry, index );
  
  return ( validationEntry[ 1 ] - planEntry[ 1 ] );
}



double
vtkMRMLPerkStationModuleNode
::GetEntryPointErrorS( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double planEntry[ 3 ];
  this->GetPlanEntryPoint( planEntry, index );
  
  double validationEntry[ 3 ];
  this->GetValidationEntryPoint( validationEntry, index );
  
  return ( validationEntry[ 2 ] - planEntry[ 2 ] );
}



/**
 * @returns Euclidean distance between plan and validation target points.
 */
double
vtkMRMLPerkStationModuleNode
::GetTargetPointError( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double planTarget[ 3 ];
  this->GetPlanTargetPoint( planTarget, index );
  
  double validationTarget[ 3 ];
  this->GetValidationTargetPoint( validationTarget, index );
  
  double sum = 0.0;
  
  for ( int i = 0; i < 3; ++ i )
    {
    sum += (   ( validationTarget[ i ] - planTarget[ i ] )
             * ( validationTarget[ i ] - planTarget[ i ] ) );
    }
  
  return std::sqrt( sum );
}



double
vtkMRMLPerkStationModuleNode
::GetTargetPointErrorR( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double plan[ 3 ];
  this->GetPlanTargetPoint( plan, index );
  
  double validation[ 3 ];
  this->GetValidationTargetPoint( validation, index );
  
  return ( validation[ 0 ] - plan[ 0 ] );
}



double
vtkMRMLPerkStationModuleNode
::GetTargetPointErrorA( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double plan[ 3 ];
  this->GetPlanTargetPoint( plan, index );
  
  double validation[ 3 ];
  this->GetValidationTargetPoint( validation, index );
  
  return ( validation[ 1 ] - plan[ 1 ] );
}



double
vtkMRMLPerkStationModuleNode
::GetTargetPointErrorS( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double plan[ 3 ];
  this->GetPlanTargetPoint( plan, index );
  
  double validation[ 3 ];
  this->GetValidationTargetPoint( validation, index );
  
  return ( validation[ 2 ] - plan[ 2 ] );
}



double
vtkMRMLPerkStationModuleNode
::GetActualPlanInsertionAngle( int index )
{
  return this->GetPlanAngleAxial( index );
}



/**
 * @returns Euclidean distance between plan target and entry points.
 */
double
vtkMRMLPerkStationModuleNode
::GetActualPlanInsertionDepth( int index )
{
  if ( index == -1 )
    {
    index = this->GetCurrentPlanIndex();
    }
  
  double planEntry[ 3 ];
  this->GetPlanEntryPoint( planEntry, index );
  double planTarget[ 3 ];
  this->GetPlanTargetPoint( planTarget, index );
  
  double insDepth = sqrt( vtkMath::Distance2BetweenPoints( planTarget, planEntry ) );
  return insDepth;
}



/**
 * Angle definition: Zero is the antero-posterior axis, when entry point is to the posterior.
 * Positive when entry is more to the right of the patient than target.
 *
 * @returns Insertion angle in degrees, between anteroposterior line and plan projected to the axial plane.
 */
double
vtkMRMLPerkStationModuleNode
::GetPlanAngleAxial( int index )
{
 if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double entry[ 3 ];
  double target[ 3 ];
  
  this->GetPlanEntryPoint( entry, index );
  this->GetPlanTargetPoint( target, index );
  
  
    // entry[ 0 ] - target[ 0 ] is positive when entry is to the right.
    // target[ 1 ] - entry[ 1 ] is positive when entry is to the posterior.
    // atan( tangent ) is positive when tangent is positive.
  
  double tangent = ( entry[ 0 ] - target[ 0 ] ) / ( target[ 1 ] - entry[ 1 ] );
  double angle = 180.0 / vtkMath::Pi() * atan( tangent );
  
  return angle;
}



/**
 * Angle definition: Zero is the antero-posterior axis, when entry point is to the posterior.
 * Positive when entry is more to the superior of the patient than target.
 *
 * @returns Insertion angle in degrees, between anteroposterior line and plan projected to the axial plane.
 */
double
vtkMRMLPerkStationModuleNode
::GetPlanAngleSagittal( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double entry[ 3 ];
  double target[ 3 ];
  
  this->GetPlanEntryPoint( entry, index );
  this->GetPlanTargetPoint( target, index );
  
  
    // entry[ 2 ] - target[ 2 ] is positive when entry is to the superior.
    // target[ 1 ] - entry[ 1 ] is positive when entry is to the posterior.
    // atan( tangent ) is positive when tangent is positive.
  
  double tangent = ( entry[ 2 ] - target[ 2 ] ) / ( target[ 1 ] - entry[ 1 ] );
  double angle = 180.0 / vtkMath::Pi() * atan( tangent );
  
  return angle;
}



double
vtkMRMLPerkStationModuleNode
::GetPlanDepth( int index )
{
  return GetActualPlanInsertionDepth( index );
}



double
vtkMRMLPerkStationModuleNode
::GetValidationAngle( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double entry[ 3 ];
  double target[ 3 ];
  
  this->GetValidationEntryPoint( entry, index );
  this->GetValidationTargetPoint( target, index );
  
  double tangent = ( entry[ 0 ] - target[ 0 ] ) / ( entry[ 1 ] - target[ 1 ] );
  double angle = - ( 180.0 / vtkMath::Pi() ) * atan( tangent );
  angle = angle + 90.0;
  
  return angle;
}



/**
 * @returns Euclidean distance between validation target and entry points.
 */
double
vtkMRMLPerkStationModuleNode
::GetValidationDepth( int index )
{
  if ( index == -1 )
    {
    index = this->GetCurrentPlanIndex();
    }
  
  double entry[ 3 ];
  double target[ 3 ];
  
  this->GetValidationEntryPoint( entry, index );
  this->GetValidationTargetPoint( target, index );
  
  double insDepth = sqrt( vtkMath::Distance2BetweenPoints( target, entry ) );
  
  return insDepth;
}



double
vtkMRMLPerkStationModuleNode
::GetDepthError( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double val = GetValidationDepth( index );
  double plan = GetActualPlanInsertionDepth( index );
  
  double diff = val - plan;
  return diff;
}



double
vtkMRMLPerkStationModuleNode
::GetAngleError( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double ep[ 3 ];  // Entry point in plan.
  double tp[ 3 ];  // Target point in plan.
  double ev[ 3 ];  // Entry point in validation.
  double tv[ 3 ];  // Target point in validation.
  
  this->GetPlanEntryPoint( ep, index );
  this->GetPlanTargetPoint( tp, index );
  this->GetValidationEntryPoint( ev, index );
  this->GetValidationTargetPoint( tv, index );
  
    // Bring the two lines' entry point to common origin, to get two vectors.
  
  double p[ 3 ];  // Plan.
  double v[ 3 ];  // Validation.
  
  for ( int i = 0; i < 3; ++ i )
    {
    p[ i ] = tp[ i ] - ep[ i ];
    v[ i ] = tv[ i ] - ev[ i ];
    }
  
    // Calculate the angle.
  
  double dotp = vtkMath::Dot( p, v );
  
  double lp = std::sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
  double lv = std::sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
  
  double angle = std::acos( dotp / lp / lv ) * 180 / 3.1415926;
  
  return angle;
}



double
vtkMRMLPerkStationModuleNode
::GetAngleErrorAxial( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double val = this->GetValidationAngle( index );
  double plan = this->GetActualPlanInsertionAngle( index );
  
  double diff = val - plan;
  return diff;
}



double
vtkMRMLPerkStationModuleNode
::GetAngleErrorSagittal( int index )
{
  if ( index == -1 ) index = this->GetCurrentPlanIndex();
  
  double ep[ 3 ];  // Entry point in plan.
  double tp[ 3 ];  // Target point in plan.
  double ev[ 3 ];  // Entry point in validation.
  double tv[ 3 ];  // Target point in validation.
  
  this->GetPlanEntryPoint( ep, index );
  this->GetPlanTargetPoint( tp, index );
  this->GetValidationEntryPoint( ev, index );
  this->GetValidationTargetPoint( tv, index );
  
    // Calculate sagittal angle.
  
  double tangentPlan = ( ep[ 1 ] - tp[ 1 ] ) / ( ep[ 2 ] - tp[ 2 ] );
  double anglePlan = std::atan( tangentPlan );
  double tangentValidation = ( ev[ 1 ] - tv[ 1 ] ) / ( ev[ 2 ] - tv[ 2 ] );
  double angleValidation = std::atan( tangentValidation );
  
  return ( angleValidation - anglePlan );
}



unsigned int
vtkMRMLPerkStationModuleNode
::AddPlan( vtkPerkStationPlan* newPlan )
{
  unsigned int index = this->PlanList.size();
  this->PlanList.push_back( newPlan );
  return index;
}



/**
 * @returns 0 on failure, other on success.
 */
int
vtkMRMLPerkStationModuleNode
::RemovePlanAtIndex( unsigned int index )
{
  if ( index >= this->PlanList.size() )
    {
    return 0;
    }
  
  /*  // Not for smart pointer.
  vtkPerkStationPlan* plan = GetPlanAtIndex( index );
  if ( plan != NULL )
  {
    plan->Delete();
  }
  */
  
  this->PlanList.erase( this->PlanList.begin() + index );
  return 1;
}



vtkPerkStationPlan*
vtkMRMLPerkStationModuleNode
::GetPlanAtIndex( unsigned int index )
{
  if ( index < this->PlanList.size() )
    {
    return this->PlanList[ index ].GetPointer();
    }
  else
    {
    return NULL;
    }
}



int
vtkMRMLPerkStationModuleNode
::SetCurrentPlanIndex( int index )
{
  if ( index >= (int)this->PlanList.size() )
    {
    return this->CurrentPlanIndex; // invalid index, do not change current
    }
  this->CurrentPlanIndex = index;
  
  this->Modified();
  return this->CurrentPlanIndex;
}



bool
vtkMRMLPerkStationModuleNode
::GetValidated() const
{
  if ( this->CurrentPlanIndex < 0 ) return false;
  return this->PlanList[ this->CurrentPlanIndex ]->GetValidated();
}



void
vtkMRMLPerkStationModuleNode
::SetValidated( const bool validated )
{
  if ( this->CurrentPlanIndex < 0 ) return;
  this->PlanList[ this->CurrentPlanIndex ]->SetValidated( validated );
}



int
vtkMRMLPerkStationModuleNode
::GetNumberOfValidations()
{
  int ret = 0;
  for ( int i = 0; i < this->GetNumberOfPlans(); ++ i )
    {
    if ( this->PlanList[ i ]->GetValidated() ) ++ ret;
    }
  return ret;
}



void
vtkMRMLPerkStationModuleNode
::GetValidationEntryPoint( double* point, int index ) const
{
  if ( index == -1 ) index = this->CurrentPlanIndex;
  
  if ( index < 0 || index > ( this->GetNumberOfPlans() - 1 ) )
    {
    point = NULL;
    return;
    }
  
  this->PlanList[ index ]->GetValidationEntryPointRAS( point );
}



void
vtkMRMLPerkStationModuleNode
::SetValidationEntryPoint( const double* point )
{
  if ( this->CurrentPlanIndex < 0 ) return;
  this->PlanList[ this->CurrentPlanIndex ]->SetValidationEntryPointRAS(
    point[ 0 ], point[ 1 ], point[ 2 ] );
}



void
vtkMRMLPerkStationModuleNode
::GetValidationTargetPoint( double* point, int index ) const
{
  if ( index == -1 ) index = this->CurrentPlanIndex;
  
  if ( index < 0 || index > ( this->GetNumberOfPlans() - 1 ) )
    {
    point = NULL;
    return;
    }
  
  this->PlanList[ index ]->GetValidationTargetPointRAS( point );
}



void
vtkMRMLPerkStationModuleNode
::SetValidationTargetPoint( const double* point )
{
  if ( this->CurrentPlanIndex < 0 ) return;
  this->PlanList[ this->CurrentPlanIndex ]->SetValidationTargetPointRAS(
    point[ 0 ], point[ 1 ], point[ 2 ] );
}



void
vtkMRMLPerkStationModuleNode
::UpdateHardwareListFile()
{
  
}
