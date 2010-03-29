
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
WriteBool( std::ostream& out, bool var, std::string name )
{
  out << " " << name << "=\"" << var << "\"" << std::endl;
}


void
WriteDouble( std::ostream& out, double var, std::string name )
{
  out << " " << name << "=\"" << var << "\"" << std::endl;
}


void
WriteDoubleVector( std::ostream& out, double* var, std::string name, int n )
{
  out << " " << name << "=\"";
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

// ----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode* vtkMRMLPerkStationModuleNode::New()
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
vtkMRMLNode* vtkMRMLPerkStationModuleNode::CreateNodeInstance()
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
  this->HideFromEditors = true;
  
  
  
  // Calibration parameters ---------------------------------------------------
  
  this->SecondMonitorRotation = 0.0;
  this->SecondMonitorRotationCenter[ 0 ] = 0.0;
  this->SecondMonitorRotationCenter[ 1 ] = 0.0;
  this->SecondMonitorTranslation[ 0 ] = 0.0;
  this->SecondMonitorTranslation[ 1 ] = 0.0;
  
  this->SecondMonitorVerticalFlip = false;
  this->SecondMonitorHorizontalFlip = false;
  
  this->TableAtScanner = 0.0;
  this->TableAtOverlay = 0.0;
  this->CurrentSliceOffset = 0.0;
  
  this->InitializeTransform();
  
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
  
  this->TimeSpentOnCalibrateStep = 0;
  this->TimeSpentOnPlanStep = 0;
  this->TimeSpentOnInsertStep = 0;
  this->TimeSpentOnValidateStep = 0;
  
  
  this->InitializeFiducialListNode();
  
    
    // Synchronize it with enum WORKPHASE...
  
  this->StepList = vtkSmartPointer< vtkStringArray >::New();
    this->StepList->InsertNextValue( "Calibration" );
    this->StepList->InsertNextValue( "Planning" );
    this->StepList->InsertNextValue( "Insertion" );
    this->StepList->InsertNextValue( "Validation" );
  
  this->CurrentStep = WORKPHASE_CALIBRATION;
  this->PreviousStep = WORKPHASE_CALIBRATION;
}


//----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode::~vtkMRMLPerkStationModuleNode()
{
   
}


//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::WriteXML(ostream& of, int nIndent)
{  
  Superclass::WriteXML(of, nIndent);
  
  // Write all MRML node attributes into output stream
  
  vtkIndent indent( nIndent );
  
  
  of << "\" \n";
  of << indent << " PlanningVolumeRef=\"" 
     << (this->PlanningVolumeRef ? this->PlanningVolumeRef: "NULL")
     << "\" \n";
  
  of << indent << " ValidationVolumeRef=\"" 
     << (this->ValidationVolumeRef ? this->ValidationVolumeRef: "NULL")
     << "\" \n";
  
  // Calibration step parameters
  
  // flip parameters
  of << indent << " SecondMonitorVerticalFlip=\"" 
     << this->SecondMonitorVerticalFlip   
     << "\" \n";
  
  of << indent << " SecondMonitorHorizontalFlip=\""
     << this->SecondMonitorHorizontalFlip
     << "\" \n";
  
  
  of << indent << " SecondMonitorTranslation=\"";
  for( int i = 0; i < 3; i++ )
      of << this->SecondMonitorTranslation[ i ] << " ";
  of << "\" \n";
  
  of << indent << " SecondMonitorRotation=\"" << this->SecondMonitorRotation
     << "\" \n";
  
  of << indent << " SecondMonitorRotationCenter=\"";
  for(int i = 0; i < 3; i++)
      of << this->SecondMonitorRotationCenter[ i ] << " ";
  of << "\" \n";
  
  
  // PLAN step parameters
  
  
  of << indent << " PlanEntryPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->PlanEntryPoint[ i ] << " ";
  of << "\" \n";
  
  of << indent << " PlanTargetPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->PlanTargetPoint[ i ] << " ";
  of << "\" \n";
  
  
  // Validate step parameters
  
  of << indent << " ValidateEntryPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->ValidateEntryPoint[i] << " ";
  of << "\" \n";

  of << indent << " ValidateTargetPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->ValidateTargetPoint[i] << " ";
  of << "\" \n";
  
  
    // Table calibration.
  
  of << indent << " TableAtScanner=\"" << this->TableAtScanner << "\" \n";
  of << indent << " TableAtOverlay=\"" << this->TableAtOverlay << "\" \n";
}


//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes( atts );

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  
  while ( *atts != NULL )
    {
    attName = *(atts++);
    attValue = *(atts++);
    
    if (!strcmp(attName, "PlanningVolumeRef"))
      {
      this->SetPlanningVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->PlanningVolumeRef, this);
      }
    else if (!strcmp(attName, "ValidationVolumeRef"))
      {
      this->SetValidationVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->ValidationVolumeRef, this);
      }
    else if (!strcmp(attName, "SecondMonitorVerticalFlip"))
      {
      std::stringstream ss;
      ss << attValue;
      bool val;
      ss >> val;
      this->SetSecondMonitorVerticalFlip( val );
      }
    else if (!strcmp(attName, "SecondMonitorHorizontalFlip"))
      {
      std::stringstream ss;
      ss << attValue;
      bool val;
      ss >> val;
      this->SetSecondMonitorHorizontalFlip( val );
      }
    else if (!strcmp(attName, "SecondMonitorTranslation"))
      {
       // read data into a temporary vector
      std::stringstream ss;
      ss << attValue;
      double d;
      std::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }
 
      if (tmpVec.size()!=3)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->SecondMonitorTranslation[ i ] = tmpVec[ i ];
      }
    else if ( ! strcmp( attName, "SecondMonitorRotation" ) )
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetSecondMonitorRotation( val );
      }
    else if ( ! strcmp( attName, "SecondMonitorRotationCenter" ) )
      {
       // read data into a temporary vector
      std::stringstream ss;
      ss << attValue;
      double d;
      std::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }
 
      if (tmpVec.size()!=3)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->SecondMonitorRotationCenter[ i ] = tmpVec[ i ];
      }
    else if (!strcmp(attName, "PlanEntryPoint"))
      {
       // read data into a temporary vector
      std::stringstream ss;
      ss << attValue;
      double d;
      std::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }
 
      if (tmpVec.size()!=3)
        {
        // error in file?
        }

      for ( unsigned int i = 0; i < tmpVec.size(); i++ )
           this->PlanEntryPoint[ i ] = tmpVec[ i ];
      }
    else if ( ! strcmp( attName, "PlanTargetPoint" ) )
      {
       // read data into a temporary vector
      std::stringstream ss;
      ss << attValue;
      double d;
      std::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }
 
      if ( tmpVec.size() != 3 )
        {
        // error in file?
        }

      for ( unsigned int i = 0; i < tmpVec.size(); i++ )
        this->PlanTargetPoint[ i ] = tmpVec[ i ];
      }
    else if (!strcmp(attName, "ValidateEntryPoint"))
      {
       // read data into a temporary vector
      std::stringstream ss;
      ss << attValue;
      double d;
      std::vector< double > tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }
 
      if (tmpVec.size()!=3)
        {
        // error in file?
        }

      for ( unsigned int i = 0; i < tmpVec.size(); i++ )
        this->ValidateEntryPoint[ i ] = tmpVec[ i ];
      }
    else if ( ! strcmp(attName, "ValidateTargetPoint"))
      {
       // read data into a temporary vector
      std::stringstream ss;
      ss << attValue;
      double d;
      std::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }
 
      if (tmpVec.size()!=3)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->ValidateTargetPoint[i] = tmpVec[i];
      }
    } // while ( *atts != NULL )
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLPerkStationModuleNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLPerkStationModuleNode *node = (vtkMRMLPerkStationModuleNode *) anode;

/*  this->SetConductance(node->Conductance);
  this->SetNumberOfIterations(node->NumberOfIterations);
  this->SetTimeStep(node->TimeStep);
  this->SetUseImageSpacing(node->UseImageSpacing);*/
  
  this->SetPlanningVolumeRef(node->PlanningVolumeRef);
  this->SetValidationVolumeRef(node->ValidationVolumeRef);
  this->SetPlanningVolumeNode(node->GetPlanningVolumeNode());
  this->SetValidationVolumeNode(node->GetValidationVolumeNode());
  
  
    // Work phases.
  
  this->StepList->Reset();
  for ( int i = 0; i > node->StepList->GetSize(); i ++ )
  {
    this->StepList->SetValue( i, node->StepList->GetValue( i ) );
  }
  
  this->CurrentStep = node->CurrentStep;
  this->PreviousStep = node->PreviousStep;
}


/**
 * Saves calibration data to a stream, as pairs of name="value".
 * @param out The output stream for saving.
 */
void
vtkMRMLPerkStationModuleNode
::SaveClibration( std::ostream& out )
{
  WriteDouble( out, this->SecondMonitorRotation, "SecondMonitorRotation" );
  WriteDoubleVector( out, this->SecondMonitorRotationCenter,
                     "SecondMonitorRotationCenter", 2 );
  WriteDoubleVector( out, this->SecondMonitorTranslation,
                     "SecondMonitorTranslation", 2 );
  WriteBool( out, this->SecondMonitorHorizontalFlip,
             "SecondMonitorHorizontalFlip" );
  WriteBool( out, this->SecondMonitorVerticalFlip,
             "SecondMonitorVerticalFlip" );
  WriteDouble( out, this->TableAtScanner, "TableAtScanner" );
  WriteDouble( out, this->TableAtOverlay, "TableAtOverlay" );
  WriteDouble( out, this->CurrentSliceOffset, "CurrentSliceOffset" );
}


/**
 * Reads calibration data.
 * @param in Stream to read calibration data from.
 * @returns true on success, false on error.
 */
bool
vtkMRMLPerkStationModuleNode
::LoadCalibration( std::istream& in )
{
  std::map< std::string, std::string > map = ReadAttributes( in );
  std::map< std::string, std::string >::iterator iter;
  
  bool nameNotFound = false;
  
  iter = map.find( "SecondMonitorRotation" );
  if ( iter != map.end() )
    StringToDouble( iter->second, this->SecondMonitorRotation );
  else nameNotFound = true;
  
  iter = map.find( "SecondMonitorRotationCenter" );
  if ( iter != map.end() )
    StringToDoubleVector( iter->second, this->SecondMonitorRotationCenter, 2 );
  else nameNotFound = true;
  
  iter = map.find( "SecondMonitorTranslation" );
  if ( iter != map.end() )
    StringToDoubleVector( iter->second, this->SecondMonitorTranslation, 2 );
  else nameNotFound = true;
  
  iter = map.find( "SecondMonitorHorizontalFlip" );
  if ( iter != map.end() )
    StringToBool( iter->second, this->SecondMonitorHorizontalFlip );
  else nameNotFound = true;
  
  iter = map.find( "SecondMonitorVerticalFlip" );
  if ( iter != map.end() )
    StringToBool( iter->second, this->SecondMonitorVerticalFlip );
  else nameNotFound = true;
  
  iter = map.find( "TableAtScanner" );
  if ( iter != map.end() )
    StringToDouble( iter->second, this->TableAtScanner );
  else nameNotFound = true;
  
  iter = map.find( "TableAtOverlay" );
  if ( iter != map.end() )
    StringToDouble( iter->second, this->TableAtOverlay );
  else nameNotFound = true;
  
  iter = map.find( "CurrentSliceOffset" );
  if ( iter != map.end() )
    StringToDouble( iter->second, this->CurrentSliceOffset );
  else nameNotFound = true;
  
  return ! nameNotFound;
}



void
vtkMRMLPerkStationModuleNode
::SaveExperiment( std::ostream& out )
{
  this->SaveClibration( out );
  
    // Plan.
  
  WriteDoubleVector( out, this->PlanEntryPoint, "PlanEntryPoint", 3 );
  WriteDoubleVector( out, this->PlanTargetPoint, "PlanTargetPoint", 3 );
  WriteDouble( out, this->TiltAngle, "TiltAngle" );
  
    // Validation.
  
  WriteDoubleVector( out, this->ValidateEntryPoint, "ValidateEntryPoint", 3 );
  WriteDoubleVector( out, this->ValidateTargetPoint, "ValidateTargetPoint", 3 );
}


bool
vtkMRMLPerkStationModuleNode
::LoadExperiment( std::istream& in )
{
  this->LoadCalibration( in );
  
  
  std::map< std::string, std::string > map = ReadAttributes( in );
  std::map< std::string, std::string >::iterator iter;
  
  bool nameNotFound = false;
  
  
    // Plan.
  
  iter = map.find( "PlanEntryPoint" );
  if ( iter != map.end() )
    StringToDoubleVector( iter->second, this->PlanEntryPoint, 3 );
  else nameNotFound = true;
  
  iter = map.find( "PlanTargetPoint" );
  if ( iter != map.end() )
    StringToDoubleVector( iter->second, this->PlanTargetPoint, 3 );
  else nameNotFound = true;
  
  iter = map.find( "TiltAngle" );
  if ( iter != map.end() )
    StringToDouble( iter->second, this->TiltAngle );
  else nameNotFound = true;
  
  
    // Validation.
  
  iter = map.find( "ValidateEntryPoint" );
  if ( iter != map.end() )
    StringToDoubleVector( iter->second, this->ValidateEntryPoint, 3 );
  else nameNotFound = true;
  
  iter = map.find( "ValidateTargetPoint" );
  if ( iter != map.end() )
    StringToDoubleVector( iter->second, this->ValidateTargetPoint, 3 );
  else nameNotFound = true;
  
  
  return ! nameNotFound;
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


/**
 * Create and initialize CalibrationMRMLTransformNode as an identity transform.
 */
void vtkMRMLPerkStationModuleNode::InitializeTransform()
{
    this->CalibrationMRMLTransformNode =
      vtkSmartPointer< vtkMRMLLinearTransformNode >::New();
    this->CalibrationMRMLTransformNode->
      SetName( "PerkStationCalibrationTransform" );
    this->CalibrationMRMLTransformNode->
      SetDescription( "Created by PERK Station Module" );

    vtkSmartPointer< vtkMatrix4x4 > matrix =
      vtkSmartPointer< vtkMatrix4x4 >::New();
    matrix->Identity();
    this->CalibrationMRMLTransformNode->ApplyTransform( matrix );
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
vtkMRMLScalarVolumeNode *vtkMRMLPerkStationModuleNode::GetActiveVolumeNode()
{
  if (strcmpi(this->VolumeInUse, "Planning") == 0)
    {
    return this->PlanningVolumeNode;
    }
  else if (strcmpi(this->VolumeInUse, "Validation") == 0)
    {
    return this->ValidationVolumeNode;
    }
  else
    {
    return NULL;
    }
}


vtkMRMLScalarVolumeNode*
vtkMRMLPerkStationModuleNode
::GetPlanningVolumeNode()
{
  return this->PlanningVolumeNode;
}

  
//-----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::SetPlanningVolumeNode(
  vtkMRMLScalarVolumeNode *planVolNode )
{
  vtkSetMRMLNodeMacro( this->PlanningVolumeNode, planVolNode );
}


//-----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::SetValidationVolumeNode(
  vtkMRMLScalarVolumeNode *validationVolNode )
{
  vtkSetMRMLNodeMacro(this->ValidationVolumeNode, validationVolNode);
}


//-----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::AddVolumeInformationToList(
  vtkMRMLScalarVolumeNode *volNode, const char *diskLocation, char *type)
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
    vtkErrorMacro( "VolumeNode is undefined" );
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
    // Direction positive if table position decreases towards the overlay.
  
  double tableDirection = 1.0;
  if ( this->TableAtScanner < this->TableAtOverlay )
    {
    tableDirection = - 1.0;
    }
  
  
    // Offset direction positive if the patient lies head-first,
    // because that is positive in RAS system.
  
  double offsetDirection = 1.0;
  if (    this->PatientPosition == FFDR
       || this->PatientPosition == FFDL
       || this->PatientPosition == FFP
       || this->PatientPosition == FFS )
    {
    offsetDirection = - 1.0;
    }
  
    
    // We know that:
    // TableAtScanner - TableAtOverlay = PatientAtScanner - PatientAtOverlay
    // Because both sides give the Overlay to Scanner vector.
  
  double patientAtOverlay = this->PatientAtScanner + this->TableAtOverlay
                            - this->TableAtScanner;
  
  
    // CurrentSliceOffset is in RAS.
  
  double directedOffset =  ( this->CurrentSliceOffset * offsetDirection
                             * tableDirection );
  
  
  return patientAtOverlay + directedOffset;
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
 * @returns Insertion angle in degrees, between anteroposterior line and plan.
 */
double
vtkMRMLPerkStationModuleNode
::GetActualPlanInsertionAngle()
{
  double insAngle =
    double( 180 / vtkMath::Pi() ) *
    atan( double( ( this->PlanEntryPoint[ 1 ] - this->PlanTargetPoint[ 1 ] ) /
                  ( this->PlanEntryPoint[ 0 ] - this->PlanTargetPoint[ 0 ] ) )
        );
  return insAngle;
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

