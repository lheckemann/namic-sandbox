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
