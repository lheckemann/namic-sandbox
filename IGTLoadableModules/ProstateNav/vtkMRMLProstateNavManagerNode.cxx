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
  this->TargetPlanList = NULL;
  this->TargetCompletedList = NULL;
  this->RobotConnector = NULL;
  this->ScannerConnector = NULL;
  this->RobotCommand = NULL;
  this->RobotTarget = NULL;
  this->ZFrameTransform = NULL;
  this->ZFrameModel = NULL;
  this->StepList=vtkStringArray::New();

  this->StepList->InsertNextValue("SetUp");
  this->StepList->InsertNextValue("ZFrameCalibration");
  this->StepList->InsertNextValue("PointTargeting");
  this->StepList->InsertNextValue("PointVerification");
  this->StepList->InsertNextValue("TransperinealProstateRobotManualControl");
}

//----------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode::~vtkMRMLProstateNavManagerNode()
{
  this->StepList->Delete();
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  //switch (this->Type)
  //  {
  //  case TYPE_SERVER:
  //    of << " connectorType=\"" << "SERVER" << "\" ";
  //    break;
  //  case TYPE_CLIENT:
  //    of << " connectorType=\"" << "CLIENT" << "\" ";
  //    of << " serverHostname=\"" << this->ServerHostname << "\" ";
  //    break;
  //  default:
  //    of << " connectorType=\"" << "NOT_DEFINED" << "\" ";
  //    break;
  //  }
  //
  //of << " serverPort=\"" << this->ServerPort << "\" ";
  //of << " restrictDeviceName=\"" << this->RestrictDeviceName << "\" ";

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

/*
    if (!strcmp(attName, "connectorType"))
      {
      if (!strcmp(attValue, "SERVER"))
        {
        type = TYPE_SERVER;
        }
      else if (!strcmp(attValue, "CLIENT"))
        {
        type = TYPE_CLIENT;
        }
      else
        {
        type = TYPE_NOT_DEFINED;
        }
      }
    if (!strcmp(attName, "serverHostname"))
      {
      serverHostname = attValue;
      }
    if (!strcmp(attName, "serverPort"))
      {
      std::stringstream ss;
      ss << attValue;
      ss >> port;
      }
    if (!strcmp(attName, "restrictDeviceName"))
      {
      std::stringstream ss;
      ss << attValue;
      ss >> restrictDeviceName;;
      }
*/
    }

  /*
  switch(type)
    {
    case TYPE_SERVER:
      this->SetTypeServer(port);
      this->SetRestrictDeviceName(restrictDeviceName);
      break;
    case TYPE_CLIENT:
      this->SetTypeClient(serverHostname, port);
      this->SetRestrictDeviceName(restrictDeviceName);
      break;
    default: // not defined
      // do nothing
      break;
    }
  */

}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLProstateNavManagerNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  vtkMRMLProstateNavManagerNode *node = (vtkMRMLProstateNavManagerNode *) anode;

  //int type = node->GetType();
  
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  if (this->TargetPlanList && this->TargetPlanList == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
    event ==  vtkCommand::ModifiedEvent)
    {
    //this->ModifiedSinceReadOn();
    //this->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent, NULL);
    //this->UpdateFromMRML();
    return;
    }

  if (this->TargetCompletedList && this->TargetCompletedList == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
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
  //:TODO: robot notification shall be performed in the robot node class
  // as a response to a step change (the robot node class shall be subscribed to the modified event of the manager node)
  // START_UP, CALIBRATION, TARGETING, MANUAL, PLANNING
  /*
  if (this->RobotCommand && this->StepList[newStep].wpcommand.length() > 0)
  {
    const char* command = this->StepList[newStep].wpcommand.c_str();
    this->RobotCommand->PushOutgoingCommand(command);
    this->RobotCommand->InvokeEvent(vtkCommand::ModifiedEvent);
  }
  */
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

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetPlanList(vtkMRMLFiducialListNode* ptr)
{

  if (this->TargetPlanList != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->TargetPlanList, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->TargetPlanList = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->TargetPlanList != ptr )
    {
    this->Modified();
    }

}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetCompletedList(vtkMRMLFiducialListNode* ptr)
{

  if (this->TargetCompletedList != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->TargetCompletedList, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->TargetCompletedList = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->TargetCompletedList != ptr )
    {
    this->Modified();
    }


}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveRobotConnector(vtkMRMLIGTLConnectorNode* ptr)
{

  if (this->RobotConnector != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->RobotConnector, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->RobotConnector = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->RobotConnector && this->RobotCommand )
    {
    this->RobotConnector->RegisterOutgoingMRMLNode( this->RobotCommand );
    this->RobotConnector->RegisterIncomingMRMLNode( this->RobotCommand );    
    }


  if ( this->RobotConnector != ptr )
    {
    this->Modified();
    }

}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveScannerConnector(vtkMRMLIGTLConnectorNode* ptr)
{

  if (this->ScannerConnector != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->ScannerConnector, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->ScannerConnector = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->ScannerConnector != ptr )
    {
    this->Modified();
    }

}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveRobotCommand(vtkMRMLBrpRobotCommandNode* ptr)
{

  if (this->RobotCommand != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->RobotCommand, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->RobotCommand = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  
  if ( this->RobotCommand != ptr )
    {
    this->Modified();
    }

}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveRobotTarget(vtkMRMLLinearTransformNode* ptr)
{

  if (this->RobotTarget != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->RobotTarget, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->RobotTarget = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->RobotTarget != ptr )
    {
    this->Modified();
    }
  
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveZFrameTransform(vtkMRMLLinearTransformNode* ptr)
{

  if (this->ZFrameTransform != NULL)
    {
    vtkEventBroker::GetInstance()->RemoveObservations(
      this->ZFrameTransform, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  this->ZFrameTransform = ptr;

  if (ptr != NULL)
    {
    vtkEventBroker::GetInstance()->AddObservation(
      ptr, vtkCommand::ModifiedEvent, this, this->MRMLCallbackCommand );
    }

  if ( this->ZFrameTransform != ptr )
    {
    this->Modified();
    }

}

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
