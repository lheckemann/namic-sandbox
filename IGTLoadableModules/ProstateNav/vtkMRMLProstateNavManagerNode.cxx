/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"

#include "vtkMRMLProstateNavManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkStringArray.h"

#include "vtkMRMLScene.h"

#include "vtkProstateNavStep.h"
#include "vtkMRMLRobotNode.h"

#include "vtkSlicerComponentGUI.h" // for vtkSetAndObserveMRMLNodeEventsMacro
#include "vtkSmartPointer.h"


//------------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode* vtkMRMLProstateNavManagerNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLProstateNavManagerNode"); if(ret)
    {
      return (vtkMRMLProstateNavManagerNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLProstateNavManagerNode;
}

//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLProstateNavManagerNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLProstateNavManagerNode");
  if(ret)
    {
      return (vtkMRMLProstateNavManagerNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLProstateNavManagerNode;
}

//----------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode::vtkMRMLProstateNavManagerNode()
{
  this->CurrentStep = 0;
  this->PreviousStep = 0;

  /*
  this->TargetPlanList = NULL;
  this->TargetCompletedList = NULL;
  this->RobotNode=NULL;
  */

  TargetPlanListNodeID=NULL;
  TargetPlanListNode=NULL;

  TargetCompletedListNodeID=NULL;
  TargetCompletedListNode=NULL;

  RobotNodeID=NULL;
  RobotNode=NULL;

  this->StepList=vtkStringArray::New();

  this->HideFromEditorsOff();

}

//----------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode::~vtkMRMLProstateNavManagerNode()
{
  this->StepList->Delete();
  if (this->TargetPlanListNodeID) 
  {
    SetAndObserveTargetPlanListNodeID(NULL);
  }
  if (this->TargetCompletedListNodeID) 
  {
    SetAndObserveTargetCompletedListNodeID(NULL);
  }
  if (this->RobotNodeID) 
  {
    SetAndObserveRobotNodeID(NULL);
  }
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " WorkflowSteps=\"" << GetWorkflowStepsString() << "\"";    

}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  const char* serverHostname = "";
  int port = 0;
  int type = -1;
  int restrictDeviceName = 0;

  
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "WorkflowSteps"))
      {
      if (attValue==NULL)
        {
        vtkErrorMacro("Empty WorkflowSteps attribute value");
        }
      else if (!SetWorkflowStepsFromString(attValue))
        {
        vtkErrorMacro("Invalid WorkflowSteps attribute value: "<<attValue);
        }
      }
    }
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLProstateNavManagerNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLProstateNavManagerNode *node = (vtkMRMLProstateNavManagerNode *) anode;

  //int type = node->GetType();

  this->CurrentStep = node->CurrentStep;
  this->PreviousStep = node->PreviousStep;
  this->SetTargetPlanListNodeID(node->TargetPlanListNodeID);
  this->SetTargetCompletedListNodeID(node->TargetCompletedListNodeID);
  this->SetRobotNodeID(node->RobotNodeID);

  this->StepList->Reset();
  for (int i=0; i>node->StepList->GetSize(); i++)
  {
    this->StepList->SetValue(i, node->StepList->GetValue(i));
  }

  this->HideFromEditors=node->HideFromEditors;
  
}

//-----------------------------------------------------------
void vtkMRMLProstateNavManagerNode::UpdateReferences()
{
   Superclass::UpdateReferences();
/*  if (this->TargetTransformNodeID != NULL && this->Scene->GetNodeByID(this->TargetTransformNodeID) == NULL)
    {
    this->SetAndObserveTargetTransformNodeID(NULL);
    }
    */
}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
  /*if (this->TargetTransformNodeID && !strcmp(oldID, this->TargetTransformNodeID))
    {
    this->SetAndObserveTargetTransformNodeID(newID);
    }*/
}

//-----------------------------------------------------------
void vtkMRMLProstateNavManagerNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);
   /*this->SetAndObserveTargetTransformNodeID(this->GetTargetTransformNodeID());*/
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  if (this->TargetPlanListNode && this->TargetPlanListNode == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
      event ==  vtkCommand::ModifiedEvent)
    {
    //this->ModifiedSinceReadOn();
    //this->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent, NULL);
    //this->UpdateFromMRML();
    return;
    }
  else if (this->TargetCompletedListNode && this->TargetCompletedListNode == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
           event ==  vtkCommand::ModifiedEvent)
    {
    //this->ModifiedSinceReadOn();
    //this->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent, NULL);
    //this->UpdateFromMRML();
    return;
    }

  return;
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::GetNumberOfSteps()
{
  return this->StepList->GetNumberOfValues();
}


//----------------------------------------------------------------------------
const char* vtkMRMLProstateNavManagerNode::GetStepName(int i)
{
  if (i>=0 && i < this->StepList->GetNumberOfValues())
    {
      return this->StepList->GetValue(i);
    }
  else
    {
    return NULL;
    }
}
//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::SwitchStep(int newStep)
{
  if (newStep<0 || newStep>=GetNumberOfSteps())
  {
    return 0;
  }
  this->PreviousStep = this->CurrentStep;
  this->CurrentStep = newStep; 

  // Tentatively, this function calls vtkMRMLBrpRobotCommandNode::SwitchStep().
  // (we cannot test with the robot without sending workphase commands...)
  this->RobotNode->SwitchStep(this->StepList->GetValue(newStep).c_str());

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::GetCurrentStep()
{
  return this->CurrentStep;
}


//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::GetPreviousStep()
{
  return this->PreviousStep;
}

/*
//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetPlanList(vtkMRMLFiducialListNode* ptr)
{
  if (this->TargetPlanList == ptr)
  {
    // no modification
    return;
  }

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue ( vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLNodeMacro(this->TargetPlanList, ptr);
  //vtkSetAndObserveMRMLNodeEventsMacro ( this->TargetPlanList, ptr, events ); // calls this->Modified()
  //vtkSetAndObserveMRMLNodeEventsMacro ( this->TargetPlanList, ptr, NULL); // calls this->Modified()
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetCompletedList(vtkMRMLFiducialListNode* ptr)
{
  if (this->TargetCompletedList == ptr)
  {
    // no modification
    return;
  }

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue ( vtkCommand::ModifiedEvent);
  //vtkSetAndObserveMRMLNodeEventsMacro ( this->TargetCompletedList, ptr, events );  // calls this->Modified()

}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveRobotNode(vtkMRMLRobotNode* ptr)
{
  if (this->RobotNode == ptr)
  {
    // no modification
    return;
  }

  // update workflow steps from the new Robot node
  // (the Manager node is the owner of workflow steps, as it may add additional steps or modify the steps that
  // it gets from the robot)
  if (ptr != NULL)
    {
    SetWorkflowStepsFromString(ptr->GetWorkflowStepsString());    
    }
  else
    {
    SetWorkflowStepsFromString("");
    }

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue ( vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLNodeEventsMacro ( this->RobotNode, ptr, events ); // this triggers a this->Modified() event  
}
*/

//----------------------------------------------------------------------------
vtkStdString vtkMRMLProstateNavManagerNode::GetWorkflowStepsString()
{
  vtkStdString workflowSteps;
  // :TODO: generate the string from the StepList
  for (int i=0; i<this->StepList->GetNumberOfValues(); i++)
    {
    workflowSteps += this->StepList->GetValue(i);
    if (i<this->StepList->GetNumberOfValues()-1)
      {
      workflowSteps += " "; // add separator after each step but the last one
      }
    }
  return workflowSteps;
}

//----------------------------------------------------------------------------
bool vtkMRMLProstateNavManagerNode::SetWorkflowStepsFromString(const vtkStdString& workflowStepsString)
{ 
  this->StepList->Reset();

  if (workflowStepsString.empty())
    {
    vtkWarningMacro("StepList string is empty");
    return false;
    }

  vtkstd::stringstream workflowStepsStream(workflowStepsString);
  vtkstd::string stepName;
  while(workflowStepsStream>>stepName)
    {
    this->StepList->InsertNextValue(stepName);
    }
   return true;
}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetPlanListNodeID(const char *nodeID)
{
  vtkSetAndObserveMRMLObjectMacro(this->TargetPlanListNode, NULL);
  this->SetTargetPlanListNodeID(nodeID);
  vtkMRMLFiducialListNode *tnode = this->GetTargetPlanListNode();
  vtkSetAndObserveMRMLObjectMacro(this->TargetPlanListNode, tnode);
}

//----------------------------------------------------------------------------
vtkMRMLFiducialListNode* vtkMRMLProstateNavManagerNode::GetTargetPlanListNode()
{
  vtkMRMLFiducialListNode* node = NULL;
  if (this->GetScene() && this->TargetPlanListNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->TargetPlanListNodeID);
    node = vtkMRMLFiducialListNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetCompletedListNodeID(const char *nodeID)
{
  vtkSetAndObserveMRMLObjectMacro(this->TargetCompletedListNode, NULL);
  this->SetTargetCompletedListNodeID(nodeID);
  vtkMRMLFiducialListNode *tnode = this->GetTargetCompletedListNode();
  vtkSetAndObserveMRMLObjectMacro(this->TargetCompletedListNode, tnode);
}

//----------------------------------------------------------------------------
vtkMRMLFiducialListNode* vtkMRMLProstateNavManagerNode::GetTargetCompletedListNode()
{
  vtkMRMLFiducialListNode* node = NULL;
  if (this->GetScene() && this->TargetCompletedListNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->TargetCompletedListNodeID);
    node = vtkMRMLFiducialListNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveRobotNodeID(const char *nodeID)
{
  vtkSetAndObserveMRMLObjectMacro(this->RobotNode, NULL);

  // update workflow steps from the new Robot node
  // (the Manager node is the owner of workflow steps, as it may add additional steps or modify the steps that
  // it gets from the robot)
  if (this->GetScene() && nodeID != NULL )
  {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(nodeID);
    vtkMRMLRobotNode* node = vtkMRMLRobotNode::SafeDownCast(snode);
    if (node != NULL)
    {
      SetWorkflowStepsFromString(node->GetWorkflowStepsString());    
    }
    else
    {
      SetWorkflowStepsFromString("");
    }
  }
  else
  {
    SetWorkflowStepsFromString("");
  }

  this->SetRobotNodeID(nodeID);
  vtkMRMLRobotNode *tnode = this->GetRobotNode();

  vtkSetAndObserveMRMLObjectMacro(this->RobotNode, tnode);
}

//----------------------------------------------------------------------------
vtkMRMLRobotNode* vtkMRMLProstateNavManagerNode::GetRobotNode()
{
  vtkMRMLRobotNode* node = NULL;
  if (this->GetScene() && this->RobotNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->RobotNodeID);
    node = vtkMRMLRobotNode::SafeDownCast(snode);
    }
  return node;
}
