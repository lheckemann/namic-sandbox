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
#include "vtkMRMLScalarVolumeNode.h"

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
  
  this->CalibrationFiducialsList   = NULL;  
  this->NumberOfTargetingFiducialLists = 0;
 
  
  this->CalibrationVolumeNode = NULL;
  this->TargetingVolumeNode = NULL;
  this->VerificationVolumeNode = NULL;

  this->HideFromEditors = true;

  this->InitializeFiducialListNode();
}

//----------------------------------------------------------------------------
vtkMRMLTRProstateBiopsyModuleNode::~vtkMRMLTRProstateBiopsyModuleNode()
{
  this->SetVolumeInUse(NULL);

  this->SetCalibrationVolumeNode(NULL);
  this->SetTargetingVolumeNode(NULL);
  this->SetVerificationVolumeNode(NULL);

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
    this->CalibrationFiducialsList->SetAllFiducialsVisibility(true);
    this->CalibrationFiducialsList->SetSymbolScale(20);
    this->CalibrationFiducialsList->SetTextScale(10);

    // create at least one targeting fiducials list
    vtkMRMLFiducialListNode *targetFidList = vtkMRMLFiducialListNode::New();
    targetFidList->SetLocked(true);
    targetFidList->SetName("TRProstateBiopsyFiducialList");
    targetFidList->SetDescription("Created by TR Prostate Biopsy Module; marks 4 calibration markers on the device");
    targetFidList->SetColor(0.5,0.5,0.5);
    targetFidList->SetGlyphType(vtkMRMLFiducialListNode::Diamond3D);
    targetFidList->SetOpacity(0.7);
    targetFidList->SetAllFiducialsVisibility(true);
    targetFidList->SetSymbolScale(20);
    targetFidList->SetTextScale(10);

    this->TargetingFiducialsListsVector.clear();
    this->NumberOfTargetingFiducialLists = 1;
    this->TargetingFiducialsListsVector.push_back(targetFidList);
    this->SetTargetingFiducialsList(targetFidList, 0);
    this->TargetingFiducialsListsNames.clear();
    this->TargetingFiducialsListsNames.push_back(std::string("Biopsy"));

    

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
  if (index < this->NumberOfTargetingFiducialLists)
    {   
    vtkSetMRMLNodeMacro(this->TargetingFiducialsListsVector[index], targetList);
    }
}
//-------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::SetCalibrationFiducialsList(vtkMRMLFiducialListNode *calibList)
{
  vtkSetMRMLNodeMacro(this->CalibrationFiducialsList, calibList);
}
//------------------------------------------------------------------------------
void vtkMRMLTRProstateBiopsyModuleNode::AddTargetingFiducialsList(vtkMRMLFiducialListNode *targetList, std::string listName)
{ 
  this->NumberOfTargetingFiducialLists++;
  this->TargetingFiducialsListsVector.resize(this->NumberOfTargetingFiducialLists);
  this->TargetingFiducialsListsVector.push_back(targetList);
  this->SetTargetingFiducialsList(targetList, this->NumberOfTargetingFiducialLists-1);
  this->TargetingFiducialsListsNames.resize(this->NumberOfTargetingFiducialLists);
  this->TargetingFiducialsListsNames.push_back(listName);
}
