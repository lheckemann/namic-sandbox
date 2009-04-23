/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLTRProstateBiopsyModuleNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkMRMLTRProstateBiopsyModuleNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLFiducial.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkSlicerLogic.h"



/*
vtkCxxSetObjectMacro(vtkMRMLTRProstateBiopsyModuleNode, CalibrationFiducialListNode,
                     vtkMRMLFiducialListNode);
vtkCxxSetObjectMacro(vtkMRMLTRProstateBiopsyModuleNode, TargetFiducialListNode,
                     vtkMRMLFiducialListNode);
*/

//------------------------------------------------------------------------------
vtkMRMLTRProstateBiopsyModuleNode* vtkMRMLTRProstateBiopsyModuleNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTRProstateBiopsyModuleNode");
  if(ret)
    {
      return (vtkMRMLTRProstateBiopsyModuleNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTRProstateBiopsyModuleNode;
}

//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLTRProstateBiopsyModuleNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTRProstateBiopsyModuleNode");
  if(ret)
    {
      return (vtkMRMLTRProstateBiopsyModuleNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTRProstateBiopsyModuleNode;
}

//----------------------------------------------------------------------------
vtkMRMLTRProstateBiopsyModuleNode::vtkMRMLTRProstateBiopsyModuleNode()
{
   
  this->VolumeInUse = NULL;
  this->SegmentationOutputVolumeRef = NULL;
  this->SegmentationInputVolumeRef = NULL;
  this->SegmentationSeedVolumeRef = NULL;
  this->CalibrationFiducialsList   = NULL;  
  this->ExperimentSavePathInConfigFile = NULL;  
 
  this->AxesDistance = -1;
  this->AxesAngleDegrees = 0;
  this->RobotRegistrationAngleDegrees = 0;
  this->SetI1(0.0,0.0,0.0);
  this->SetI2(0.0,0.0,0.0);
  this->Setv1(0.0,0.0,0.0);
  this->Setv2(0.0,0.0,0.0);
  this->CalibrationVolumeNode = NULL;
  this->TargetingVolumeNode = NULL;
  this->VerificationVolumeNode = NULL;
  this->FiducialSegmentationRegistrationComplete = false;

  this->HideFromEditors = true;
  
  // initialize at least one needle
  this->NumberOfNeedles = 1;
  this->NeedlesVector.resize(this->NumberOfNeedles); 
  NeedleDescriptorStruct *needle = new NeedleDescriptorStruct;
  needle->Description = "Biopsy needle";
  needle->NeedleDepth = -13;
  needle->NeedleOvershoot = 15;
  needle->NeedleType = "Biopsy";
  needle->UniqueIdentifier = 1;
  this->NeedlesVector[0] = needle;
    
  this->TargetingFiducialsListsNames.clear();
  this->TargetingFiducialsListsVector.clear();
  this->TargetDescriptorsVector.clear();
  this->NeedlesVector.clear();
  

  this->InitializeFiducialListNode();
}

//----------------------------------------------------------------------------
vtkMRMLTRProstateBiopsyModuleNode::~vtkMRMLTRProstateBiopsyModuleNode()
{
  this->SetVolumeInUse(NULL);

  this->SetCalibrationVolumeNode(NULL);
  this->SetTargetingVolumeNode(NULL);
  this->SetVerificationVolumeNode(NULL);
  this->SetExperimentSavePathInConfigFile(NULL);
  this->CalibrationFiducialsList = NULL;

  this->AxesDistance = -1;
  this->AxesAngleDegrees = 0;
  this->RobotRegistrationAngleDegrees = 0;
  this->SetI1(0.0,0.0,0.0);
  this->SetI2(0.0,0.0,0.0);
  this->Setv1(0.0,0.0,0.0);
  this->Setv2(0.0,0.0,0.0);
  this->CalibrationVolumeNode = NULL;
  this->TargetingVolumeNode = NULL;
  this->VerificationVolumeNode = NULL;
  this->FiducialSegmentationRegistrationComplete = false;

  this->HideFromEditors = true;
  
  this->NumberOfNeedles = 0;
  this->TargetingFiducialsListsNames.clear();
  this->TargetingFiducialsListsVector.clear();
  this->TargetDescriptorsVector.clear();
  this->NeedlesVector.clear();
  

}

//----------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::WriteXML(ostream& of, int nIndent)
{  
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream

  vtkIndent indent(nIndent);

  /*
  of << "\" \n";
  of << indent << " PlanningVolumeRef=\"" 
     << (this->PlanningVolumeRef ? this->PlanningVolumeRef: "NULL")
     << "\" \n";
  
  of << indent << " ValidationVolumeRef=\"" 
     << (this->ValidationVolumeRef ? this->ValidationVolumeRef: "NULL")
     << "\" \n";

  */

}

//----------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
  /*  if (!strcmp(attName, "PlanningVolumeRef"))
      {
      this->SetPlanningVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->PlanningVolumeRef, this);
      }
    else if (!strcmp(attName, "ValidationVolumeRef"))
      {
      this->SetValidationVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->ValidationVolumeRef, this);
      }
      */
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLTRProstateBiopsyModuleNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLTRProstateBiopsyModuleNode *node = (vtkMRMLTRProstateBiopsyModuleNode *) anode;

/*  this->SetConductance(node->Conductance);
  this->SetNumberOfIterations(node->NumberOfIterations);
  this->SetTimeStep(node->TimeStep);
  this->SetUseImageSpacing(node->UseImageSpacing);*/
  
  this->SetCalibrationVolumeNode(node->GetCalibrationVolumeNode());
  this->SetTargetingVolumeNode(node->GetTargetingVolumeNode());
  this->SetVerificationVolumeNode(node->GetVerificationVolumeNode());

}

//----------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMRMLNode::PrintSelf(os,indent);

  /*
  os << indent << "PlanningVolumeRef:   " << 
   (this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)") << "\n";
 */
}

//----------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  /*if (!strcmp(oldID, this->InputVolumeRef))
    {
    this->SetInputVolumeRef(newID);
    }*/
}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::InitializeFiducialListNode()
{
    // create node
    this->CalibrationFiducialsList = vtkMRMLFiducialListNode::New();
    if (this->CalibrationFiducialsList == NULL)
        {
        // error macro  
        int error = 1;
        }   
    this->CalibrationFiducialsList->SetLocked(true);
    this->CalibrationFiducialsList->SetName("TRProstateBiopsyFiducialList");
    this->CalibrationFiducialsList->SetDescription("Created by TR Prostate Biopsy Module; marks 4 calibration markers on the device");
    this->CalibrationFiducialsList->SetColor(0.5,0.5,0.5);
    this->CalibrationFiducialsList->SetGlyphType(vtkMRMLFiducialListNode::Diamond3D);
    this->CalibrationFiducialsList->SetOpacity(0.7);
    this->CalibrationFiducialsList->SetAllFiducialsVisibility(false);
    this->CalibrationFiducialsList->SetSymbolScale(5);
    this->CalibrationFiducialsList->SetTextScale(5);

    this->NumberOfNeedles = 1;

    

}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetCalibrationVolumeNode(vtkMRMLScalarVolumeNode *calibVolNode)
{
  vtkSetMRMLNodeMacro(this->CalibrationVolumeNode, calibVolNode);  
}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetTargetingVolumeNode(vtkMRMLScalarVolumeNode *targetingVolNode)
{
  vtkSetMRMLNodeMacro(this->TargetingVolumeNode, targetingVolNode);
}

//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetVerificationVolumeNode(vtkMRMLScalarVolumeNode *verifVolNode)
{
  vtkSetMRMLNodeMacro(this->VerificationVolumeNode, verifVolNode);
}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetTargetingFiducialsList(vtkMRMLFiducialListNode *targetList, unsigned int index)
{
  if (index < this->NumberOfNeedles)
    {   
    vtkSetMRMLNodeMacro(this->TargetingFiducialsListsVector[index], targetList);
    }
}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetCalibrationFiducialsList(vtkMRMLFiducialListNode *calibList)
{
  vtkSetMRMLNodeMacro(this->CalibrationFiducialsList, calibList);
}
//-------------------------------------------------------------------------------
unsigned int vtkMRMLTRProstateBiopsyModuleNode::AddTargetDescriptor(vtkTRProstateBiopsyTargetDescriptor *target)
{
  unsigned int index = this->TargetDescriptorsVector.size();
  this->TargetDescriptorsVector.push_back(target);  
  return index;
}
//-------------------------------------------------------------------------------
vtkTRProstateBiopsyTargetDescriptor *vtkMRMLTRProstateBiopsyModuleNode::GetTargetDescriptorAtIndex(unsigned int index)
{
  if (index < this->TargetDescriptorsVector.size())
    {
    return this->TargetDescriptorsVector[index];
    }
  else
    {
    return NULL;
    }
}
//-------------------------------------------------------------------------------
bool vtkMRMLTRProstateBiopsyModuleNode::AddTargetToFiducialList(double targetRAS[3], unsigned int fiducialListIndex, unsigned int targetNr, int & fiducialIndex)
{
  if (fiducialListIndex < this->NumberOfNeedles)
    {
    std::string targetTypeName = this->TargetingFiducialsListsNames[fiducialListIndex];
    this->TargetingFiducialsListsVector[fiducialListIndex]->SetDisableModifiedEvent(1);
    fiducialIndex = this->TargetingFiducialsListsVector[fiducialListIndex]->AddFiducialWithXYZ(targetRAS[0], targetRAS[1], targetRAS[2], false);
    if (fiducialIndex!= -1)
      {
      char targetName[20];
      sprintf(targetName, "%s_%1d", targetTypeName.c_str(),targetNr);
      this->TargetingFiducialsListsVector[fiducialListIndex]->SetNthFiducialLabelText(fiducialIndex, std::string(targetName).c_str());
      this->TargetingFiducialsListsVector[fiducialListIndex]->SetNthFiducialID(fiducialIndex, std::string(targetName).c_str());
      this->TargetingFiducialsListsVector[fiducialListIndex]->SetDisableModifiedEvent(0);
      this->TargetingFiducialsListsVector[fiducialListIndex]->Modified();

      //this->TargetingFiducialsListsVector[fiducialListIndex]->SetNthFiducialVisibility(fiducialIndex, true);
      return true;
      }
    }

  return false;
}
//-------------------------------------------------------------------------------
bool vtkMRMLTRProstateBiopsyModuleNode::GetTargetFromFiducialList(int fiducialListIndex, int fiducialIndex, double &r, double &a, double &s)
{
    if (fiducialListIndex < this->NumberOfNeedles && fiducialListIndex != -1)
      {
      if (fiducialIndex < this->TargetingFiducialsListsVector[fiducialListIndex]->GetNumberOfFiducials() && fiducialIndex != -1)
        {
        float *ras = new float[3];
        ras = this->TargetingFiducialsListsVector[fiducialListIndex]->GetNthFiducialXYZ(fiducialIndex);
        r = ras[0];
        a = ras[1];
        s = ras[2];
        return true;
        }
      }
    return false;
    
}

//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetFiducialColor(int fiducialListIndex, int fiducialIndex, bool selected)
{
  if (fiducialListIndex < this->NumberOfNeedles && fiducialListIndex != -1)
      {
      if (fiducialIndex < this->TargetingFiducialsListsVector[fiducialListIndex]->GetNumberOfFiducials() && fiducialIndex != -1)
        {
        this->TargetingFiducialsListsVector[fiducialListIndex]->SetNthFiducialSelected(fiducialIndex, selected);
        }
      }
}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetupNeedlesList()
{
  this->NeedlesVector.resize(this->NumberOfNeedles); 
  for (unsigned int i = 0; i < this->NumberOfNeedles; i++)
    {
    NeedleDescriptorStruct *needle = new NeedleDescriptorStruct;
    this->NeedlesVector[i] = needle;
    }

}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetupTargetingFiducialsList()
{ 
   
  this->TargetingFiducialsListsVector.resize(this->NumberOfNeedles);  
  this->TargetingFiducialsListsNames.resize(this->NumberOfNeedles);  

  this->TargetingFiducialsListsVector.clear();
  this->TargetingFiducialsListsNames.clear();

  for (unsigned int i = 0; i < this->NumberOfNeedles; i++)
  {
    std::string needleType = this->NeedlesVector[i]->NeedleType;
    this->TargetingFiducialsListsNames.push_back(this->NeedlesVector[i]->NeedleType);   
    // create corresponding targeting fiducials list
    vtkMRMLFiducialListNode *targetFidList = vtkMRMLFiducialListNode::New();
    targetFidList->SetLocked(true);
    targetFidList->SetName("TRProstateBiopsyFiducialList");
    targetFidList->SetDescription("Created by TR Prostate Biopsy Module");
    targetFidList->SetColor(0.5+i/10.0,0.7+i/10.0,0.3+i/10.0);
    targetFidList->SetSelectedColor(1.0, 0.0, 0.0);
    targetFidList->SetGlyphType(vtkMRMLFiducialListNode::Sphere3D);
    targetFidList->SetOpacity(0.6);
    targetFidList->SetAllFiducialsVisibility(true);
    targetFidList->SetSymbolScale(5);
    targetFidList->SetTextScale(5);        
    this->TargetingFiducialsListsVector.push_back(targetFidList);
    this->SetTargetingFiducialsList(targetFidList, i);
  }
}

//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetCalibrationMarker(unsigned int markerNr, double markerRAS[3])
{
  if (markerNr <=4 && markerNr >0)
    {
    char markername[20];
    sprintf(markername, "Marker_%1d", markerNr);
    int index = this->CalibrationFiducialsList->GetFiducialIndex(std::string(markername));
    if (index == -1)
      {
      index = this->CalibrationFiducialsList->AddFiducialWithXYZ(markerRAS[0], markerRAS[1], markerRAS[2],false);
      }
    this->CalibrationFiducialsList->SetNthFiducialLabelText(index, std::string(markername).c_str());
    this->CalibrationFiducialsList->SetNthFiducialID(index, std::string(markername).c_str());
    this->CalibrationFiducialsList->SetNthFiducialVisibility(index, false);
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::GetCalibrationMarker(unsigned int markerNr, double &r, double &a, double &s)
{
  if (markerNr <=4 && markerNr >0)
    {
    char markername[20];
    sprintf(markername, "Marker_%1d", markerNr);
    int index = this->CalibrationFiducialsList->GetFiducialIndex(std::string(markername));

    if (index != -1)
      {
      float *ras = new float[3];
      ras = this->CalibrationFiducialsList->GetNthFiducialXYZ(index);
      r = ras[0];
      a = ras[1];
      s = ras[2];
      }
    
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleType(unsigned int needleIndex, std::string type)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    this->NeedlesVector[needleIndex]->NeedleType = type;
    }
}
//----------------------------------------------------------------------------
std::string vtkMRMLTRProstateBiopsyModuleNode::GetNeedleType(unsigned int needleIndex)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    return this->NeedlesVector[needleIndex]->NeedleType;
    }
  else
    {
    return NULL;
    }
}
//-----------------------------------------------------------------------------  
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleDescription(unsigned int needleIndex, std::string desc)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    this->NeedlesVector[needleIndex]->Description = desc;
    }
}
//-----------------------------------------------------------------------------
std::string vtkMRMLTRProstateBiopsyModuleNode::GetNeedleDescription(unsigned int needleIndex)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    return this->NeedlesVector[needleIndex]->Description;
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleDepth(unsigned int needleIndex, float depth)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    this->NeedlesVector[needleIndex]->NeedleDepth = depth;
    }
}
//------------------------------------------------------------------------------
float vtkMRMLTRProstateBiopsyModuleNode::GetNeedleDepth(unsigned int needleIndex)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    return this->NeedlesVector[needleIndex]->NeedleDepth;
    }
  else
    {
    return 0;
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleOvershoot(unsigned int needleIndex, float overshoot)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    this->NeedlesVector[needleIndex]->NeedleOvershoot = overshoot;
    }
}
//------------------------------------------------------------------------------
float vtkMRMLTRProstateBiopsyModuleNode::GetNeedleOvershoot(unsigned int needleIndex)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    return this->NeedlesVector[needleIndex]->NeedleOvershoot;
    }
  else
    {
    return 0;
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleUID(unsigned int needleIndex, unsigned int uid)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    this->NeedlesVector[needleIndex]->UniqueIdentifier = uid;
    }  
}
//------------------------------------------------------------------------------
unsigned int vtkMRMLTRProstateBiopsyModuleNode::GetNeedleUID(unsigned int needleIndex)
{
  if (needleIndex < this->NumberOfNeedles)
    {
    return this->NeedlesVector[needleIndex]->UniqueIdentifier;
    }
  else
    {
    return 0;
    } 
}
//------------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkMRMLTRProstateBiopsyModuleNode::GetVolumeNodeAtIndex(int index)
{
  if (index >=0 && index < this->VolumesList.size())
    {
    return this->VolumesList[index]->VolumeNode;
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
char *vtkMRMLTRProstateBiopsyModuleNode::GetDiskLocationOfVolumeAtIndex(int index)
{
  if (index >=0 && index < this->VolumesList.size())
    {
    return this->VolumesList[index]->DiskLocation;
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
char *vtkMRMLTRProstateBiopsyModuleNode::GetTypeOfVolumeAtIndex(int index)
{
  if (index >=0 && index < this->VolumesList.size())
    {
    return this->VolumesList[index]->Type;
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
bool vtkMRMLTRProstateBiopsyModuleNode::IsVolumeAtIndexActive(int index)
{
  return false;
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::AddVolumeInformationToList(vtkMRMLScalarVolumeNode *volNode, const char *diskLocation, char *type)
{
  VolumeInformationStruct *volStruct = new VolumeInformationStruct;

  volStruct->DiskLocation = new char[strlen(diskLocation)+1];
  strcpy(volStruct->DiskLocation, diskLocation);

  volStruct->Type = new char[strlen(type)+1];
  strcpy(volStruct->Type, type);

  volStruct->VolumeNode = volNode;

  volStruct->Active = false;

  this->VolumesList.push_back(volStruct);
}
