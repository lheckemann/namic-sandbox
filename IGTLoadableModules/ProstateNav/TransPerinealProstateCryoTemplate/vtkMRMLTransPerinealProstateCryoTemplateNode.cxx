/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTransPerinealProstateCryoTemplateNode.cxx,v $
  Date:      $Date: 2006/03/17 15:10:10 $
  Version:   $Revision: 1.2 $

  =========================================================================auto=*/
#include <cmath>
#include "vtkObjectFactory.h"

#include "vtkMRMLTransPerinealProstateCryoTemplateNode.h"
#include "vtkMRMLScene.h"

#include "vtkOpenIGTLinkIFGUI.h"
#include "vtkOpenIGTLinkIFLogic.h"
#include "vtkIGTLToMRMLCoordinate.h"

#include "vtkMRMLIGTLConnectorNode.h"

#include "vtkZFrameRobotToImageRegistration.h"

#include "vtkProstateNavTargetDescriptor.h"

#include "vtkTriangleFilter.h"

#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"

#include "vtkParametricEllipsoid.h"
#include "vtkParametricFunctionSource.h"


//
// Models and Transforms
//    + ZFrameTransformNode (position of the center of Z-frame)
//      + ZFrameModel
//      + TemplateTransformNode (center of hole (0, 0) on the front surface, in the Z-frame coordinate)
//          + TemplateModel

// Hole (0, 0) position (surface)
#define TEMPLATE_HOLE_OFFSET_FROM_ZFRAME_X  40.0
#define TEMPLATE_HOLE_OFFSET_FROM_ZFRAME_Y  30.3
#define TEMPLATE_HOLE_OFFSET_FROM_ZFRAME_Z  30.0

// Template corner position
#define TEMPLATE_BLOCK_OFFSET_FROM_HOLE_X   15.0
#define TEMPLATE_BLOCK_OFFSET_FROM_HOLE_Y   15.0
#define TEMPLATE_BLOCK_OFFSET_FROM_HOLE_Z   0.0

#define TEMPLATE_WIDTH   100.0  // dimension in x direction (mm)
#define TEMPLATE_HEIGHT  120.0  // dimension in y direction (mm)
#define TEMPLATE_DEPTH   25.0   // dimension in z direction (mm)
#define TEMPLATE_INTERSECTION_DEPTH TEMPLATE_DEPTH*10
#define TEMPLATE_HOLE_RADIUS 1.5 // (only for visualization)

#define TEMPLATE_GRID_PITCH_X -5
#define TEMPLATE_GRID_PITCH_Y -4.33
#define TEMPLATE_NUMBER_OF_GRIDS_X 17  // must be odd number
#define TEMPLATE_NUMBER_OF_GRIDS_Y 19

#define NEEDLE_LENGTH 100

//-------------------------------------

static const unsigned int STATUS_SCANNER=1;

static const char STATUS_SCANNER_OFF[]="Scanner: OFF";
static const char STATUS_SCANNER_ON[]="Scanner: ON";

//------------------------------------------------------------------------------
vtkMRMLTransPerinealProstateCryoTemplateNode* vtkMRMLTransPerinealProstateCryoTemplateNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransPerinealProstateCryoTemplateNode");
  if(ret)
    {
    return (vtkMRMLTransPerinealProstateCryoTemplateNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransPerinealProstateCryoTemplateNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransPerinealProstateCryoTemplateNode* vtkMRMLTransPerinealProstateCryoTemplateNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLTransPerinealProstateCryoTemplateNode");
  if(ret)
    {
    return (vtkMRMLTransPerinealProstateCryoTemplateNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLTransPerinealProstateCryoTemplateNode;
}

//----------------------------------------------------------------------------
vtkMRMLTransPerinealProstateCryoTemplateNode::vtkMRMLTransPerinealProstateCryoTemplateNode()
{
  // Node references

  this->ScannerConnectorNodeID=NULL;
  this->ScannerConnectorNode=NULL;

  this->ZFrameModelNodeID=NULL;
  this->ZFrameModelNode=NULL;

  this->ZFrameTransformNodeID=NULL;
  this->ZFrameTransformNode=NULL;

  this->TemplateModelNodeID=NULL;
  this->TemplateModelNode=NULL;

  this->TemplateIntersectionModelNodeID=NULL;
  this->TemplateIntersectionModelNode=NULL;

  //this->TemplateTransformNodeID=NULL;
  //this->TemplateTransformNode=NULL;

  this->WorkspaceModelNodeID=NULL;
  this->WorkspaceModelNode=NULL;

  this->ActiveNeedleModelNodeID = NULL;
  this->ActiveNeedleModelNode   = NULL;

  this->ActiveNeedleTransformNodeID = NULL;
  this->ActiveNeedleTransformNode   = NULL;

  // Real-time MRI
  this->ImagingPlaneTransformNodeID = NULL;
  this->ImagingPlaneTransformNode   = NULL;


  // Other
  this->ScannerWorkPhase     = -1;
  this->ScannerConnectedFlag = 0;

  StatusDescriptor s;
  s.indicator=StatusOff;
  s.text=STATUS_SCANNER_OFF;
  this->StatusDescriptors.push_back(s); // STATUS_SCANNER=1

  // Needle guiding template information
  this->TemplateGridPitch[0] = TEMPLATE_GRID_PITCH_X;
  this->TemplateGridPitch[1] = TEMPLATE_GRID_PITCH_Y;
  this->TemplateNumGrids[0]  = TEMPLATE_NUMBER_OF_GRIDS_X;
  this->TemplateNumGrids[1]  = TEMPLATE_NUMBER_OF_GRIDS_Y;
  this->TemplateOffset[0]    = TEMPLATE_HOLE_OFFSET_FROM_ZFRAME_X;
  this->TemplateOffset[1]    = TEMPLATE_HOLE_OFFSET_FROM_ZFRAME_Y;
  this->TemplateOffset[2]    = TEMPLATE_HOLE_OFFSET_FROM_ZFRAME_Z;

  // Needle information
  this->FlagNeedleInfomation = 0;
  this->NeedleDepth = 0.0;
  this->NeedleType = 0;
}


//----------------------------------------------------------------------------
vtkMRMLTransPerinealProstateCryoTemplateNode::~vtkMRMLTransPerinealProstateCryoTemplateNode()
{

  if (this->ImagingPlaneTransformNodeID)
    {
    SetAndObserveImagingPlaneTransformNodeID(NULL);
    }
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
  if (this->TemplateModelNodeID)
    {
    SetAndObserveTemplateModelNodeID(NULL);
    }
  if (this->TemplateIntersectionModelNodeID)
    {
    SetAndObserveTemplateModelNodeID(NULL);
    }
  //if (this->TemplateTransformNodeID)
  //  {
  //  SetAndObserveTemplateTransformNodeID(NULL);
  //  }
  if (this->ActiveNeedleModelNodeID)
    {
    SetAndObserveActiveNeedleModelNodeID(NULL);
    }
  if (this->ActiveNeedleTransformNodeID)
    {
    SetAndObserveActiveNeedleTransformNodeID(NULL);
    }

}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::WriteXML(ostream& of, int nIndent)
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
void vtkMRMLTransPerinealProstateCryoTemplateNode::ReadXMLAttributes(const char** atts)
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
void vtkMRMLTransPerinealProstateCryoTemplateNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLTransPerinealProstateCryoTemplateNode *node = vtkMRMLTransPerinealProstateCryoTemplateNode::SafeDownCast(anode);
  if (node!=NULL)
    {
    this->SetAndObserveImagingPlaneTransformNodeID(NULL); // remove observer
    this->SetImagingPlaneTransformNodeID(node->ImagingPlaneTransformNodeID);
    this->SetAndObserveScannerConnectorNodeID(NULL); // remove observer
    this->SetScannerConnectorNodeID(node->ScannerConnectorNodeID);
    this->SetAndObserveZFrameModelNodeID(NULL); // remove observer
    this->SetZFrameModelNodeID(node->ZFrameModelNodeID);
    this->SetAndObserveZFrameTransformNodeID(NULL); // remove observer
    this->SetZFrameTransformNodeID(node->ZFrameTransformNodeID);
    this->SetAndObserveTemplateModelNodeID(NULL); // remove observer
    this->SetTemplateModelNodeID(node->TemplateModelNodeID);
    //this->SetTemplateTransformNodeID(node->TemplateTransformNodeID);
    this->SetAndObserveActiveNeedleModelNodeID(NULL); // remove observer
    this->SetActiveNeedleModelNodeID(node->ActiveNeedleModelNodeID);
    this->SetAndObserveActiveNeedleTransformNodeID(NULL); // remove observer
    this->SetActiveNeedleTransformNodeID(node->ActiveNeedleTransformNodeID);
    }
  else
    {
    vtkErrorMacro("Invalid node");
    }

  this->StatusDescriptors.clear();
  for (unsigned int i=0; i<node->StatusDescriptors.size(); i++)
    {
    this->StatusDescriptors.push_back(node->StatusDescriptors[i]);
    }

  this->EndModify(disabledModify);
}

//-----------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (this->ImagingPlaneTransformNodeID != NULL && this->Scene->GetNodeByID(this->ImagingPlaneTransformNodeID) == NULL)
    {
    this->SetAndObserveImagingPlaneTransformNodeID(NULL);
    }
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
  if (this->TemplateModelNodeID != NULL && this->Scene->GetNodeByID(this->TemplateModelNodeID) == NULL &&
      this->TemplateIntersectionModelNodeID != NULL && this->Scene->GetNodeByID(this->TemplateIntersectionModelNodeID) == NULL)
    {
    this->SetAndObserveTemplateModelNodeID(NULL);
    }
  //if (this->TemplateTransformNodeID != NULL && this->Scene->GetNodeByID(this->TemplateTransformNodeID) == NULL)
  //  {
  //  this->SetAndObserveTemplateTransformNodeID(NULL);
  //  }
  if (this->ActiveNeedleModelNodeID != NULL && this->Scene->GetNodeByID(this->ActiveNeedleModelNodeID) == NULL)
    {
    this->SetAndObserveActiveNeedleModelNodeID(NULL);
    }
  if (this->ActiveNeedleTransformNodeID != NULL && this->Scene->GetNodeByID(this->ActiveNeedleTransformNodeID) == NULL)
    {
    this->SetAndObserveActiveNeedleTransformNodeID(NULL);
    }

}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);

  if (this->ImagingPlaneTransformNodeID && !strcmp(oldID, this->ImagingPlaneTransformNodeID))
    {
    this->SetAndObserveImagingPlaneTransformNodeID(newID);
    }
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
  if (this->TemplateModelNodeID && !strcmp(oldID, this->TemplateModelNodeID))
    {
    this->SetAndObserveTemplateModelNodeID(newID);
    }
  //if (this->TemplateTransformNodeID && !strcmp(oldID, this->TemplateTransformNodeID))
  //  {
  //  this->SetAndObserveTemplateTransformNodeID(newID);
  //  }

  if (this->ActiveNeedleModelNodeID && !strcmp(oldID, this->ActiveNeedleModelNodeID))
    {
    this->SetAndObserveActiveNeedleModelNodeID(newID);
    }
  if (this->ActiveNeedleTransformNodeID && !strcmp(oldID, this->ActiveNeedleTransformNodeID))
    {
    this->SetAndObserveActiveNeedleTransformNodeID(newID);
    }

}

//-----------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::UpdateScene(vtkMRMLScene *scene)
{
  Superclass::UpdateScene(scene);

  this->SetAndObserveImagingPlaneTransformNodeID(this->GetImagingPlaneTransformNodeID());
  this->SetAndObserveScannerConnectorNodeID(this->GetScannerConnectorNodeID());
  this->SetAndObserveZFrameModelNodeID(this->GetZFrameModelNodeID());
  this->SetAndObserveZFrameTransformNodeID(this->GetZFrameTransformNodeID());
  this->SetAndObserveTemplateModelNodeID(this->GetTemplateModelNodeID());
  //this->SetAndObserveTemplateTransformNodeID(this->GetTemplateTransformNodeID());
  this->SetAndObserveActiveNeedleModelNodeID(this->GetActiveNeedleModelNodeID());
  this->SetAndObserveActiveNeedleTransformNodeID(this->GetActiveNeedleTransformNodeID());
}

//-----------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
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
void vtkMRMLTransPerinealProstateCryoTemplateNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


int vtkMRMLTransPerinealProstateCryoTemplateNode::Init(vtkSlicerApplication* app, const char* moduleShareDir)
{
  this->Superclass::Init(app, moduleShareDir);

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


  //// Template transform node
  //if (GetTemplateTransformNode()==NULL)
  //  {
  //  vtkMRMLLinearTransformNode* ttnode = vtkMRMLLinearTransformNode::New();
  //  ttnode->SetName("TemplateTransform");
  //  vtkMatrix4x4* ztransform = vtkMatrix4x4::New();
  //  ztransform->Identity();
  //  ztransform->SetElement(0, 3, this->TemplateOffset[0]);
  //  ztransform->SetElement(1, 3, this->TemplateOffset[1]);
  //  ztransform->SetElement(2, 3, this->TemplateOffset[2]);
  //
  //  //transformNode->SetAndObserveImageData(transform);
  //  ttnode->ApplyTransform(ztransform);
  //  ttnode->SetScene(this->Scene);
  //  ztransform->Delete();
  //  this->Scene->AddNode(ttnode);
  //  SetAndObserveTemplateTransformNodeID(ttnode->GetID());
  //
  //  if (GetZFrameTransformNodeID())
  //    {
  //    ttnode->SetAndObserveTransformNodeID(GetZFrameTransformNodeID());
  //    }
  //}

  // Template model
  // This part should be moved to Robot Display Node.

  if (GetTemplateModelNode()==NULL)
    {
    const char* nodeID = AddTemplateModel("CryoTemplate");
    vtkMRMLModelNode*  modelNode = vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(nodeID));
    vtkMRMLModelNode*  intersectionModelNode = NULL;
    if (this->TemplateIntersectionModelNodeID)
      {
      intersectionModelNode = vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(this->TemplateIntersectionModelNodeID));
      }

    if (modelNode && intersectionModelNode)
      {
      vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
      displayNode->SetVisibility(0);
      modelNode->Modified();
      this->Scene->Modified();
      modelNode->SetAndObserveTransformNodeID(GetZFrameTransformNodeID());
      intersectionModelNode->SetAndObserveTransformNodeID(GetZFrameTransformNodeID());
      SetAndObserveTemplateModelNodeID(nodeID);
      }
    }

  // Active needle transform node
/*
  if (GetActiveNeedleTransformNode()==NULL)
  {
  vtkMRMLLinearTransformNode* atnode = vtkMRMLLinearTransformNode::New();
  atnode->SetName("ActiveNeedleTransform");
  vtkMatrix4x4* ztransform = vtkMatrix4x4::New();
  ztransform->Identity();
  //transformNode->SetAndObserveImageData(transform);
  atnode->ApplyTransform(ztransform);
  atnode->SetScene(this->Scene);
  ztransform->Delete();
  this->Scene->AddNode(atnode);
  SetAndObserveActiveNeedleTransformNodeID(atnode->GetID());
  }

  // Active Needle model
  // This part should be moved to Robot Display Node.
  if (GetActiveNeedleModelNode()==NULL)
  {
  const char* nodeID = AddNeedleModel("ActiveNeedle", 200.0, 4.0);
  vtkMRMLModelNode*  modelNode = vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(nodeID));
  if (modelNode)
  {
  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
  displayNode->SetVisibility(0);
  modelNode->Modified();
  this->Scene->Modified();
  modelNode->SetAndObserveTransformNodeID(GetActiveNeedleTransformNodeID());
  SetAndObserveActiveNeedleModelNodeID(nodeID);
  }
  }
*/

  // For Real-time MRI
  // Imaging plane
  if (GetImagingPlaneTransformNodeID() == NULL)
    {
    vtkMRMLLinearTransformNode* itnode = vtkMRMLLinearTransformNode::New();
    itnode->SetName("PLANE_0");
    vtkMatrix4x4* transform = vtkMatrix4x4::New();
    transform->Identity();
    itnode->ApplyTransform(transform);
    itnode->SetScene(this->Scene);
    transform->Delete();
    this->Scene->AddNode(itnode);
    this->SetImagingPlaneTransformNodeID(itnode->GetID());
    //SetAndObserveActiveNeedleTransformNodeID(itnode->GetID());
    }


  return 1;
}


int vtkMRMLTransPerinealProstateCryoTemplateNode::OnTimer()
{
  return 1;
}


//---------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::SendZFrame()
{

  std::cerr << "int vtkProstateNavLogic::SendZFrame(): " << this->GetZFrameTransformNodeID() << std::endl;

  return 1;

}


//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::AddWorkspaceModel(const char* nodeName)
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
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::AddZFrameModel(const char* nodeName)
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
  //trans2->RotateX(-45.0);
  trans2->RotateX(45.0);
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
  trans4->Translate(0.0, length/2.0, 0.0);
  trans4->RotateX(90.0);
  //trans4->RotateZ(-45.0);
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
  //trans6->RotateX(45.0);
  trans6->RotateX(-45.0);
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
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::AddTemplateModel(const char* nodeName)
{
  // Regular template
  const char* templateID = this->GenerateTemplateModel(nodeName);

  // Intersection template
  const char* templateIntersectionID = this->GenerateTemplateModel(nodeName, 1);
  this->SetTemplateIntersectionModelNodeID(templateIntersectionID);

  return templateID;
}

//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::GenerateTemplateModel(const char* nodeName, int intersectionModel)
{

  double length = TEMPLATE_DEPTH;
  bool visibility = true;
  std::stringstream newNodeName;
  newNodeName << nodeName;
  if (intersectionModel)
    {
    visibility = false;
    length = TEMPLATE_INTERSECTION_DEPTH;
    newNodeName << "-crossSection";
    }

  vtkMRMLModelNode           *model;
  vtkMRMLModelDisplayNode    *disp;

  model = vtkMRMLModelNode::New();
  disp  = vtkMRMLModelDisplayNode::New();

  this->Scene->SaveStateForUndo();
  this->Scene->AddNode(disp);
  this->Scene->AddNode(model);

  disp->SetScene(this->Scene);
  model->SetName(newNodeName.str().c_str());
  model->SetScene(this->Scene);
  model->SetAndObserveDisplayNodeID(disp->GetID());
  model->SetHideFromEditors(0);

  vtkAppendPolyData *apd = vtkAppendPolyData::New();

  // Even lines template
  for (int i = 0; i < (this->TemplateNumGrids[1]+1)/2; ++i)
    {
    for (int j = 0; j < (this->TemplateNumGrids[0]-1); ++j)
      {
      double offset[3];
      offset[0] = this->TemplateOffset[0] + this->TemplateGridPitch[0]/2 + j*this->TemplateGridPitch[0];
      offset[1] = this->TemplateOffset[1] + i*this->TemplateGridPitch[1]*2;
      offset[2] = this->TemplateOffset[2] + length/2;

      vtkCylinderSource *cylinder = vtkCylinderSource::New();
      cylinder->SetResolution(24);
      cylinder->SetRadius(TEMPLATE_HOLE_RADIUS);
      cylinder->SetHeight(length);
      cylinder->SetCenter(0, 0, 0);
      cylinder->Update();

      vtkTransform* trans =   vtkTransform::New();
      trans->Translate(offset);
      trans->RotateX(90.0);
      trans->Update();

      vtkTransformPolyDataFilter *tfilter = vtkTransformPolyDataFilter::New();
      tfilter->SetInput(cylinder->GetOutput());
      tfilter->SetTransform(trans);
      tfilter->Update();

      apd->AddInput(tfilter->GetOutput());
      apd->Update();

      cylinder->Delete();
      trans->Delete();
      tfilter->Delete();
      }
    }

  // Odd lines template
  for (int i = 0; i < (this->TemplateNumGrids[1]-1)/2; ++i)
    {
    for (int j = 0; j < this->TemplateNumGrids[0]; ++j)
      {
      double offset[3];
      offset[0] = this->TemplateOffset[0] + j*this->TemplateGridPitch[0];
      offset[1] = this->TemplateOffset[1] + this->TemplateGridPitch[1] + i*this->TemplateGridPitch[1]*2;
      offset[2] = this->TemplateOffset[2] + length/2;

      vtkCylinderSource *cylinder = vtkCylinderSource::New();
      cylinder->SetResolution(24);
      cylinder->SetRadius(TEMPLATE_HOLE_RADIUS);
      cylinder->SetHeight(length);
      cylinder->SetCenter(0, 0, 0);
      cylinder->Update();

      vtkTransform* trans =   vtkTransform::New();
      trans->Translate(offset);
      trans->RotateX(90.0);
      trans->Update();

      vtkTransformPolyDataFilter *tfilter = vtkTransformPolyDataFilter::New();
      tfilter->SetInput(cylinder->GetOutput());
      tfilter->SetTransform(trans);
      tfilter->Update();

      apd->AddInput(tfilter->GetOutput());
      apd->Update();

      cylinder->Delete();
      trans->Delete();
      tfilter->Delete();
      }
    }

  // Horizontal template lines
  for (int i = 4; i < this->TemplateNumGrids[1]; i+=5)
    {
    double offset[3];
    offset[0] = this->TemplateOffset[0] + ((this->TemplateNumGrids[0]-1)/2)*this->TemplateGridPitch[0];
    offset[1] = this->TemplateOffset[1] + i*this->TemplateGridPitch[1];
    offset[2] = this->TemplateOffset[2] + length/2;

    vtkCubeSource* cube = vtkCubeSource::New();
    cube->SetXLength(TEMPLATE_HOLE_RADIUS/3);
    cube->SetYLength(std::fabs((this->TemplateNumGrids[1]-1)*this->TemplateGridPitch[1]));
    cube->SetZLength(length);
    cube->SetCenter(0,0,0);
    cube->Update();

    vtkTransform* trans = vtkTransform::New();
    trans->Translate(offset);
    trans->RotateZ(90);
    trans->Update();

    vtkTransformPolyDataFilter *tFilter = vtkTransformPolyDataFilter::New();
    tFilter->SetInput(cube->GetOutput());
    tFilter->SetTransform(trans);
    tFilter->Update();

    apd->AddInput(tFilter->GetOutput());
    apd->Update();

    cube->Delete();
    trans->Delete();
    tFilter->Delete();
    }

  // Vertical template lines
  for (int i = 4; i < this->TemplateNumGrids[0]-1; i+=4)
    {
    double offset[3];
    offset[0] = this->TemplateOffset[0] + i*this->TemplateGridPitch[0];
    offset[1] = this->TemplateOffset[1] + ((this->TemplateNumGrids[1]-1)/2)*this->TemplateGridPitch[1];
    offset[2] = this->TemplateOffset[2] + length/2;

    vtkCubeSource* cube = vtkCubeSource::New();
    cube->SetXLength(TEMPLATE_HOLE_RADIUS/3);
    cube->SetYLength(std::fabs((this->TemplateNumGrids[1]-1)*this->TemplateGridPitch[1]));
    cube->SetZLength(length);
    cube->SetCenter(0,0,0);
    cube->Update();

    vtkTransform* trans = vtkTransform::New();
    trans->Translate(offset);
    trans->Update();

    vtkTransformPolyDataFilter *tFilter = vtkTransformPolyDataFilter::New();
    tFilter->SetInput(cube->GetOutput());
    tFilter->SetTransform(trans);
    tFilter->Update();

    apd->AddInput(tFilter->GetOutput());
    apd->Update();

    cube->Delete();
    trans->Delete();
    tFilter->Delete();
    }

  model->SetAndObservePolyData(apd->GetOutput());

  double color[3];
  color[0] = 0.2;
  color[1] = 0.2;
  color[2] = 1.0;

  disp->SetPolyData(model->GetPolyData());
  disp->SetColor(color);
  disp->SetOpacity(0.5);
  disp->SetVisibility(visibility);

  apd->Delete();

  const char* modelID = model->GetID();

  disp->Delete();
  model->Delete();

  return modelID;
}

//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::AddNeedleModel(const char* nodeName, double length, double diameter)
{
  double needleColor[3] = {1.0, 0.0, 0.0};
  double needleLength = NEEDLE_LENGTH;
  double coneHeight = 5.0;

  vtkSmartPointer<vtkMRMLModelNode> needleModel =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  vtkSmartPointer<vtkMRMLModelDisplayNode> needleDisp =
    vtkSmartPointer<vtkMRMLModelDisplayNode>::New();

  this->Scene->SaveStateForUndo();
  this->Scene->AddNode(needleDisp.GetPointer());
  this->Scene->AddNode(needleModel.GetPointer());

  needleDisp->SetScene(this->Scene);
  needleModel->SetName(nodeName);
  needleModel->SetScene(this->Scene);
  needleModel->SetAndObserveDisplayNodeID(needleDisp->GetID());
  needleModel->SetHideFromEditors(0);

  const char* modelID = needleModel->GetID();

  vtkSmartPointer<vtkConeSource> cone =
    vtkSmartPointer<vtkConeSource>::New();
  cone->SetRadius(1.5);
  cone->SetHeight(coneHeight);
  cone->SetResolution(6);
  cone->SetCenter(0,0,0);
  cone->CappingOff();
  cone->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> cfilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> ctrans =
    vtkSmartPointer<vtkTransform>::New();
  ctrans->RotateY(-90);
  ctrans->RotateX(30);
  ctrans->Translate(-coneHeight/2, 0.0, 0.0);
  cfilter->SetInput(cone->GetOutput());
  cfilter->SetTransform(ctrans.GetPointer());
  cfilter->Update();

  // Cylinder represents the locator stick
  vtkSmartPointer<vtkCylinderSource> cylinder =
    vtkSmartPointer<vtkCylinderSource>::New();
  cylinder->SetRadius(1.5);
  cylinder->SetHeight(needleLength);
  cylinder->SetCenter(0, 0, 0);
  cylinder->Update();

  // Rotate cylinder
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> ttrans =
    vtkSmartPointer<vtkTransform>::New();
  ttrans->RotateX(90.0);
  ttrans->Translate(0.0, -coneHeight-needleLength/2, 0.0);
  ttrans->Update();
  tfilter->SetInput(cylinder->GetOutput());
  tfilter->SetTransform(ttrans.GetPointer());
  tfilter->Update();

  vtkSmartPointer<vtkAppendPolyData> apd =
    vtkSmartPointer<vtkAppendPolyData>::New();
  apd->AddInput(cfilter->GetOutput());
  apd->AddInput(tfilter->GetOutput());
  apd->Update();

  vtkSmartPointer<vtkTriangleFilter> cleaner =
    vtkSmartPointer<vtkTriangleFilter>::New();
  cleaner->SetInputConnection(apd->GetOutputPort());

  needleModel->SetAndObservePolyData(cleaner->GetOutput());
  needleDisp->SetPolyData(needleModel->GetPolyData());
  needleDisp->SetColor(needleColor);
  needleDisp->SetOpacity(0.5);
  needleDisp->SetVisibility(0);
  needleDisp->SetSliceIntersectionVisibility(0);

  this->AddIceBallModel(modelID);

  return modelID;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::UpdateIceBallModel(const char* needleID)
{
  if (!this->Scene || !needleID)
    {
    return;
    }

  vtkMRMLModelNode* needleModel =
    vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(needleID));
  if (!needleModel)
    {
    return;
    }

  // Choose iceball properties based on needle type
  double needleRadius[3];
  this->GetIceBallRadius(needleRadius);

  const char* cryoBallID = needleModel->GetAttribute("CryoBallID");
  if (!cryoBallID)
    {
    // Every needle should have a ice ball associated when created
    // Should we create a new one if it's not the case ?
    return;
    }

  vtkMRMLNode* cryoBallModel = this->Scene->GetNodeByID(cryoBallID);
  if (cryoBallModel)
    {
    this->Scene->RemoveNode(cryoBallModel);
    }

  vtkMRMLLinearTransformNode* needleTransform =
    vtkMRMLLinearTransformNode::SafeDownCast(needleModel->GetParentTransformNode());
  if (needleTransform)
    {
    this->AddIceBallModel(needleID, needleTransform);
    }
}

//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::AddIceBallModel(const char* needleID,
                                                                          vtkMRMLLinearTransformNode* parentTransform)
{
  if (!this->Scene || !needleID)
    {
    return NULL;
    }

  vtkMRMLModelNode* needleNode =
    vtkMRMLModelNode::SafeDownCast(this->Scene->GetNodeByID(needleID));
  if (!needleNode)
    {
    return NULL;
    }
  vtkMRMLModelDisplayNode* needleDisplayNode = needleNode->GetModelDisplayNode();

  // Iceball tip is located 5mm above needle tip
  double cryoBallOffset = 5.0;
  double cryoBallColor[3] = {0.0, 1.0, 0.0};
  double cryoBallRadius[3];
  this->GetIceBallRadius(cryoBallRadius);

  vtkSmartPointer<vtkMRMLModelNode> cryoBallModel =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  vtkSmartPointer<vtkMRMLModelDisplayNode> cryoBallDisp =
    vtkSmartPointer<vtkMRMLModelDisplayNode>::New();

  this->Scene->SaveStateForUndo();
  this->Scene->AddNode(cryoBallModel.GetPointer());
  this->Scene->AddNode(cryoBallDisp.GetPointer());

  const char* cryoBallID = cryoBallModel->GetID();

  cryoBallDisp->SetScene(this->Scene);
  cryoBallModel->SetName("CryoBall");
  cryoBallModel->SetScene(this->Scene);
  cryoBallModel->SetAndObserveDisplayNodeID(cryoBallDisp->GetID());
  cryoBallModel->SetHideFromEditors(0);

  vtkSmartPointer<vtkParametricEllipsoid> ellipsoid =
    vtkSmartPointer<vtkParametricEllipsoid>::New();
  ellipsoid->SetXRadius(cryoBallRadius[0]);
  ellipsoid->SetYRadius(cryoBallRadius[1]);
  ellipsoid->SetZRadius(cryoBallRadius[2]);

  vtkSmartPointer<vtkParametricFunctionSource> ellipsoidSource =
    vtkSmartPointer<vtkParametricFunctionSource>::New();
  ellipsoidSource->SetParametricFunction(ellipsoid.GetPointer());
  ellipsoidSource->SetScalarModeToV();

  vtkSmartPointer<vtkTransformPolyDataFilter> efilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> etrans =
    vtkSmartPointer<vtkTransform>::New();
  etrans->Translate(0.0, 0.0, -ellipsoid->GetZRadius()+cryoBallOffset);
  etrans->Update();
  efilter->SetInput(ellipsoidSource->GetOutput());
  efilter->SetTransform(etrans.GetPointer());
  efilter->Update();

  cryoBallModel->SetAndObservePolyData(efilter->GetOutput());
  cryoBallDisp->SetPolyData(cryoBallModel->GetPolyData());
  cryoBallDisp->SetColor(cryoBallColor);
  cryoBallDisp->SetOpacity(0.5);
  cryoBallDisp->SetVisibility(needleDisplayNode->GetVisibility());
  cryoBallDisp->SetSliceIntersectionVisibility(needleDisplayNode->GetSliceIntersectionVisibility());

  needleNode->SetAttribute("CryoBallID", cryoBallID);

  if (parentTransform != NULL)
    {
    const char* parentTransformID = parentTransform->GetID();
    if (parentTransformID)
      {
      cryoBallModel->SetAndObserveTransformNodeID(parentTransformID);
      }
    }

  return cryoBallID;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::GetIceBallRadius(double* needleRadius)
{
  switch(this->NeedleType)
    {
    // Hide ice ball
    case 0:
      needleRadius[0] = 0.0;
      needleRadius[1] = 0.0;
      needleRadius[2] = 0.0;
      break;

    // IceSeed
    case 1:
      needleRadius[0] = 20.0 / 2.0;
      needleRadius[1] = 20.0 / 2.0;
      needleRadius[2] = 25.0 / 2.0;
      break;

    // IceRod
    case 2:
      needleRadius[0] = 25.0 / 2.0;
      needleRadius[1] = 25.0 / 2.0;
      needleRadius[2] = 35.0 / 2.0;
      break;

    // IceSeed by default
    default:
      needleRadius[0] = 20.0 / 2.0;
      needleRadius[1] = 20.0 / 2.0;
      needleRadius[2] = 25.0 / 2.0;
      break;
    }
}

//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::PerformRegistration(vtkMRMLScalarVolumeNode* volumeNode)
{
  vtkZFrameRobotToImageRegistration* registration = vtkZFrameRobotToImageRegistration::New();
  registration->SetFiducialVolume(volumeNode);

  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->Scene->GetNodeByID(this->GetZFrameTransformNodeID()));
  if (transformNode != NULL)
    {
    registration->SetRobotToImageTransform(transformNode);
    registration->DoRegistration();
    }

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::PerformRegistration(vtkMRMLScalarVolumeNode* volumeNode, int param1, int param2)
{
  vtkZFrameRobotToImageRegistration* registration = vtkZFrameRobotToImageRegistration::New();
  registration->SetFiducialVolume(volumeNode);

  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->Scene->GetNodeByID(this->GetZFrameTransformNodeID()));
  if (transformNode != NULL)
    {
    registration->SetRobotToImageTransform(transformNode);
    registration->SetSliceRange(param1, param2);
    registration->DoRegistration();
    }

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::MoveTo(const char *transformNodeId)
{
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(this->Scene->GetNodeByID(transformNodeId));
  vtkMatrix4x4* transform = transformNode->GetMatrixTransformToParent();
  double targetX = transform->GetElement(0, 3);
  double targetY = transform->GetElement(1, 3);
  double targetZ = transform->GetElement(2, 3);
  int i;
  int j;
  double depth;
  double errorX;
  double errorY;
  double errorZ;

  FindHole(targetX, targetY, targetZ, i, j, depth, errorX, errorY, errorZ);
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  GetHoleTransform(i, j, matrix);

  vtkMRMLLinearTransformNode* needleTransformNode =
    vtkMRMLLinearTransformNode::SafeDownCast(this->Scene->GetNodeByID(this->GetActiveNeedleTransformNodeID()));
  if (needleTransformNode != NULL)
    {
    vtkMatrix4x4* needleTransform = needleTransformNode->GetMatrixTransformToParent();
    double needleOffset[4];
    double needleTip[4];
    needleOffset[0] = 0.0;
    needleOffset[1] = 0.0;
    needleOffset[2] = depth;
    needleOffset[3] = 1.0;
    matrix->MultiplyPoint(needleOffset, needleTip);
    needleTransform->DeepCopy(matrix);
    needleTransform->SetElement(0, 3, needleTip[0]);
    needleTransform->SetElement(1, 3, needleTip[1]);
    needleTransform->SetElement(2, 3, needleTip[2]);
    needleTransformNode->Modified();

    // Substitute hole information (for secondary display)
    this->FlagNeedleInfomation = 1;
    this->NeedleDepth  = depth;
    this->GridIndex[0] = i;
    this->GridIndex[1] = j;
    this->TargetRAS[0] = targetX;
    this->TargetRAS[1] = targetY;
    this->TargetRAS[2] = targetZ;
    this->ExpectedTargetError[0] = errorX;
    this->ExpectedTargetError[1] = errorY;
    this->ExpectedTargetError[2] = errorZ;

    // For Real-time MRI
    vtkMRMLLinearTransformNode* planeTransformNode =
      vtkMRMLLinearTransformNode::SafeDownCast(this->Scene->GetNodeByID(this->GetImagingPlaneTransformNodeID()));

    if (planeTransformNode != NULL)
      {
      vtkMatrix4x4* planeTransform = planeTransformNode->GetMatrixTransformToParent();
      planeTransform->SetElement(0, 3, needleTip[0]);
      planeTransform->SetElement(1, 3, needleTip[1]);
      planeTransform->SetElement(2, 3, needleTip[2]);
      planeTransformNode->Modified();
      }

    Modified();
    }

  return 1;
}


//----------------------------------------------------------------------------
const char* vtkMRMLTransPerinealProstateCryoTemplateNode::GetTargetReport(vtkProstateNavTargetDescriptor* targetDesc)
{
  if (!targetDesc)
    {
    return NULL;
    }
  double* xyz=targetDesc->GetRASLocation();

  double targetX = xyz[0];
  double targetY = xyz[1];
  double targetZ = xyz[2];
  int i;
  int j;
  double depth;
  double errorX;
  double errorY;
  double errorZ;

  FindHole(targetX, targetY, targetZ, i, j, depth, errorX, errorY, errorZ);
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  GetHoleTransform(i, j, matrix);
  int j_corrected = j - ((i%2 == 0 && j>7)? 7: 8);

  std::ostringstream ss;
  //ss << "Grid:   (" << i << ", " << (char) ('A' + j) << ")" << std::endl;
  ss << "Name:   " << targetDesc->GetName() << std::endl;
  ss << "Grid:   (" << (char) ('A' + i) << ", " << (j_corrected) << ")" << std::endl;
  ss << "Depth:  " << depth << " mm" << std::endl;
  ss << "Target: R=" << targetX << ", A=" << targetY << ", S=" << targetZ << std::endl;
  ss << "Error:  R=" << errorX  << ", A=" << errorY  << ", S=" << errorZ  << std::endl;
  //SetScreenMessage(ss.str().c_str());

  return ss.str().c_str();
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SwitchStep(const char *stepName)
{
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveScannerConnectorNodeID(const char *nodeId)
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

  if (this->GetScannerConnectorNode())
    {
    this->GetScannerConnectorNode()->RegisterOutgoingMRMLNode(this->GetImagingPlaneTransformNode());
    }

  this->Modified();

}


//----------------------------------------------------------------------------
vtkMRMLIGTLConnectorNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetScannerConnectorNode()
{
  if (this->GetScene() && this->ScannerConnectorNodeID != NULL )
    {
    return vtkMRMLIGTLConnectorNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ScannerConnectorNodeID));
    }
  return NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveZFrameModelNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ZFrameModelNode, NULL);
  this->SetZFrameModelNodeID(nodeId);
  vtkMRMLModelNode *tnode = this->GetZFrameModelNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ZFrameModelNode, tnode, events);
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetZFrameModelNode()
{
  if (this->GetScene() && this->ZFrameModelNodeID != NULL )
    {
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ZFrameModelNodeID));
    }
  return NULL;
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveWorkspaceModelNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->WorkspaceModelNode, NULL);
  this->SetWorkspaceModelNodeID(nodeId);
  vtkMRMLModelNode *tnode = this->GetWorkspaceModelNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->WorkspaceModelNode, tnode, events);
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetWorkspaceModelNode()
{
  if (this->GetScene() && this->WorkspaceModelNodeID != NULL )
    {
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->WorkspaceModelNodeID));
    }
  return NULL;
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveZFrameTransformNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ZFrameTransformNode, NULL);
  this->SetZFrameTransformNodeID(nodeId);
  vtkMRMLLinearTransformNode *tnode = this->GetZFrameTransformNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ZFrameTransformNode, tnode, events);
}


//----------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetZFrameTransformNode()
{
  if (this->GetScene() && this->ZFrameTransformNodeID != NULL )
    {
    return vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ZFrameTransformNodeID));
    }
  return NULL;
}

////----------------------------------------------------------------------------
//void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveTemplateTransformNodeID(const char *nodeId)
//{
//  vtkSetAndObserveMRMLObjectMacro(this->TemplateTransformNode, NULL);
//  this->SetTemplateTransformNodeID(nodeId);
//  vtkMRMLLinearTransformNode *tnode = this->GetTemplateTransformNode();
//  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
//  events->InsertNextValue(vtkCommand::ModifiedEvent);
//  vtkSetAndObserveMRMLObjectEventsMacro(this->TemplateTransformNode, tnode, events);
//}


////----------------------------------------------------------------------------
//vtkMRMLLinearTransformNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetTemplateTransformNode()
//{
//  if (this->GetScene() && this->TemplateTransformNodeID != NULL )
//    {
//    return vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetNodeByID(this->TemplateTransformNodeID));
//    }
//  return NULL;
//}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveTemplateModelNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->TemplateModelNode, NULL);
  this->SetTemplateModelNodeID(nodeId);
  vtkMRMLModelNode *tnode = this->GetTemplateModelNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->TemplateModelNode, tnode, events);

  if (this->TemplateIntersectionModelNode)
    {
    vtkSetAndObserveMRMLObjectMacro(this->TemplateIntersectionModelNode, NULL);
    vtkMRMLModelNode *tinode = this->GetTemplateIntersectionModelNode();
    vtkSetAndObserveMRMLObjectEventsMacro(this->TemplateIntersectionModelNode, tinode, events);
    }
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetTemplateModelNode()
{
  if (this->GetScene() && this->TemplateModelNodeID != NULL )
    {
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->TemplateModelNodeID));
    }
  return NULL;
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetTemplateIntersectionModelNode()
{
  if (this->GetScene() && this->TemplateIntersectionModelNodeID != NULL )
    {
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->TemplateIntersectionModelNodeID));
    }
  return NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveActiveNeedleModelNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ActiveNeedleModelNode, NULL);
  this->SetActiveNeedleModelNodeID(nodeId);
  vtkMRMLModelNode *tnode = this->GetActiveNeedleModelNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ActiveNeedleModelNode, tnode, events);
}

//----------------------------------------------------------------------------
vtkMRMLModelNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetActiveNeedleModelNode()
{
  if (this->GetScene() && this->ActiveNeedleModelNodeID != NULL )
    {
    return vtkMRMLModelNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ActiveNeedleModelNodeID));
    }
  return NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveActiveNeedleTransformNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ActiveNeedleTransformNode, NULL);
  this->SetActiveNeedleTransformNodeID(nodeId);
  vtkMRMLLinearTransformNode *tnode = this->GetActiveNeedleTransformNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ActiveNeedleTransformNode, tnode, events);
}


//----------------------------------------------------------------------------
void vtkMRMLTransPerinealProstateCryoTemplateNode::SetAndObserveImagingPlaneTransformNodeID(const char *nodeId)
{
  vtkSetAndObserveMRMLObjectMacro(this->ImagingPlaneTransformNode, NULL);
  this->SetImagingPlaneTransformNodeID(nodeId);
  vtkMRMLLinearTransformNode *tnode = this->GetImagingPlaneTransformNode();
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ImagingPlaneTransformNode, tnode, events);
}


//----------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetActiveNeedleTransformNode()
{
  if (this->GetScene() && this->ActiveNeedleTransformNodeID != NULL )
    {
    return vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ActiveNeedleTransformNodeID));
    }
  return NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLinearTransformNode* vtkMRMLTransPerinealProstateCryoTemplateNode::GetImagingPlaneTransformNode()
{
  if (this->GetScene() && this->ImagingPlaneTransformNodeID != NULL )
    {
    return vtkMRMLLinearTransformNode::SafeDownCast(this->GetScene()->GetNodeByID(this->ImagingPlaneTransformNodeID));
    }
  return NULL;
}


//----------------------------------------------------------------------------
std::string vtkMRMLTransPerinealProstateCryoTemplateNode::GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc, NeedleDescriptorStruct *needle)
{
  if (!targetDesc)
    {
    return "";
    }

  double* target = targetDesc->GetRASLocation();
  int i;
  int j;
  double depth;
  double errorX;
  double errorY;
  double errorZ;

  FindHole(target[0], target[1], target[2], i, j, depth, errorX, errorY, errorZ);
  int j_corrected = j - ((i%2 == 0 && j>7)? 7: 8);

  std::ostrstream os;
  os << "Target: " << targetDesc->GetName()<<std::endl;
  os << "Grid:   (" << (char) ('A' + i) << ", " << (j_corrected) << ")" << std::endl;
  os << "Depth:  " << depth << " mm" << std::endl;
  os << "Target: R=" << target[0] << ", A=" << target[1] << ", S=" << target[2] << std::endl;
  os << "Error:  R=" << errorX  << ", A=" << errorY  << ", S=" << errorZ  << std::endl;
  os << std::ends;
  //SetScreenMessage(ss.str().c_str());

  std::string result=os.str();
  os.rdbuf()->freeze();

  // :TODO: construct a string that contains useful information for the active target (reachable, etc.)
  return result;
}


//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::GetHoleTransform(int i, int j, vtkMatrix4x4* matrix)
{
  if (matrix == NULL)
    {
    return 0;
    }

  int nOfHoles = this->TemplateNumGrids[0];
  if (i % 2 == 0)
    {
    nOfHoles--;
    }

  if (i < 0 || i >= this->TemplateNumGrids[1] ||
      j < 0 || j >= nOfHoles)
    {
    // the grid index is out of range
    return 0;
    }

  vtkMRMLLinearTransformNode *tnode = this->GetZFrameTransformNode();
  if (tnode == NULL)
    {
    return 0;
    }

  double shiftingLineOffset = 0.0;
  if (i % 2 == 0)
    {
    shiftingLineOffset = this->TemplateGridPitch[0]/2;
    }

  // offset from the Z-frame center
  double off[4];
  off[0] = this->TemplateOffset[0] + shiftingLineOffset + j * this->TemplateGridPitch[0];
  off[1] = this->TemplateOffset[1] + i * this->TemplateGridPitch[1];
  off[2] = this->TemplateOffset[2];
  off[3] = 1.0;

  vtkMatrix4x4* transform = tnode->GetMatrixTransformToParent();
  double coord[4];
  transform->MultiplyPoint(off, coord);

  matrix->DeepCopy(transform);
  matrix->SetElement(0, 3, coord[0]);
  matrix->SetElement(1, 3, coord[1]);
  matrix->SetElement(2, 3, coord[2]);
  matrix->SetElement(3, 3, 1.0);

  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::GetNeedleTransform(int i, int j, double length, vtkMatrix4x4* matrix)
{
  if (matrix == NULL)
    {
    return 0;
    }

  return 0; // not implemented
}


//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::FindHole(double targetX, double targetY, double targetZ,
                                                           int& nearest_i, int& nearest_j, double& nearest_depth,
                                                           double& errorX, double& errorY, double& errorZ)
{

  vtkMatrix4x4* matrix = vtkMatrix4x4::New();

  // NOTE: To find the hole for targeting, we will use the fact that
  // the optimal hole is nearest to the target. (It's not the best way, though...)
  double holeX;
  double holeY;
  double holeZ;

  nearest_i = 0;
  nearest_j = 0;
  GetHoleTransform(nearest_i, nearest_j, matrix);
  holeX = matrix->GetElement(0, 3);
  holeY = matrix->GetElement(1, 3);
  holeZ = matrix->GetElement(2, 3);
  nearest_depth = sqrt((holeX-targetX)*(holeX-targetX)+(holeY-targetY)*(holeY-targetY)+(holeZ-targetZ)*(holeZ-targetZ));

  for (int i = 0; i < this->TemplateNumGrids[1]; ++i)
    {
    int nOfHoles = this->TemplateNumGrids[0];
    if (i % 2 == 0)
      {
      nOfHoles--;
      }

    for (int j = 0; j < nOfHoles; j ++)
      {
      GetHoleTransform(i, j, matrix);
      holeX = matrix->GetElement(0, 3);
      holeY = matrix->GetElement(1, 3);
      holeZ = matrix->GetElement(2, 3);
      double dist = sqrt((holeX-targetX)*(holeX-targetX)+(holeY-targetY)*(holeY-targetY)+(holeZ-targetZ)*(holeZ-targetZ));
      if (dist < nearest_depth)
        {
        nearest_depth = dist;
        nearest_i = i;
        nearest_j = j;
        }
      }
    }

  // use the distance as needle length
  double needleOffset[4];
  double needleTip[4];
  GetHoleTransform(nearest_i, nearest_j, matrix);
  needleOffset[0] = 0.0;
  needleOffset[1] = 0.0;
  needleOffset[2] = nearest_depth;
  needleOffset[3] = 1.0;
  matrix->MultiplyPoint(needleOffset, needleTip);

  // Calculate targeting error
  errorX = needleTip[0] - targetX;
  errorY = needleTip[1] - targetY;
  errorZ = needleTip[2] - targetZ;
  return 0;
}

//----------------------------------------------------------------------------
int vtkMRMLTransPerinealProstateCryoTemplateNode::GetHoleTransformWithDepth(int i, int j, double depth, vtkMatrix4x4* matrix)
{
  if (matrix == NULL)
    {
    return 0;
    }

  int nOfHoles = this->TemplateNumGrids[0];
  if (i % 2 == 0)
    {
    nOfHoles--;
    }

  if (i < 0 || i >= this->TemplateNumGrids[1] ||
      j < 0 || j >= nOfHoles)
    {
    // the grid index is out of range
    return 0;
    }

  vtkMRMLLinearTransformNode *tnode = this->GetZFrameTransformNode();
  if (tnode == NULL)
    {
    return 0;
    }

  double shiftingLineOffset = 0.0;
  if (i % 2 == 0)
    {
    shiftingLineOffset = this->TemplateGridPitch[0]/2;
    }

  // offset from the Z-frame center
  double off[4];
  off[0] = this->TemplateOffset[0] + shiftingLineOffset + j * this->TemplateGridPitch[0];
  off[1] = this->TemplateOffset[1] + i * this->TemplateGridPitch[1];
  off[2] = depth;
  off[3] = 1.0;

  vtkMatrix4x4* transform = tnode->GetMatrixTransformToParent();
  double coord[4];
  transform->MultiplyPoint(off, coord);

  matrix->DeepCopy(transform);
  matrix->SetElement(0, 3, coord[0]);
  matrix->SetElement(1, 3, coord[1]);
  matrix->SetElement(2, 3, coord[2]);
  matrix->SetElement(3, 3, 1.0);

  return 1;
}
