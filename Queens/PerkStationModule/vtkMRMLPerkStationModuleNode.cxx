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

  /*{
    std::stringstream ss;
    ss << this->Conductance;
    of << indent << " Conductance=\"" << ss.str() << "\"";
  }*/
  {
    std::stringstream ss;
    if ( this->InputVolumeRef )
      {
      ss << this->InputVolumeRef;
      of << indent << " InputVolumeRef=\"" << ss.str() << "\"";
     }
  }
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
    /*if (!strcmp(attName, "Conductance")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> this->Conductance;
      }
    else*/ if (!strcmp(attName, "InputVolumeRef"))
      {
      this->SetInputVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->InputVolumeRef, this);
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
/*
void vtkMRMLPerkStationModuleNode::SetTransformNodeMatrix(vtkMatrix4x4 *matrix)
{
  this->CalibrationMRMLTransformNode->ApplyTransform(matrix);
}*/
