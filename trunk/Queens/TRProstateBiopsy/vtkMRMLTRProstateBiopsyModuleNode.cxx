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
#include "vtkSmartPointer.h"

#include "vtkMRMLTRProstateBiopsyModuleNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLFiducial.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkTRProstateBiopsyTargetDescriptor.h"
#include "vtkSlicerLogic.h"
#include "vtkMath.h"


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
  this->CalibrationVolumeNode = NULL;
  this->TargetingVolumeNode = NULL;
  this->VerificationVolumeNode = NULL;
  this->VolumeInUse = NULL;
  this->SegmentationOutputVolumeRef = NULL;
  this->SegmentationInputVolumeRef = NULL;
  this->SegmentationSeedVolumeRef = NULL;
  this->CalibrationFiducialsListNode   = NULL;  
  this->ExperimentSavePathInConfigFile = NULL;  

  this->CurrentTargetIndex=-1;

  this->ResetCalibrationData();

  this->HideFromEditors = true;
  
  // initialize at least one needle  
  NeedleDescriptorStruct needle;
  needle.Description = "Biopsy needle";
  needle.NeedleDepth = -13;
  needle.NeedleOvershoot = 15;
  needle.NeedleType = "Biopsy";
  needle.UniqueIdentifier = 1;
  needle.TargetingFiducialsList = NULL;

  // create corresponding targeting fiducials list
  vtkMRMLFiducialListNode* targetFidList = vtkMRMLFiducialListNode::New();
  targetFidList->SetLocked(true);
  targetFidList->SetName("TRProstateBiopsyFiducialList");
  targetFidList->SetDescription("Created by TR Prostate Biopsy Module");
  targetFidList->SetColor(0.5,0.7,0.3);
  targetFidList->SetSelectedColor(1.0, 0.0, 0.0);
  targetFidList->SetGlyphType(vtkMRMLFiducialListNode::Sphere3D);
  targetFidList->SetOpacity(0.6);
  targetFidList->SetAllFiducialsVisibility(true);
  targetFidList->SetSymbolScale(5);
  targetFidList->SetTextScale(5);    
  vtkSetMRMLNodeMacro(needle.TargetingFiducialsList, targetFidList);
  targetFidList->Delete();

  this->NeedlesVector.push_back(needle);
    
  this->TargetDescriptorsVector.clear();

  // create node
  vtkSmartPointer<vtkMRMLFiducialListNode> cfln = vtkSmartPointer<vtkMRMLFiducialListNode>::New();
  cfln->SetLocked(false);
  cfln->SetName("TRPB-Calibration");
  cfln->SetDescription("Created by TR Prostate Biopsy Module; marks 4 calibration markers on the device");
  cfln->SetColor(0.5,0.5,0.5);
  cfln->SetGlyphType(vtkMRMLFiducialListNode::Diamond3D);
  cfln->SetOpacity(0.7);
  cfln->SetAllFiducialsVisibility(false);
  cfln->SetSymbolScale(5);
  cfln->SetTextScale(5);    
  vtkSetMRMLNodeMacro(this->CalibrationFiducialsListNode, cfln);  

}

//----------------------------------------------------------------------------
vtkMRMLTRProstateBiopsyModuleNode::~vtkMRMLTRProstateBiopsyModuleNode()
{
  this->SetVolumeInUse(NULL);

  this->SetCalibrationVolumeNode(NULL);
  this->SetTargetingVolumeNode(NULL);
  this->SetVerificationVolumeNode(NULL);
  this->SetExperimentSavePathInConfigFile(NULL);
  

  this->CalibrationVolumeNode = NULL;
  this->TargetingVolumeNode = NULL;
  this->VerificationVolumeNode = NULL;

  this->HideFromEditors = true;
  
  for (std::vector<NeedleDescriptorStruct>::iterator it=this->NeedlesVector.begin(); it!=this->NeedlesVector.end(); ++it)
    {   
    vtkSetMRMLNodeMacro(it->TargetingFiducialsList, NULL);
    }
  this->NeedlesVector.clear();

  vtkSetMRMLNodeMacro(this->CalibrationFiducialsListNode, NULL);
  
  for (std::vector<vtkTRProstateBiopsyTargetDescriptor*>::iterator it=this->TargetDescriptorsVector.begin(); it!=this->TargetDescriptorsVector.end(); ++it)
    {   
    if ((*it)!=NULL)
      {
      (*it)->Delete();
      (*it)=NULL;
      }
    }
  this->TargetDescriptorsVector.clear();
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
 
  vtkSetMRMLNodeMacro(this->CalibrationFiducialsListNode, node->CalibrationFiducialsListNode);  

  // clean up old fiducial list vector
  for (std::vector<NeedleDescriptorStruct>::iterator it=this->NeedlesVector.begin(); it!=this->NeedlesVector.end(); ++it)
    {   
    vtkSetMRMLNodeMacro(it->TargetingFiducialsList, NULL);
    }
  //copy the contents of the other
  this->NeedlesVector.assign(node->NeedlesVector.begin(), node->NeedlesVector.end() );
  // for nodes a simple copy is not enough, vtkSetMRMLNodeMacro shall be used
  {
    std::vector<NeedleDescriptorStruct>::iterator srcit=node->NeedlesVector.begin();
    std::vector<NeedleDescriptorStruct>::iterator dstit=this->NeedlesVector.begin();
    for (; srcit!=node->NeedlesVector.end() && dstit!=this->NeedlesVector.end(); ++srcit, ++dstit)
    {
      dstit->TargetingFiducialsList=NULL;
      vtkSetMRMLNodeMacro(dstit->TargetingFiducialsList, srcit->TargetingFiducialsList);
    }
  }

  // clean up old fiducial list vector
  for (std::vector<vtkTRProstateBiopsyTargetDescriptor*>::iterator it=this->TargetDescriptorsVector.begin(); it!=this->TargetDescriptorsVector.end(); ++it)
    {   
    if ((*it)!=NULL)
      {
      (*it)->Delete();
      (*it)=NULL;
      }
    }
  this->TargetDescriptorsVector.clear();
  //copy the contents of the other
  for (std::vector<vtkTRProstateBiopsyTargetDescriptor*>::iterator it=node->TargetDescriptorsVector.begin(); it!=node->TargetDescriptorsVector.end(); ++it)
    {
    this->TargetDescriptorsVector.push_back(*it);
    this->TargetDescriptorsVector.back()->Register(this); // increase ref counter as there is one more reference
    }

  this->VolumesList.assign(node->VolumesList.begin(), node->VolumesList.end());

  this->SetVolumeInUse(node->VolumeInUse);
  this->SetSegmentationOutputVolumeRef(node->SegmentationOutputVolumeRef);
  this->SetSegmentationInputVolumeRef(node->SegmentationInputVolumeRef);
  this->SetSegmentationSeedVolumeRef(node->SegmentationSeedVolumeRef);
  this->SetExperimentSavePathInConfigFile(node->ExperimentSavePathInConfigFile);

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
unsigned int vtkMRMLTRProstateBiopsyModuleNode::AddTargetDescriptor(vtkTRProstateBiopsyTargetDescriptor* target)
{
  unsigned int index = this->TargetDescriptorsVector.size();
  target->Register(this);
  this->TargetDescriptorsVector.push_back(target);
  return index;
}
//-------------------------------------------------------------------------------
vtkTRProstateBiopsyTargetDescriptor* vtkMRMLTRProstateBiopsyModuleNode::GetTargetDescriptorAtIndex(unsigned int index)
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
int vtkMRMLTRProstateBiopsyModuleNode::SetCurrentTargetIndex(int index)
{
  if (index >= this->TargetDescriptorsVector.size())
    {
    // invalid index, do not change current
    return this->CurrentTargetIndex;
    }
  this->CurrentTargetIndex=index;
  this->Modified();
  return this->CurrentTargetIndex;
}

//-------------------------------------------------------------------------------
bool vtkMRMLTRProstateBiopsyModuleNode::AddTargetToFiducialList(double targetRAS[3], unsigned int fiducialListIndex, unsigned int targetNr, int & fiducialIndex)
{
  if (fiducialListIndex >= this->NeedlesVector.size())
    {
    return false;
    }
  std::string targetTypeName = this->NeedlesVector[fiducialListIndex].NeedleType;  

  int modifyOld=this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->StartModify();
  fiducialIndex = this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->AddFiducialWithXYZ(targetRAS[0], targetRAS[1], targetRAS[2], false);
  if (fiducialIndex==-1)
  {
    this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->EndModify(modifyOld);
    return false;
  }
  std::ostrstream os;
  os << targetTypeName << "_" << targetNr << std::ends;
  this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->SetNthFiducialLabelText(fiducialIndex, os.str());
  this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->SetNthFiducialID(fiducialIndex, os.str());
  os.rdbuf()->freeze();
  this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->SetNthFiducialVisibility(fiducialIndex, true);    
  this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->EndModify(modifyOld);
  // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
  this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
  return true;
}
//-------------------------------------------------------------------------------
bool vtkMRMLTRProstateBiopsyModuleNode::GetTargetFromFiducialList(int fiducialListIndex, int fiducialIndex, double &r, double &a, double &s)
{
    if (fiducialListIndex < this->NeedlesVector.size() && fiducialListIndex != -1)
      {
      if (fiducialIndex < this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->GetNumberOfFiducials() && fiducialIndex != -1)
        {
        float *ras = new float[3];
        ras = this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->GetNthFiducialXYZ(fiducialIndex);
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
  int oldModify=this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->StartModify();
  if (fiducialListIndex < this->NeedlesVector.size() && fiducialListIndex != -1)
      {
      if (fiducialIndex < this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->GetNumberOfFiducials() && fiducialIndex != -1)
        {
        this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->SetNthFiducialSelected(fiducialIndex, selected);
        }
      }
  this->NeedlesVector[fiducialListIndex].TargetingFiducialsList->EndModify(oldModify);
}

//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetCalibrationMarker(unsigned int markerNr, double markerRAS[3])
{
  if (markerNr<0 || markerNr>=4)
    {
    vtkErrorMacro("SetCalibrationMarker: Invalid calibration marker index "<<markerNr);
    return;
    }

  char markername[20];
  sprintf(markername, "Marker_%1d", markerNr+1);    
  int index = this->CalibrationFiducialsListNode->GetFiducialIndex(std::string(markername));
  int modifyOld=this->CalibrationFiducialsListNode->StartModify(); 
  if (index == -1)
    {    
    index = this->CalibrationFiducialsListNode->AddFiducialWithXYZ(markerRAS[0], markerRAS[1], markerRAS[2],false);
    }
  this->CalibrationFiducialsListNode->SetNthFiducialLabelText(index, std::string(markername).c_str());
  this->CalibrationFiducialsListNode->SetNthFiducialID(index, std::string(markername).c_str());
  this->CalibrationFiducialsListNode->SetNthFiducialVisibility(index, true);
  this->CalibrationFiducialsListNode->EndModify(modifyOld); 
  // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
  this->CalibrationFiducialsListNode->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::GetCalibrationMarker(unsigned int markerNr, double &r, double &a, double &s)
{
  if (markerNr<0 || markerNr>=4)
    {
    vtkErrorMacro("GetCalibrationMarker: Invalid calibration marker index "<<markerNr);
    return;
    }
  
  char markername[20];
  sprintf(markername, "Marker_%1d", markerNr+1);
  int index = this->CalibrationFiducialsListNode->GetFiducialIndex(std::string(markername));

  if (index != -1)
    {
    float *ras = new float[3];
    ras = this->CalibrationFiducialsListNode->GetNthFiducialXYZ(index);
    r = ras[0];
    a = ras[1];
    s = ras[2];
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::RemoveAllCalibrationMarkers()
{
  this->CalibrationFiducialsListNode->RemoveAllFiducials();
}

//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleType(unsigned int needleIndex, std::string type)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    this->NeedlesVector[needleIndex].NeedleType = type;
    }
}
//----------------------------------------------------------------------------
std::string vtkMRMLTRProstateBiopsyModuleNode::GetNeedleType(unsigned int needleIndex)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    return this->NeedlesVector[needleIndex].NeedleType;
    }
  else
    {
    return NULL;
    }
}
//-----------------------------------------------------------------------------  
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleDescription(unsigned int needleIndex, std::string desc)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    this->NeedlesVector[needleIndex].Description = desc;
    }
}
//-----------------------------------------------------------------------------
std::string vtkMRMLTRProstateBiopsyModuleNode::GetNeedleDescription(unsigned int needleIndex)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    return this->NeedlesVector[needleIndex].Description;
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleDepth(unsigned int needleIndex, float depth)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    this->NeedlesVector[needleIndex].NeedleDepth = depth;
    }
}
//------------------------------------------------------------------------------
float vtkMRMLTRProstateBiopsyModuleNode::GetNeedleDepth(unsigned int needleIndex)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    return this->NeedlesVector[needleIndex].NeedleDepth;
    }
  else
    {
    return 0;
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleOvershoot(unsigned int needleIndex, float overshoot)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    this->NeedlesVector[needleIndex].NeedleOvershoot = overshoot;
    }
}
//------------------------------------------------------------------------------
float vtkMRMLTRProstateBiopsyModuleNode::GetNeedleOvershoot(unsigned int needleIndex)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    return this->NeedlesVector[needleIndex].NeedleOvershoot;
    }
  else
    {
    return 0;
    }
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetNeedleUID(unsigned int needleIndex, unsigned int uid)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    this->NeedlesVector[needleIndex].UniqueIdentifier = uid;
    }  
}
//------------------------------------------------------------------------------
unsigned int vtkMRMLTRProstateBiopsyModuleNode::GetNeedleUID(unsigned int needleIndex)
{
  if (needleIndex < this->NeedlesVector.size())
    {
    return this->NeedlesVector[needleIndex].UniqueIdentifier;
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
    return this->VolumesList[index].VolumeNode;
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
const char *vtkMRMLTRProstateBiopsyModuleNode::GetDiskLocationOfVolumeAtIndex(int index)
{
  if (index >=0 && index < this->VolumesList.size())
    {
      return this->VolumesList[index].DiskLocation.c_str();
    }
  else
    {
    return NULL;
    }
}
//------------------------------------------------------------------------------
const char *vtkMRMLTRProstateBiopsyModuleNode::GetTypeOfVolumeAtIndex(int index)
{
  if (index >=0 && index < this->VolumesList.size())
    {
      return this->VolumesList[index].Type.c_str();
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
  VolumeInformationStruct volStruct;
  volStruct.DiskLocation=diskLocation;
  volStruct.Type = type;
  volStruct.VolumeNode = volNode;
  volStruct.Active = false;
  this->VolumesList.push_back(volStruct);
}

//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::ResetCalibrationData()
{
  this->CalibrationData.AxesDistance = -1;
  this->CalibrationData.AxesAngleDegrees = 0;
  this->CalibrationData.RobotRegistrationAngleDegrees = 0;
  for (int i=0; i<3; i++)
  {
    this->CalibrationData.I1[i]=0.0;
    this->CalibrationData.I2[i]=0.0;
    this->CalibrationData.v1[i]=0.0;
    this->CalibrationData.v2[i]=0.0;
  }
  this->CalibrationData.CalibrationValid=false;
}


//------------------------------------------------------------------------------
bool vtkMRMLTRProstateBiopsyModuleNode::GetRobotManipulatorTransform(vtkMatrix4x4* transform)
{
  if (!this->CalibrationData.CalibrationValid)
  {
    // no claibration robot position is unknown
    return false;
  }

  transform->Identity();

  // TODO: fix this, this is just a dummy implementation, it does not take into account current encoder position

  double v1norm[3]={this->CalibrationData.v1[0], this->CalibrationData.v1[1], this->CalibrationData.v1[2]};         
  vtkMath::Normalize(v1norm);
  double v2norm[3]={this->CalibrationData.v2[0], this->CalibrationData.v2[1], this->CalibrationData.v2[2]};         
  vtkMath::Normalize(v2norm);

  double x[3]={v1norm[0],v1norm[1],v1norm[2]};
  double y[3]={0,0,0};
  vtkMath::Cross(x, v2norm, y);
  double z[3]={0,0,0};
  vtkMath::Cross(x, y, z);

  // orientation
  transform->SetElement(0,0, x[0]);
  transform->SetElement(1,0, x[1]);
  transform->SetElement(2,0, x[2]);

  transform->SetElement(0,1, y[0]);
  transform->SetElement(1,1, y[1]);
  transform->SetElement(2,1, y[2]);

  transform->SetElement(0,2, z[0]);
  transform->SetElement(1,2, z[1]);
  transform->SetElement(2,2, z[2]);

  // position
  transform->SetElement(0,3, this->CalibrationData.I1[0]);
  transform->SetElement(1,3, this->CalibrationData.I1[1]);
  transform->SetElement(2,3, this->CalibrationData.I1[2]);
  
  return true;
}
