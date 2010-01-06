/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLTransPerinealProstateTemplateNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"

#include "vtkMRMLTransPerinealProstateTemplateNode.h"
#include "vtkMRMLScene.h"

#include "vtkOpenIGTLinkIFGUI.h"
#include "vtkOpenIGTLinkIFLogic.h"
#include "vtkIGTLToMRMLCoordinate.h"

#include "vtkMRMLIGTLConnectorNode.h"

#include "vtkZFrameRobotToImageRegistration.h"

#include "vtkProstateNavTargetDescriptor.h"

#include "vtkTriangleFilter.h"

//-------------------------------------

static const int STATUS_SCANNER=1;

static const char STATUS_SCANNER_OFF[]="Scanner: OFF";
static const char STATUS_SCANNER_ON[]="Scanner: ON";

//------------------------------------------------------------------------------
vtkMRMLTransPerinealProstateTemplateNode* vtkMRMLTransPerinealProstateTemplateNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransPerinealProstateTemplateNode");
  if(ret)
    {
      return (vtkMRMLTransPerinealProstateTemplateNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransPerinealProstateTemplateNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransPerinealProstateTemplateNode* vtkMRMLTransPerinealProstateTemplateNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransPerinealProstateTemplateNode");
  if(ret)
    {
      return (vtkMRMLTransPerinealProstateTemplateNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransPerinealProstateTemplateNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransPerinealProstateTemplateNode::vtkMRMLTransPerinealProstateTemplateNode()
{
  // Node references

  this->ScannerConnectorNodeID=NULL;
  this->ScannerConnectorNode=NULL;

  this->ZFrameModelNodeID=NULL;
  this->ZFrameModelNode=NULL;

  this->ZFrameTransformNodeID=NULL;
  this->ZFrameTransformNode=NULL;  

  this->WorkspaceModelNodeID=NULL;
  this->WorkspaceModelNode=NULL;

  // Other

  this->ScannerWorkPhase     = -1;
  this->ScannerConnectedFlag = 0;

  StatusDescriptor s;
  s.indicator=StatusOff;
  s.text=STATUS_SCANNER_OFF;
  this->StatusDescriptors.push_back(s); // STATUS_SCANNER=1
}

//----------------------------------------------------------------------------
vtkMRMLTransPerinealProstateTemplateNode::~vtkMRMLTransPerinealProstateTemplateNode()
{
  if (this->ScannerConnectorNodeID) 
  {
    SetAndObserveScannerConnectorNodeID(NULL);
  }
  if (this->ZFrameModelNodeID) 
  {
    SetAndObserveZFrameModelNodeID(NULL);
  }
  if (this->ZFrameTransformNodeID) 
  {
    SetAndObserveZFrameTransformNodeID(NULL);
  }

}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::WriteXML(ostream& of, int nIndent)
{
  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  if (this->ScannerConnectorNodeID != NULL) 
    {
    of << indent << " ScannerConnectorNodeRef=\"" << this->ScannerConnectorNodeID << "\"";
    }
  if (this->ZFrameModelNodeID != NULL) 
    {
    of << indent << " ZFrameModelNodeRef=\"" << this->ZFrameModelNodeID << "\"";
    }
  if (this->ZFrameTransformNodeID != NULL) 
    {
    of << indent << " ZFrameTransformNodeRef=\"" << this->ZFrameTransformNodeID << "\"";
    }

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


}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
    attName  = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "ScannerConnectorNodeRef")) 
      {
      this->SetAndObserveScannerConnectorNodeID(attValue);
      }
    if (!strcmp(attName, "ZFrameModelNodeRef")) 
      {
      this->SetAndObserveZFrameModelNodeID(attValue);
      }
    if (!strcmp(attName, "ZFrameTransformNodeRef")) 
      {
      this->SetAndObserveZFrameTransformNodeID(attValue);
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
void vtkMRMLTransPerinealProstateTemplateNode::Copy(vtkMRMLNode *anode)
{  
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLTransPerinealProstateTemplateNode *node = vtkMRMLTransPerinealProstateTemplateNode::SafeDownCast(anode);
  if (node!=NULL)
  {
    this->SetScannerConnectorNodeID(node->ScannerConnectorNodeID);
    this->SetZFrameModelNodeID(node->ZFrameModelNodeID);
    this->SetZFrameTransformNodeID(node->ZFrameTransformNodeID);
  }
  else
  {
    vtkErrorMacro("Invalid node");
  }

  this->StatusDescriptors.clear();
  for (int i=0; i<node->StatusDescriptors.size(); i++)
  {    
    this->StatusDescriptors.push_back(node->StatusDescriptors[i]);
  }

  this->EndModify(disabledModify);
}

//-----------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (this->ScannerConnectorNodeID != NULL && this->Scene->GetNodeByID(this->ScannerConnectorNodeID) == NULL)
    {
    this->SetAndObserveScannerConnectorNodeID(NULL);
    }
  if (this->ZFrameModelNodeID != NULL && this->Scene->GetNodeByID(this->ZFrameModelNodeID) == NULL)
    {
    this->SetAndObserveZFrameModelNodeID(NULL);
    }
  if (this->ZFrameTransformNodeID != NULL && this->Scene->GetNodeByID(this->ZFrameTransformNodeID) == NULL)
    {
    this->SetAndObserveZFrameTransformNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
  if (this->ScannerConnectorNodeID && !strcmp(oldID, this->ScannerConnectorNodeID))
    {
    this->SetAndObserveScannerConnectorNodeID(newID);
    }
  if (this->ZFrameModelNodeID && !strcmp(oldID, this->ZFrameModelNodeID))
    {
    this->SetAndObserveZFrameModelNodeID(newID);
    }
  if (this->ZFrameTransformNodeID && !strcmp(oldID, this->ZFrameTransformNodeID))
    {
    this->SetAndObserveZFrameTransformNodeID(newID);
    }

}

//-----------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);
   this->SetAndObserveScannerConnectorNodeID(this->GetScannerConnectorNodeID());
   this->SetAndObserveZFrameModelNodeID(this->GetZFrameModelNodeID());
   this->SetAndObserveZFrameTransformNodeID(this->GetZFrameTransformNodeID());
}

//-----------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  if (this->StatusDescriptors.size()<=STATUS_SCANNER)
  {
    vtkErrorMacro("StatusDescriptors does not contain scanner and robot status");
  }
  else
  {
    if (this->GetScannerConnectorNode() && this->GetScannerConnectorNode() == vtkMRMLIGTLConnectorNode::SafeDownCast(caller))
      {
      switch (event)
        {
        case vtkMRMLIGTLConnectorNode::ConnectedEvent:
          this->StatusDescriptors[STATUS_SCANNER].indicator=StatusOk;
          this->StatusDescriptors[STATUS_SCANNER].text=STATUS_SCANNER_ON;
          this->InvokeEvent(vtkMRMLRobotNode::ChangeStatusEvent);
          break;
        case vtkMRMLIGTLConnectorNode::DisconnectedEvent:
          this->StatusDescriptors[STATUS_SCANNER].indicator=StatusOff;
          this->StatusDescriptors[STATUS_SCANNER].text=STATUS_SCANNER_OFF;
          this->InvokeEvent(vtkMRMLRobotNode::ChangeStatusEvent);
          break;
        }
      }
  }
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


int vtkMRMLTransPerinealProstateTemplateNode::Init(vtkSlicerApplication* app)
{ 
  this->Superclass::Init(app);

  vtkOpenIGTLinkIFGUI* igtlGUI = vtkOpenIGTLinkIFGUI::SafeDownCast(app->GetModuleGUIByName("OpenIGTLink IF"));
  if (igtlGUI)
    {
    igtlGUI->Enter();    
    //igtlGUI->GetLogic()->RegisterMessageConverter(this->CoordinateConverter);
    //igtlGUI->GetLogic()->RegisterMessageConverter(this->CommandConverter);
    }

  // Z-Frame transform node
  if (GetZFrameTransformNode()==NULL)
  {
    vtkMRMLLinearTransformNode* ztnode = vtkMRMLLinearTransformNode::New();
    ztnode->SetName("ZFrameTransform");
    vtkMatrix4x4* ztransform = vtkMatrix4x4::New();
    ztransform->Identity();
    //transformNode->SetAndObserveImageData(transform);
    ztnode->ApplyTransform(ztransform);
    ztnode->SetScene(this->Scene);
    ztransform->Delete();
    this->Scene->AddNode(ztnode);
    SetAndObserveZFrameTransformNodeID(ztnode->GetID());
  }

  // ZFrame model
  // This part should be moved to Robot Display Node.
  if (GetZFrameModelNode()==NULL)
  {
    const char* nodeID = AddZFrameModel("ZFrame");
    vtkMRMLModelNode*  modelNode = vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(nodeID));
    if (modelNode)
      {
      vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
      displayNode->SetVisibility(0);
      modelNode->Modified();
      this->Scene->Modified();
      modelNode->SetAndObserveTransformNodeID(GetZFrameTransformNodeID());
      SetAndObserveZFrameModelNodeID(nodeID);
      }
  }

  // Workspace model
  // This part should be moved to Robot Display Node.
  if (GetWorkspaceModelNode()==NULL)
  {
    const char* nodeID = AddWorkspaceModel("Range of Motion");
    vtkMRMLModelNode*  modelNode = vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(nodeID));
    if (modelNode)
      {
      vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
      displayNode->SetVisibility(0);
      modelNode->Modified();
      this->Scene->Modified();
      modelNode->SetAndObserveTransformNodeID(GetZFrameTransformNodeID());
      SetAndObserveWorkspaceModelNodeID(nodeID);
      }
  }

  return 1;
}


int vtkMRMLTransPerinealProstateTemplateNode::OnTimer()
{
    return 1;
}


//---------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateTemplateNode::SendZFrame()
{

  std::cerr << "int vtkProstateNavLogic::SendZFrame(): " << this->GetZFrameTransformNodeID() << std::endl;

  return 1;
  
}


//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateTemplateNode::AddWorkspaceModel(const char* nodeName)
{
  vtkMRMLModelNode           *workspaceModel;
  vtkMRMLModelDisplayNode    *workspaceDisp;
  
  workspaceModel = vtkMRMLModelNode::New();
  workspaceDisp  = vtkMRMLModelDisplayNode::New();

  this->Scene->SaveStateForUndo();
  this->Scene->AddNode(workspaceDisp);
  this->Scene->AddNode(workspaceModel);

  workspaceDisp->SetScene(this->Scene);
  workspaceModel->SetName(nodeName);
  workspaceModel->SetScene(this->Scene);
  workspaceModel->SetAndObserveDisplayNodeID(workspaceDisp->GetID());
  workspaceModel->SetHideFromEditors(0);

  // construct Z-frame model
  
  // Parameters
  // offset from z-frame -- this will be a class variable to let users configure it in the future.
  const double offsetFromZFrame[] = {0, 22.76, 150.0};
  const double length = 200.0;

  //----- cylinder 1 (R-A) -----
  vtkCylinderSource *cylinder1 = vtkCylinderSource::New();
  cylinder1->SetResolution(24);
  cylinder1->SetRadius(25.0);
  cylinder1->SetHeight(length);
  cylinder1->SetCenter(0, 0, 0);
  cylinder1->Update();
  
  vtkTransform* trans1 =   vtkTransform::New();
  trans1->Translate(offsetFromZFrame);
  trans1->RotateX(90.0);
  trans1->Update();

  vtkTransformPolyDataFilter *tfilter1 = vtkTransformPolyDataFilter::New();
  tfilter1->SetInput(cylinder1->GetOutput());
  tfilter1->SetTransform(trans1);
  tfilter1->Update();


  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(tfilter1->GetOutput());
  //apd->AddInput(tfilter2->GetOutput());
  apd->Update();

  vtkSmartPointer<vtkTriangleFilter> cleaner=vtkSmartPointer<vtkTriangleFilter>::New();
  cleaner->SetInputConnection(apd->GetOutputPort());
  
  workspaceModel->SetAndObservePolyData(cleaner->GetOutput());

  double color[3];
  color[0] = 0.5;
  color[1] = 0.5;
  color[2] = 1.0;
  workspaceDisp->SetPolyData(workspaceModel->GetPolyData());
  workspaceDisp->SetColor(color);
  workspaceDisp->SetOpacity(0.5);
  
  trans1->Delete();
  tfilter1->Delete();
  cylinder1->Delete();
  apd->Delete();

  const char* modelID = workspaceModel->GetID();

  workspaceDisp->Delete();
  workspaceModel->Delete();

  return modelID;

}


//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateTemplateNode::AddZFrameModel(const char* nodeName)
{
  vtkMRMLModelNode           *zframeModel;
  vtkMRMLModelDisplayNode    *zframeDisp;

  zframeModel = vtkMRMLModelNode::New();
  zframeDisp = vtkMRMLModelDisplayNode::New();

  this->Scene->SaveStateForUndo();
  this->Scene->AddNode(zframeDisp);
  this->Scene->AddNode(zframeModel);  

  zframeDisp->SetScene(this->Scene);
  zframeModel->SetName(nodeName);
  zframeModel->SetScene(this->Scene);
  zframeModel->SetAndObserveDisplayNodeID(zframeDisp->GetID());
  zframeModel->SetHideFromEditors(0);

  // construct Z-frame model
  const double length = 60; // mm

  //----- cylinder 1 (R-A) -----
  vtkCylinderSource *cylinder1 = vtkCylinderSource::New();
  cylinder1->SetRadius(1.5);
  cylinder1->SetHeight(length);
  cylinder1->SetCenter(0, 0, 0);
  cylinder1->Update();
  
  vtkTransformPolyDataFilter *tfilter1 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans1 =   vtkTransform::New();
  trans1->Translate(length/2.0, length/2.0, 0.0);
  trans1->RotateX(90.0);
  trans1->Update();
  tfilter1->SetInput(cylinder1->GetOutput());
  tfilter1->SetTransform(trans1);
  tfilter1->Update();


  //----- cylinder 2 (R-center) -----
  vtkCylinderSource *cylinder2 = vtkCylinderSource::New();
  cylinder2->SetRadius(1.5);
  cylinder2->SetHeight(length*1.4142135);
  cylinder2->SetCenter(0, 0, 0);
  cylinder2->Update();

  vtkTransformPolyDataFilter *tfilter2 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans2 =   vtkTransform::New();
  trans2->Translate(length/2.0, 0.0, 0.0);
  trans2->RotateX(90.0);
  trans2->RotateX(-45.0);
  trans2->Update();
  tfilter2->SetInput(cylinder2->GetOutput());
  tfilter2->SetTransform(trans2);
  tfilter2->Update();


  //----- cylinder 3 (R-P) -----
  vtkCylinderSource *cylinder3 = vtkCylinderSource::New();
  cylinder3->SetRadius(1.5);
  cylinder3->SetHeight(length);
  cylinder3->SetCenter(0, 0, 0);
  cylinder3->Update();

  vtkTransformPolyDataFilter *tfilter3 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans3 =   vtkTransform::New();
  trans3->Translate(length/2.0, -length/2.0, 0.0);
  trans3->RotateX(90.0);
  trans3->Update();
  tfilter3->SetInput(cylinder3->GetOutput());
  tfilter3->SetTransform(trans3);
  tfilter3->Update();


  //----- cylinder 4 (center-P) -----  
  vtkCylinderSource *cylinder4 = vtkCylinderSource::New();
  cylinder4->SetRadius(1.5);
  cylinder4->SetHeight(length*1.4142135);
  cylinder4->SetCenter(0, 0, 0);
  cylinder4->Update();

  vtkTransformPolyDataFilter *tfilter4 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans4 =   vtkTransform::New();
  trans4->Translate(0.0, -length/2.0, 0.0);
  trans4->RotateX(90.0);
  trans4->RotateZ(-45.0);
  trans4->Update();
  tfilter4->SetInput(cylinder4->GetOutput());
  tfilter4->SetTransform(trans4);
  tfilter4->Update();


  //----- cylinder 5 (L-P) -----  
  vtkCylinderSource *cylinder5 = vtkCylinderSource::New();
  cylinder5->SetRadius(1.5);
  cylinder5->SetHeight(length);
  cylinder5->SetCenter(0, 0, 0);
  cylinder5->Update();

  vtkTransformPolyDataFilter *tfilter5 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans5 =   vtkTransform::New();
  trans5->Translate(-length/2.0, -length/2.0, 0.0);
  trans5->RotateX(90.0);
  trans5->Update();
  tfilter5->SetInput(cylinder5->GetOutput());
  tfilter5->SetTransform(trans5);
  tfilter5->Update();


  //----- cylinder 6 (L-center) -----  
  vtkCylinderSource *cylinder6 = vtkCylinderSource::New();
  cylinder6->SetRadius(1.5);
  cylinder6->SetHeight(length*1.4142135);
  cylinder6->SetCenter(0, 0, 0);
  cylinder6->Update();

  vtkTransformPolyDataFilter *tfilter6 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans6 =   vtkTransform::New();
  trans6->Translate(-length/2.0, 0.0, 0.0);
  trans6->RotateX(90.0);
  trans6->RotateX(45.0);
  trans6->Update();
  tfilter6->SetInput(cylinder6->GetOutput());
  tfilter6->SetTransform(trans6);
  tfilter6->Update();


  //----- cylinder 7 (L-A) -----  
  vtkCylinderSource *cylinder7 = vtkCylinderSource::New();
  cylinder7->SetRadius(1.5);
  cylinder7->SetHeight(length);
  cylinder7->SetCenter(0, 0, 0);
  cylinder7->Update();

  vtkTransformPolyDataFilter *tfilter7 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans7 =   vtkTransform::New();
  trans7->Translate(-length/2.0, length/2.0, 0.0);
  trans7->RotateX(90.0);
  trans7->Update();
  tfilter7->SetInput(cylinder7->GetOutput());
  tfilter7->SetTransform(trans7);
  tfilter7->Update();

  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(tfilter1->GetOutput());
  apd->AddInput(tfilter2->GetOutput());
  apd->AddInput(tfilter3->GetOutput());
  apd->AddInput(tfilter4->GetOutput());
  apd->AddInput(tfilter5->GetOutput());
  apd->AddInput(tfilter6->GetOutput());
  apd->AddInput(tfilter7->GetOutput());
  apd->Update();
  
  zframeModel->SetAndObservePolyData(apd->GetOutput());

  double color[3];
  color[0] = 1.0;
  color[1] = 1.0;
  color[2] = 0.0;
  zframeDisp->SetPolyData(zframeModel->GetPolyData());
  zframeDisp->SetColor(color);
  
  trans1->Delete();
  trans2->Delete();
  trans3->Delete();
  trans4->Delete();
  trans5->Delete();
  trans6->Delete();
  trans7->Delete();
  tfilter1->Delete();
  tfilter2->Delete();
  tfilter3->Delete();
  tfilter4->Delete();
  tfilter5->Delete();
  tfilter6->Delete();
  tfilter7->Delete();
  cylinder1->Delete();
  cylinder2->Delete();
  cylinder3->Delete();
  cylinder4->Delete();
  cylinder5->Delete();
  cylinder6->Delete();
  cylinder7->Delete();

  apd->Delete();

  const char* modelID = zframeModel->GetID();

  zframeDisp->Delete();
  zframeModel->Delete();  

  return modelID;
}

//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateTemplateNode::PerformRegistration(vtkMRMLScalarVolumeNode* volumeNode)
{
  vtkZFrameRobotToImageRegistration* registration = vtkZFrameRobotToImageRegistration::New();
  registration->SetFiducialVolume(volumeNode);

  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->Scene->GetNodeByID(this->GetZFrameTransformNodeID()));
  if (transformNode != NULL)
  {
    registration->SetRobotToImageTransform(transformNode);
    registration->DoRegistration();
    //this->GetLogic()->SendZFrame();
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::SwitchStep(const char *stepName)
{
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::SetAndObserveScannerConnectorNodeID(const char *nodeId)
{
  if (nodeId==this->GetScannerConnectorNodeID())
  {
    // no change
    return;
  }
  if (this->GetScannerConnectorNodeID()!=NULL && nodeId!=NULL && strcmp(nodeId, this->GetScannerConnectorNodeID())==0)
  {
    // no change
    return;
  }
  vtkSetAndObserveMRMLObjectMacro(this->ScannerConnectorNode, NULL);
  this->SetScannerConnectorNodeID(nodeId);
  vtkMRMLIGTLConnectorNode *tnode = this->GetScannerConnectorNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkMRMLIGTLConnectorNode::ConnectedEvent);
  events->InsertNextValue(vtkMRMLIGTLConnectorNode::DisconnectedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ScannerConnectorNode, tnode, events);
}


//----------------------------------------------------------------------------
vtkMRMLIGTLConnectorNode* vtkMRMLTransPerinealProstateTemplateNode::GetScannerConnectorNode()
{
  if (this->GetScene() && this->ScannerConnectorNodeID != NULL )
    {    
    return vtkMRMLIGTLConnectorNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ScannerConnectorNodeID));
    }
  return NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::SetAndObserveZFrameModelNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ZFrameModelNode, NULL);
  this->SetZFrameModelNodeID(nodeId);
  vtkMRMLModelNode *tnode = this->GetZFrameModelNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ZFrameModelNode, tnode, events);
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateTemplateNode::GetZFrameModelNode()
{
  if (this->GetScene() && this->ZFrameModelNodeID != NULL )
    {    
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ZFrameModelNodeID));
    }
  return NULL;
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::SetAndObserveWorkspaceModelNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->WorkspaceModelNode, NULL);
  this->SetWorkspaceModelNodeID(nodeId);
  vtkMRMLModelNode *tnode = this->GetWorkspaceModelNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->WorkspaceModelNode, tnode, events);
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateTemplateNode::GetWorkspaceModelNode()
{
  if (this->GetScene() && this->WorkspaceModelNodeID != NULL )
    {    
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->WorkspaceModelNodeID));
    }
  return NULL;
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateTemplateNode::SetAndObserveZFrameTransformNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ZFrameTransformNode, NULL);
  this->SetZFrameTransformNodeID(nodeId);
  vtkMRMLLinearTransformNode *tnode = this->GetZFrameTransformNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ZFrameTransformNode, tnode, events);
}


//----------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLTransPerinealProstateTemplateNode::GetZFrameTransformNode()
{
  if (this->GetScene() && this->ZFrameTransformNodeID != NULL )
    {    
    return vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ZFrameTransformNodeID));
    }
  return NULL;
}

//----------------------------------------------------------------------------
bool vtkMRMLTransPerinealProstateTemplateNode::FindTargetingParams(vtkProstateNavTargetDescriptor *targetDesc)
{
  // this is used for coverage area computation (IsOutsideReach means that the target is outside the robot's coverage area)

  // :TODO: perform real targeting parameter computation  
  double *ras=targetDesc->GetRASLocation();
  const double center[3]={0,0,0};
  const double radius2=25*25;
  targetDesc->SetIsOutsideReach(
    (ras[0]-center[0])*(ras[0]-center[0])+
    (ras[1]-center[1])*(ras[1]-center[1])+
    (ras[2]-center[2])*(ras[2]-center[2])>radius2
    );
  return true;
}

std::string vtkMRMLTransPerinealProstateTemplateNode::GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc)
{
  // :TODO: construct a string that contains useful information for the current target (reachable, etc.)
  return "";
}
