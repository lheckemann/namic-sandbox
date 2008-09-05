/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLPerkStationModuleNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"

// for getting display device information
#include "Windows.h"
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

//----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode::vtkMRMLPerkStationModuleNode()
{
   this->InputVolumeRef = NULL;
   this->HideFromEditors = true;

   // member variables
   this->VerticalFlip = false;
   this->HorizontalFlip = false;

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

   this->UserPlanInsertionDepth = 0;
   this->ActualPlanInsertionDepth = 0;

   this->ValidateEntryPoint[0] = 0.0;
   this->ValidateEntryPoint[1] = 0.0;
   this->ValidateEntryPoint[2] = 0.0;

   this->ValidateTargetPoint[0] = 0.0;
   this->ValidateTargetPoint[1] = 0.0;
   this->ValidateTargetPoint[2] = 0.0;

   this->CalibrateTranslationError[0] = 0;
   this->CalibrateTranslationError[1] = 0;

   this->CalibrateScaleError[0] = 0.0;
   this->CalibrateScaleError[1] = 0.0;

   this->CalibrateRotationError = 0;

   this->PlanInsertionAngleError = 0;
   this->PlanInsertionDepthError = 0;

   this->EntryPointError = 0;
   this->TargetPointError = 0;

   this->InitializeTransform();

   this->InitializeFiducialListNode();
}

//----------------------------------------------------------------------------
vtkMRMLPerkStationModuleNode::~vtkMRMLPerkStationModuleNode()
{
   this->SetInputVolumeRef( NULL );
}

//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::WriteXML(ostream& of, int nIndent)
{  
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream

  vtkIndent indent(nIndent);

  of << indent << " InputVolumeRef=\"" 
     << (this->InputVolumeRef ? this->InputVolumeRef: "NULL")
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



}

//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "InputVolumeRef"))
      {
      this->SetInputVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->InputVolumeRef, this);
      }
    else if (!strcmp(attName, "VerticalFlip"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      bool val;
      ss >> val;
      this->SetVerticalFlip(val);
      }
    else if (!strcmp(attName, "HorizontalFlip"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      bool val;
      ss >> val;
      this->SetHorizontalFlip(val);
      }
    else if (!strcmp(attName, "UserScaling"))
      {
       // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetUserRotation(val);
      }
    else if (!strcmp(attName, "ActualRotation"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetActualRotation(val);
      }
    else if (!strcmp(attName, "CenterOfRotation"))
      {
       // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetUserPlanInsertionAngle(val);
      }
    else if (!strcmp(attName, "ActualPlanInsertionAngle"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetActualPlanInsertionAngle(val);
      }
    else if (!strcmp(attName, "UserPlanInsertionDepth"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetUserPlanInsertionDepth(val);
      }
    else if (!strcmp(attName, "ActualPlanInsertionDepth"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->SetActualPlanInsertionDepth(val);
      }
    else if (!strcmp(attName, "ValidateEntryPoint"))
      {
       // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double d;
      vtksys_stl::vector<double> tmpVec;
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
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->CalibrateRotationError = val;
      }
    else if (!strcmp(attName, "PlanInsertionAngleError"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->PlanInsertionAngleError = val;
      }
    else if (!strcmp(attName, "PlanInsertionDepthError"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->PlanInsertionDepthError = val;
      }
    else if (!strcmp(attName, "EntryPointError"))
      {
      vtksys_stl::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->EntryPointError = val;
      }
    else if (!strcmp(attName, "TargetPointError"))
      {
      vtksys_stl::stringstream ss;
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
  this->SetInputVolumeRef(node->InputVolumeRef);

}

//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "InputVolumeRef:   " << 
   (this->InputVolumeRef ? this->InputVolumeRef : "(none)") << "\n";
 
}

//----------------------------------------------------------------------------
void vtkMRMLPerkStationModuleNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  if (!strcmp(oldID, this->InputVolumeRef))
    {
    this->SetInputVolumeRef(newID);
    }
}
//-----------------------------------------------------------------------------
int vtkMRMLPerkStationModuleNode::GetSecondaryMonitorSpacing(double & xSpacing, double & ySpacing)
{
  int retValue = -1; // indicates no secondary monitor connected
  DISPLAY_DEVICE lpDisplayDevice;
  lpDisplayDevice.cb = sizeof(lpDisplayDevice);
  lpDisplayDevice.StateFlags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
  int iDevNum = 0;
  int iPhyDevNum = 0;
  DWORD dwFlags = 0;
  while(EnumDisplayDevices(NULL, iDevNum, &lpDisplayDevice, dwFlags))
    {
    iDevNum++;
    if( (lpDisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 1)
      {
      iPhyDevNum++;
      // get the device context for the monitor
      HDC hdc = CreateDC(NULL, lpDisplayDevice.DeviceName, NULL, NULL);
      // now the device context can be used in many functions to retrieve information about the monitor
      int width_mm = GetDeviceCaps(hdc, HORZSIZE);
      int height_mm = GetDeviceCaps(hdc, VERTSIZE);
     
      int width_pix = GetDeviceCaps(hdc, HORZRES);
      int height_pix = GetDeviceCaps(hdc, VERTRES);
      xSpacing = double(width_mm)/double(width_pix);
      ySpacing = double(height_mm)/double(height_pix);

      if (iPhyDevNum == 2)
        retValue = 1;
      }
    }

  
  return retValue;  

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
    //matrix->Delete();      
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

    this->PlanMRMLFiducialListNode->SetName("PerkStationFiducialList");
    this->PlanMRMLFiducialListNode->SetDescription("Created by PERK Station Module; marks entry point and target point");
    this->PlanMRMLFiducialListNode->SetColor(0.5,0.5,0.5);
    this->PlanMRMLFiducialListNode->SetGlyphType(vtkMRMLFiducialListNode::Diamond3D);
    this->PlanMRMLFiducialListNode->SetOpacity(0.7);
    this->PlanMRMLFiducialListNode->SetAllFiducialsVisibility(true);
    this->PlanMRMLFiducialListNode->SetSymbolScale(20);
    this->PlanMRMLFiducialListNode->SetTextScale(10);
}

/*


void vtkMRMLPerkStationModuleNode::SetTransformNodeMatrix(vtkMatrix4x4 *matrix)
{
  this->CalibrationMRMLTransformNode->ApplyTransform(matrix);
}*/
