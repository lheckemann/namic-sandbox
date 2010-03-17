
#include "vtkMRMLPerkStationModuleNode.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "itkSpatialOrientationAdapter.h"

#include "vtkObjectFactory.h"
#include "vtkStringArray.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLScalarVolumeNode.h"

#include "vtkSlicerLogic.h"


//------------------------------------------------------------------------------
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
  this->PlanningVolumeRef = NULL;
  this->ValidationVolumeRef = NULL;
  this->PlanningVolumeNode = NULL;
  this->ValidationVolumeNode = NULL;
  this->VolumeInUse = NULL;

  this->HideFromEditors = true;

  // member variables
  this->VerticalFlip = false;
  this->HorizontalFlip = false;
  
  
  
  this->ClinicalModeRotation = 0.0;
  this->ClinicalModeTranslation[0] = 0.0;
  this->ClinicalModeTranslation[1] = 0.0;

  this->UserScaling[0] = 1.0;
  this->UserScaling[1] = 1.0;
  this->UserScaling[2] = 1.0;

  this->ActualScaling[0] = 1.0;
  this->ActualScaling[1] = 1.0;
  this->ActualScaling[2] = 1.0;

  this->UserTranslation[0] = 0;
  this->UserTranslation[1] = 0;
  this->UserTranslation[2] = 0;

  this->ActualTranslation[0] = 0;
  this->ActualTranslation[1] = 0;
  this->ActualTranslation[2] = 0;

  this->UserRotation = 0;
  this->ActualRotation = 0;
  this->CenterOfRotation[0] = 0.0;
  this->CenterOfRotation[1] = 0.0;
  this->CenterOfRotation[2] = 0.0;

  this->PlanEntryPoint[0] = 0.0;
  this->PlanEntryPoint[1] = 0.0;
  this->PlanEntryPoint[2] = 0.0;

  this->PlanTargetPoint[0] = 0.0;
  this->PlanTargetPoint[1] = 0.0;
  this->PlanTargetPoint[2] = 0.0;

  this->UserPlanInsertionAngle = 0;
  this->ActualPlanInsertionAngle = 0;

  this->TiltAngle = 0;
  this->OriginalSliceToRAS = vtkMatrix4x4::New();
  this->TiltSliceToRAS = vtkMatrix4x4::New();

  this->UserPlanInsertionDepth = 0;
  this->ActualPlanInsertionDepth = 0;
  this->TrackerToPhantomMatrix = vtkMatrix4x4::New();
  this->PhantomToImageRASMatrix = vtkMatrix4x4::New();
  this->ToolTipOffset[0] = 0.0;
  this->ToolTipOffset[1] = 0.0;
  this->ToolTipOffset[2] = 0.0;

  this->ReferenceBodyToolPort = 0;
  this->NeedleToolPort = 1;

  this->ValidateEntryPoint[0] = 0.0;
  this->ValidateEntryPoint[1] = 0.0;
  this->ValidateEntryPoint[2] = 0.0;

  this->ValidateTargetPoint[0] = 0.0;
  this->ValidateTargetPoint[1] = 0.0;
  this->ValidateTargetPoint[2] = 0.0;

  this->ValidateInsertionDepth = 0.0;


  this->CalibrateTranslationError[0] = 0;
  this->CalibrateTranslationError[1] = 0;

  this->CalibrateScaleError[0] = 0.0;
  this->CalibrateScaleError[1] = 0.0;

  this->CalibrateRotationError = 0;

  this->PlanInsertionAngleError = 0;
  this->PlanInsertionDepthError = 0;

  this->EntryPointError = 0;
  this->TargetPointError = 0;

  this->TimeSpentOnCalibrateStep = 0;
  this->TimeSpentOnPlanStep = 0;
  this->TimeSpentOnInsertStep = 0;
  this->TimeSpentOnValidateStep = 0;


  this->InitializeTransform();
  this->InitializeFiducialListNode();
  
  this->StepList = vtkStringArray::New();
    this->StepList->InsertNextValue( "Calibration" );
    this->StepList->InsertNextValue( "Planning" );
    this->StepList->InsertNextValue( "Insertion" );
    this->StepList->InsertNextValue( "Validation" );
    this->StepList->InsertNextValue( "Evaluation" );
  this->CurrentStep = 0;
  this->PreviousStep = 0;
  
  
    // Patient position.
  this->m_PatientPosition = PPNA;
  
  this->TableAtScanner = 0.0;
  this->TableAtOverlay = 0.0;
  this->CurrentSliceOffset = 0.0;
}


//----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode::~vtkMRMLPerkStationModuleNode()
{
   this->OriginalSliceToRAS->Delete();
   this->TiltSliceToRAS->Delete();
   this->TrackerToPhantomMatrix->Delete();
   this->PhantomToImageRASMatrix->Delete();

   this->SetPlanningVolumeRef(NULL);
   this->SetValidationVolumeRef(NULL);
   this->SetPlanningVolumeNode(NULL);
   this->SetValidationVolumeNode(NULL);
   this->SetVolumeInUse(NULL);
   if(this->CalibrationMRMLTransformNode)
     {
     this->CalibrationMRMLTransformNode->Delete();
     this->CalibrationMRMLTransformNode = NULL;
     }
   if(this->PlanMRMLFiducialListNode)
     {
     this->PlanMRMLFiducialListNode->Delete();
     this->PlanMRMLFiducialListNode = NULL;
     }
//   vtkSetMRMLNodeMacro (this->CalibrationMRMLTransformNode, NULL);
//   vtkSetMRMLNodeMacro (this->PlanMRMLFiducialListNode, NULL);  
  
  this->StepList->Delete();
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
  of << indent << " VerticalFlip=\"" << this->VerticalFlip   
     << "\" \n";
  
  of << indent << " HorizontalFlip=\"" << this->HorizontalFlip   
     << "\" \n";
  
  // scale parameters
  of << indent << " UserScaling=\"";
  for(int i = 0; i < 2; i++)
      of << this->UserScaling[i] << " ";
  of << "\" \n";
  
  of << indent << " ActualScaling=\"";
  for(int i = 0; i < 2; i++)
      of << this->ActualScaling[i] << " ";
  of << "\" \n";
  
  // translation parameters
  of << indent << " UserTranslation=\"";
  for(int i = 0; i < 3; i++)
      of << this->UserTranslation[i] << " ";
  of << "\" \n";
  
  of << indent << " ActualTranslation=\"";
  for(int i = 0; i < 3; i++)
      of << this->ActualTranslation[i] << " ";
  of << "\" \n";
  
  // rotation parameters
  of << indent << " UserRotation=\"" << this->UserRotation
     << "\" \n";
  
  of << indent << " ActualRotation=\"" << this->ActualRotation
     << "\" \n";
  
  of << indent << " CenterOfRotation=\"";
  for(int i = 0; i < 3; i++)
      of << this->CenterOfRotation[i] << " ";
  of << "\" \n";
  
  
  // PLAN step parameters
  
  
  of << indent << " PlanEntryPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->PlanEntryPoint[i] << " ";
  of << "\" \n";
  
  of << indent << " PlanTargetPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->PlanTargetPoint[i] << " ";
  of << "\" \n";
  
  of << indent << " UserPlanInsertionAngle=\"" << this->UserPlanInsertionAngle
     << "\" \n";
  
  of << indent << " ActualPlanInsertionAngle=\"" << this->ActualPlanInsertionAngle
     << "\" \n";
  
  of << indent << " UserPlanInsertionDepth=\"" << this->UserPlanInsertionDepth
     << "\" \n";
  
  of << indent << " ActualPlanInsertionDepth=\"" << this->ActualPlanInsertionDepth
     << "\" \n";
  
  
  
  // Validate step parameters
  
  of << indent << " ValidateEntryPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->ValidateEntryPoint[i] << " ";
  of << "\" \n";

  of << indent << " ValidateTargetPoint=\"";
  for(int i = 0; i < 3; i++)
      of << this->ValidateTargetPoint[i] << " ";
  of << "\" \n";


  // Evaluate step parameters

  of << indent << " CalibrateScaleError=\"";
  for(int i = 0; i < 2; i++)
      of << this->CalibrateScaleError[i] << " ";
  of << "\" \n";

  of << indent << " CalibrateTranslationError=\"";
  for(int i = 0; i < 2; i++)
      of << this->CalibrateTranslationError[i] << " ";
  of << "\" \n";

  of << indent << " CalibrateRotationError=\"" << this->CalibrateRotationError
     << "\" \n";

  of << indent << " PlanInsertionAngleError=\"" << this->PlanInsertionAngleError
     << "\" \n";

  of << indent << " PlanInsertionDepthError=\"" << this->PlanInsertionDepthError
     << "\" \n";

  of << indent << " EntryPointError=\"" << this->EntryPointError
     << "\" \n";

  of << indent << " TargetPointError=\"" << this->TargetPointError
     << "\" \n";
  
  
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
    else if (!strcmp(attName, "VerticalFlip"))
      {
      std::stringstream ss;
      ss << attValue;
      bool val;
      ss >> val;
      this->SetVerticalFlip(val);
      }
    else if (!strcmp(attName, "HorizontalFlip"))
      {
      std::stringstream ss;
      ss << attValue;
      bool val;
      ss >> val;
      this->SetHorizontalFlip(val);
      }
    else if (!strcmp(attName, "UserScaling"))
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
      if (tmpVec.size()!=2)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->UserScaling[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "ActualScaling"))
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

      if (tmpVec.size()!=2)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->ActualScaling[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "UserTranslation"))
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
           this->UserTranslation[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "ActualTranslation"))
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
           this->ActualScaling[i] = tmpVec[i];
      }



    else if (!strcmp(attName, "UserRotation"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetUserRotation(val);
      }
    else if (!strcmp(attName, "ActualRotation"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetActualRotation(val);
      }
    else if (!strcmp(attName, "CenterOfRotation"))
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
           this->CenterOfRotation[i] = tmpVec[i];
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

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->PlanEntryPoint[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "PlanTargetPoint"))
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
           this->PlanTargetPoint[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "UserPlanInsertionAngle"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetUserPlanInsertionAngle(val);
      }
    else if (!strcmp(attName, "ActualPlanInsertionAngle"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetActualPlanInsertionAngle(val);
      }
    else if (!strcmp(attName, "UserPlanInsertionDepth"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetUserPlanInsertionDepth(val);
      }
    else if (!strcmp(attName, "ActualPlanInsertionDepth"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetActualPlanInsertionDepth(val);
      }
    else if (!strcmp(attName, "ValidateEntryPoint"))
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
           this->ValidateEntryPoint[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "ValidateTargetPoint"))
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
    else if (!strcmp(attName, "CalibrateScaleError"))
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
      if (tmpVec.size()!=2)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->CalibrateScaleError[i] = tmpVec[i];
      }
    else if (!strcmp(attName, "CalibrateTranslationError"))
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
      if (tmpVec.size()!=2)
        {
        // error in file?
        }

      for (unsigned int i = 0; i < tmpVec.size(); i++)
           this->CalibrateTranslationError[i] = tmpVec[i];
      }

    else if (!strcmp(attName, "CalibrateRotationError"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->CalibrateRotationError = val;
      }
    else if (!strcmp(attName, "PlanInsertionAngleError"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->PlanInsertionAngleError = val;
      }
    else if (!strcmp(attName, "PlanInsertionDepthError"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->PlanInsertionDepthError = val;
      }
    else if (!strcmp(attName, "EntryPointError"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->EntryPointError = val;
      }
    else if (!strcmp(attName, "TargetPointError"))
      {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->TargetPointError = val;
      } 
     
    }
  
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

//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "PlanningVolumeRef:   " << 
   (this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)") << "\n";
 
}

//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  /*if (!strcmp(oldID, this->InputVolumeRef))
    {
    this->SetInputVolumeRef(newID);
    }*/
}
//------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculateCalibrateScaleError()
{
  this->CalibrateScaleError[0] = 100*(this->ActualScaling[0]-this->UserScaling[0])/this->ActualScaling[0];
  this->CalibrateScaleError[1] = 100*(this->ActualScaling[1]-this->UserScaling[1])/this->ActualScaling[1];
}
//------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculateCalibrateTranslationError()
{
  this->CalibrateTranslationError[0] = this->ActualTranslation[0] - this->UserTranslation[0];
  this->CalibrateTranslationError[1] = this->ActualTranslation[1] - this->UserTranslation[1];
}
//------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculateCalibrateRotationError()
{
  this->CalibrateRotationError = this->ActualRotation - this->UserRotation;
}
//------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculatePlanInsertionAngleError()
{
  this->PlanInsertionAngleError = this->ActualPlanInsertionAngle - this->UserPlanInsertionAngle;
}
//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculatePlanInsertionDepthError()
{ 
  this->PlanInsertionDepthError = this->ActualPlanInsertionDepth - this->UserPlanInsertionDepth;
}
//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculateEntryPointError()
{
  this->EntryPointError = sqrt( (this->PlanEntryPoint[0]-this->ValidateEntryPoint[0])*(this->PlanEntryPoint[0]-this->ValidateEntryPoint[0]) + (this->PlanEntryPoint[1]-this->ValidateEntryPoint[1])*(this->PlanEntryPoint[1]-this->ValidateEntryPoint[1]));
}
//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::CalculateTargetPointError()
{
  this->TargetPointError = sqrt( (this->PlanTargetPoint[0]-this->ValidateTargetPoint[0])*(this->PlanTargetPoint[0]-this->ValidateTargetPoint[0]) + (this->PlanTargetPoint[1]-this->ValidateTargetPoint[1])*(this->PlanTargetPoint[1]-this->ValidateTargetPoint[1]));
}

//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::InitializeTransform()
{
    this->CalibrationMRMLTransformNode = vtkMRMLLinearTransformNode::New();
    this->CalibrationMRMLTransformNode->SetName("PerkStationCalibrationTransform");
    this->CalibrationMRMLTransformNode->SetDescription("Created by PERK Station Module");

    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity();
    this->CalibrationMRMLTransformNode->ApplyTransform(matrix);
    matrix->Delete();      
}
//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::InitializeFiducialListNode()
{
    // create node
    this->PlanMRMLFiducialListNode = vtkMRMLFiducialListNode::New();
    if (this->PlanMRMLFiducialListNode == NULL)
        {
        // error macro      
        }   
    //this->PlanMRMLFiducialListNode->SetLocked(true);
    this->PlanMRMLFiducialListNode->SetName("PerkStationFiducialList");
    this->PlanMRMLFiducialListNode->SetDescription("Created by PERK Station Module; marks entry point and target point");
    this->PlanMRMLFiducialListNode->SetColor(0.5,1,0.5);
    this->PlanMRMLFiducialListNode->SetGlyphType(vtkMRMLFiducialListNode::Diamond3D);
    this->PlanMRMLFiducialListNode->SetOpacity(0.7);
    this->PlanMRMLFiducialListNode->SetAllFiducialsVisibility(true);
    this->PlanMRMLFiducialListNode->SetSymbolScale(10);
    this->PlanMRMLFiducialListNode->SetTextScale(10);
}
//-------------------------------------------------------------------------------
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


//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::SetPlanningVolumeNode(vtkMRMLScalarVolumeNode *planVolNode)
{
  vtkSetMRMLNodeMacro( this->PlanningVolumeNode, planVolNode );
}

//-------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::SetValidationVolumeNode(vtkMRMLScalarVolumeNode *validationVolNode)
{
  vtkSetMRMLNodeMacro(this->ValidationVolumeNode, validationVolNode);
}

//------------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::AddVolumeInformationToList(vtkMRMLScalarVolumeNode *volNode, const char *diskLocation, char *type)
{
  VolumeInformationStruct volStruct;
  volStruct.DiskLocation=diskLocation;
  volStruct.Type = type;
  volStruct.VolumeNode = volNode;
  volStruct.Active = false;
  this->VolumesList.push_back(volStruct);
}
/*


void vtkMRMLPerkStationModuleNode::SetTransformNodeMatrix(vtkMatrix4x4 *matrix)
{
  this->CalibrationMRMLTransformNode->ApplyTransform(matrix);
}*/


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
PatientPosition
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
  
  if ( tagValueTrimmed == "HFP" ) this->m_PatientPosition = HFP;
  else if ( tagValueTrimmed == "HFS" ) this->m_PatientPosition = HFS;
  else if ( tagValueTrimmed == "HFDR" ) this->m_PatientPosition = HFDR;
  else if ( tagValueTrimmed == "HFDL" ) this->m_PatientPosition = HFDL;
  else if ( tagValueTrimmed == "FFDR" ) this->m_PatientPosition = FFDR;
  else if ( tagValueTrimmed == "FFDL" ) this->m_PatientPosition = FFDL;
  else if ( tagValueTrimmed == "FFP" ) this->m_PatientPosition = FFP;
  else if ( tagValueTrimmed == "FFS" ) this->m_PatientPosition = FFS;
  
  return this->m_PatientPosition;
}


/**
 * @param offset New slice offset in RAS coordinates.
 */
void
vtkMRMLPerkStationModuleNode
::SetCurrentSliceOffset( double offset )
{
  this->CurrentSliceOffset = offset;
}


double
vtkMRMLPerkStationModuleNode
::GetCurrentTablePosition()
{
  return this->TableAtOverlay + this->CurrentSliceOffset;
}

