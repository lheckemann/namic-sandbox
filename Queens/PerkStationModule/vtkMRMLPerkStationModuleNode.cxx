
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


void
StringToInt( std::string str, int& var )
{
  std::stringstream ss( str );
  ss >> var;
}


void
StringToDouble( std::string str, double& var )
{
  std::stringstream ss( str );
  ss >> var;
}


void
StringToDoubleVector( std::string str, double* var, int n )
{
  std::stringstream ss( str );
  for ( int i = 0; i < n; ++ i )
    ss >> var[ i ];
}


//----------------------------------------------------------------------------
// Separate attName = Needle123_Length to a sectionInd=123 and sectionName=Length
bool GetAttNameSection( const std::string& attName,
                        const std::string& groupName,
                        unsigned int &sectionInd,
                        std::string &sectionName)
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



//-----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode*
vtkMRMLPerkStationModuleNode
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPerkStationModuleNode");
  if(ret)
    {
      return (vtkMRMLPerkStationModuleNode*)ret;
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
      return (vtkMRMLPerkStationModuleNode*)ret;
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
    // TODO: fill this list from a config file.
  
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
  
  
  // Calibration parameters ---------------------------------------------------
  
  this->CurrentCalibration = -1; // No calibration;
  
  this->CurrentSliceOffset = 0.0;
  
  this->PatientPosition = PPNA;
  
  
    // Plan parameters --------------------------------------------------------
  
  this->PlanEntryPoint[0] = 0.0;
  this->PlanEntryPoint[1] = 0.0;
  this->PlanEntryPoint[2] = 0.0;
  
  this->PlanTargetPoint[0] = 0.0;
  this->PlanTargetPoint[1] = 0.0;
  this->PlanTargetPoint[2] = 0.0;
  
  this->TiltAngle = 0;
  
  this->SliceToRAS = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  this->PlanList.clear();
  
  this->PlanUID = 0;
  
    // Insertion parameters ---------------------------------------------------
  
  this->TrackerToPhantomMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->PhantomToImageRASMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  this->ToolTipOffset[ 0 ] = 0.0;
  this->ToolTipOffset[ 1 ] = 0.0;
  this->ToolTipOffset[ 2 ] = 0.0;
  
  this->ReferenceBodyToolPort = 0;
  this->NeedleToolPort = 1;
  
  
    // Validation parameters --------------------------------------------------
  
  this->ValidateEntryPoint[ 0 ] = 0.0;
  this->ValidateEntryPoint[ 1 ] = 0.0;
  this->ValidateEntryPoint[ 2 ] = 0.0;
  
  this->ValidateTargetPoint[ 0 ] = 0.0;
  this->ValidateTargetPoint[ 1 ] = 0.0;
  this->ValidateTargetPoint[ 2 ] = 0.0;
  
  
    // Common parameters ------------------------------------------------------
  
  this->PlanningVolumeRef = NULL;
  this->ValidationVolumeRef = NULL;
  this->VolumeInUse = NULL;
  
  PlanningVolumeNode = NULL;
  ValidationVolumeNode = NULL;
  
  this->TimeOnCalibrateStep = 0.0;
  this->TimeOnPlanStep = 0.0;
  this->TimeOnInsertStep = 0.0;
  this->TimeOnValidateStep = 0.0;
  
  
  this->InitializeFiducialListNode();
  
    
    // Synchronize it with enum WORKPHASE...
  
  this->StepList = vtkSmartPointer< vtkStringArray >::New();
    this->StepList->InsertNextValue( "Calibration" );
    this->StepList->InsertNextValue( "Planning" );
    this->StepList->InsertNextValue( "Insertion" );
    this->StepList->InsertNextValue( "Validation" );
  
  this->CurrentStep = 0;
  this->PreviousStep = WORKPHASE_CALIBRATION;
}


//----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode::~vtkMRMLPerkStationModuleNode()
{
  for ( std::vector< vtkPerkStationPlan* >::iterator it = this->PlanList.begin();
        it != this->PlanList.end(); ++ it )
    {   
    if ((*it)!=NULL)
      {
      (*it)->Delete();
      (*it)=NULL;
      }
    }
  this->PlanList.clear();
}


//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::WriteXML(ostream& of, int nIndent)
{  
  Superclass::WriteXML(of, nIndent);
  of << std::endl;
  
  // Write all MRML node attributes into output stream
  
  vtkIndent indent( nIndent );
  
  
  // Calibration step parameters
  
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
    of << this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorTranslation[ 0 ] << " ";
    of << this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorTranslation[ 1 ] << "\"";
    of << std::endl;
    
    of << indent << "Calibration" << calInd << "_SecondMonitorRotation=\""
       << this->CalibrationList[ calInd ]->SecondMonitorRotation << "\"" << std::endl;
    
    of << indent << "Calibration" << calInd << "_SecondMonitorRotationCenter=\"";
    of << this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotationCenter[ 0 ] << " ";
    of << this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorRotationCenter[ 1 ] << "\"";
    of << std::endl;
    
    of << indent << "Calibration" << calInd << "_TableAtOverlay=\""
       << this->CalibrationList[ calInd ]->TableAtOverlay << "\"" << std::endl;
    }
  
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
  
  
    // Validation.
  
  WriteDoubleVector( of, indent, this->ValidateEntryPoint, "ValidateEntryPoint", 3 );
  WriteDoubleVector( of, indent, this->ValidateTargetPoint, "ValidateTargetPoint", 3 );
  
    // Common.
  
  WriteDouble( of, indent, this->TimeOnCalibrateStep, "TimeOnCalibrateStep" );
  WriteDouble( of, indent, this->TimeOnPlanStep, "TimeOnPlanStep" );
  WriteDouble( of, indent, this->TimeOnInsertStep, "TimeOnInsertStep" );
  WriteDouble( of, indent, this->TimeOnValidateStep, "TimeOnValidateStep" );
}


//----------------------------------------------------------------------------
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
    
    /*
    if ( ! strcmp( attName, "SecondMonitorVerticalFlip" ) ) {
      StringToBool( std::string( attValue ), this->SecondMonitorVerticalFlip );
      }
    else if ( ! strcmp( attName, "SecondMonitorHorizontalFlip" ) ) {
      StringToBool( std::string( attValue ), this->SecondMonitorHorizontalFlip );
      }
    else if ( ! strcmp( attName, "SecondMonitorTranslation" ) ) {
      StringToDoubleVector( std::string( attValue ), this->SecondMonitorTranslation, 2 );
      }
    else if ( ! strcmp( attName, "SecondMonitorRotation" ) ) {
      StringToDouble( std::string( attValue ), this->SecondMonitorRotation );
      }
    else if ( ! strcmp( attName, "SecondMonitorRotationCenter" ) ) {
      StringToDoubleVector( std::string( attValue ), this->SecondMonitorRotationCenter, 2 );
      }
    else if ( ! strcmp( attName, "TableAtOverlay" ) ) {
      StringToDouble( std::string( attValue ), this->TableAtOverlay );
      }
    */
    
    unsigned int sectionInd = 0;
    std::string sectionName;
    
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
        plan->SetPlanningVolumeRef( std::string( attValue ) );
        }
      
      }
    
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
      }
    } // while ( *atts != NULL )
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLPerkStationModuleNode::Copy(vtkMRMLNode *anode)
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
  if ( this->CurrentCalibration < 0 ) return false;
  return this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorVerticalFlip;
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
  if ( this->CurrentCalibration < 0 ) return false;
  return this->CalibrationList[ this->CurrentCalibration ]->SecondMonitorHorizontalFlip;
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
  if ( this->CurrentCalibration < 0 ) return 0.0;
  return this->CalibrationList[ this->CurrentCalibration ]->TableAtOverlay;
}


void
vtkMRMLPerkStationModuleNode
::SetTableAtOverlay( double pos )
{
  if ( this->CurrentCalibration < 0 ) return;
  this->CalibrationList[ this->CurrentCalibration ]->TableAtOverlay = pos;
}


std::vector< OverlayHardware >
vtkMRMLPerkStationModuleNode
::GetHardwareList()
{
  return this->HardwareList;
}


//----------------------------------------------------------------------------
void
vtkMRMLPerkStationModuleNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "PlanningVolumeRef: " << 
   ( this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)" ) << "\n";
 
}


//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::UpdateReferenceID( const char *oldID,
                                                      const char *newID )
{
  /*if (!strcmp(oldID, this->InputVolumeRef))
    {
    this->SetInputVolumeRef(newID);
    }*/
}


//-----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::InitializeFiducialListNode()
{
    this->PlanMRMLFiducialListNode =
      vtkSmartPointer< vtkMRMLFiducialListNode >::New();
    
    this->PlanMRMLFiducialListNode->SetName( "PerkStationFiducialList" );
    this->PlanMRMLFiducialListNode->SetDescription(
      "Created by PERK Station Module; marks entry point and target point" );
    this->PlanMRMLFiducialListNode->SetColor( 0.5, 1, 0.5 );
    this->PlanMRMLFiducialListNode->SetGlyphType(
      vtkMRMLFiducialListNode::Diamond3D );
    this->PlanMRMLFiducialListNode->SetOpacity( 0.7 );
    this->PlanMRMLFiducialListNode->SetAllFiducialsVisibility( true );
    this->PlanMRMLFiducialListNode->SetSymbolScale( 10 );
    this->PlanMRMLFiducialListNode->SetTextScale( 10 );
}


//-----------------------------------------------------------------------------
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


unsigned int
vtkMRMLPerkStationModuleNode
::AddCalibration( OverlayCalibration* newCalibration )
{
  unsigned int index = this->PlanList.size();
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
::AddCurrentPlan()
{
  std::stringstream ss;
    ss << "Plan" << this->PlanUID;
  ++ this->PlanUID;
  
  vtkSmartPointer< vtkPerkStationPlan > plan = vtkSmartPointer< vtkPerkStationPlan >::New();
    plan->SetName( ss.str() );
    plan->SetEntryPointRAS( this->PlanEntryPoint );
    plan->SetTargetPointRAS( this->PlanTargetPoint );
    plan->SetPlanningVolumeRef( std::string( this->PlanningVolumeRef ) );
  
  this->AddPlan( plan.GetPointer() );
}


vtkMRMLScalarVolumeNode*
vtkMRMLPerkStationModuleNode
::GetPlanningVolumeNode()
{
  return this->PlanningVolumeNode;
}

  
//-----------------------------------------------------------------------------
void
vtkMRMLPerkStationModuleNode
::SetPlanningVolumeNode( vtkMRMLScalarVolumeNode *planVolNode )
{
  if ( planVolNode != NULL )
    {
    vtkSetMRMLNodeMacro( this->PlanningVolumeNode, planVolNode );
    this->PlanningVolumeRef = this->PlanningVolumeNode->GetID();
    }
}


//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
void
vtkMRMLPerkStationModuleNode
::AddVolumeInformationToList( vtkMRMLScalarVolumeNode* volNode,
                              const char* diskLocation,
                              char* type )
{
  VolumeInformationStruct volStruct;
  volStruct.DiskLocation=diskLocation;
  volStruct.Type = type;
  volStruct.VolumeNode = volNode;
  volStruct.Active = false;
  this->VolumesList.push_back(volStruct);
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
  
  if ( this->CurrentStep == WORKPHASE_PLANNING )
    {
    this->PlanMRMLFiducialListNode->SetAllFiducialsVisibility( 1 );
    }
  else
    {
    this->PlanMRMLFiducialListNode->SetAllFiducialsVisibility( 0 );
    }
  
  
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
  
  return this->CalibrationList[ this->CurrentCalibration ]->TableAtOverlay + directedOffset;
}


/**
 * @returns Euclidean distance between plan and validation entry points.
 */
double
vtkMRMLPerkStationModuleNode
::GetEntryPointError()
{
  double sum = 0.0;
  
  for ( int i = 0; i < 3; ++ i )
    {
    sum += (   ( this->ValidateEntryPoint[ i ] - this->PlanEntryPoint[ i ] )
             * ( this->ValidateEntryPoint[ i ] - this->PlanEntryPoint[ i ] ) );
    }
  
  return std::sqrt( sum );
}


/**
 * @returns Euclidean distance between plan and validation target points.
 */
double
vtkMRMLPerkStationModuleNode
::GetTargetPointError()
{
  double sum = 0.0;
  
  for ( int i = 0; i < 3; ++ i )
    {
    sum += (   ( this->ValidateTargetPoint[ i ] - this->PlanTargetPoint[ i ] )
             * ( this->ValidateTargetPoint[ i ] - this->PlanTargetPoint[ i ] ) );
    }
  
  return std::sqrt( sum );
}


/**
 * Angle definition:
 * - : 0   degrees
 * / : 60  degrees
 * | : 90  degrees
 * \ : 120 degrees
 *
 * @returns Insertion angle in degrees, between anteroposterior line and plan.
 */
double
vtkMRMLPerkStationModuleNode
::GetActualPlanInsertionAngle()
{
  double tangent =   ( this->PlanEntryPoint[ 0 ] - this->PlanTargetPoint[ 0 ] )
                   / ( this->PlanEntryPoint[ 1 ] - this->PlanTargetPoint[ 1 ] );
  
  double angle = - ( 180.0 / vtkMath::Pi() ) * atan( tangent );
  
  angle = angle + 90.0;
  
  return angle;
}


/**
 * @returns Euclidean distance between plan target and entry points.
 */
double
vtkMRMLPerkStationModuleNode
::GetActualPlanInsertionDepth()
{
  double insDepth =
    sqrt( vtkMath::Distance2BetweenPoints( this->PlanTargetPoint,
                                           this->PlanEntryPoint ) );
  return insDepth;
}


/**
 * @returns Euclidean distance between validation target and entry points.
 */
double
vtkMRMLPerkStationModuleNode
::GetValidationDepth()
{
  double insDepth =
    sqrt( vtkMath::Distance2BetweenPoints( this->ValidateTargetPoint,
                                           this->ValidateEntryPoint ) );
  return insDepth;
}


unsigned int
vtkMRMLPerkStationModuleNode
::AddPlan( vtkPerkStationPlan* newPlan )
{
  unsigned int index = this->PlanList.size();
  newPlan->Register( this );
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
  vtkPerkStationPlan* plan = GetPlanAtIndex( index );
  if ( plan != NULL )
  {
    plan->Delete();
  }
  this->PlanList.erase( this->PlanList.begin() + index );
  return 1;
}


vtkPerkStationPlan*
vtkMRMLPerkStationModuleNode
::GetPlanAtIndex( unsigned int index )
{
  if ( index < this->PlanList.size() )
    {
    return this->PlanList[ index ];
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
  // this->InvokeEvent( vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent );
  return this->CurrentPlanIndex;
  
  this->PlanList[ this->CurrentPlanIndex ]->GetEntryPointRAS( this->PlanEntryPoint );
  this->PlanList[ this->CurrentPlanIndex ]->GetTargetPointRAS( this->PlanTargetPoint );
  this->PlanList[ this->CurrentPlanIndex ]->GetValidationEntryPointRAS( this->ValidateEntryPoint );
  this->PlanList[ this->CurrentPlanIndex ]->GetValidationTargetPointRAS( this->ValidateTargetPoint );
}
