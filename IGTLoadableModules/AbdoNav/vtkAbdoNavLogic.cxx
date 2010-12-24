/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkAbdoNavLogic.h"

/* MRML includes */
#include "vtkMRMLLinearTransformNode.h"

/* VTK includes */
#include "vtkAppendPolyData.h"
#include "vtkCallbackCommand.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkRegularPolygonSource.h"
#include "vtkRotationalExtrusionFilter.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavLogic, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavLogic);


//---------------------------------------------------------------------------
vtkAbdoNavLogic::vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  //----------------------------------------------------------------
  this->AbdoNavNode = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void*>(this));
  this->DataCallbackCommand->SetCallback(vtkAbdoNavLogic::DataCallback);
  this->LocatorFreezePosition = NULL;
}


//---------------------------------------------------------------------------
vtkAbdoNavLogic::~vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Cleanup logic values.
  //----------------------------------------------------------------
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  if (this->LocatorFreezePosition)
    {
    this->LocatorFreezePosition->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkAbdoNavLogic: " << this->GetClassName() << "\n";
  os << indent << "vtkAbdoNavNode: " << this->AbdoNavNode << "\n";
  os << indent << "DataCallbackCommand: " << this->DataCallbackCommand << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  if (caller != NULL)
    {
    // vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    // TODO: fill in or delete!
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::DataCallback(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(eventid), void* clientData, void* vtkNotUsed(callData))
{
  // TODO: use (UpdateAll() isn't implemented) or delete!

  vtkAbdoNavLogic* self = reinterpret_cast<vtkAbdoNavLogic*>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkAbdoNavLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateAll()
{
  // TODO: implement or delete!
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkAbdoNavLogic::EnableLocatorDriver(const char* locatorName)
{
  // TODO: all TransformModifiedEvents within the scope of this function seem to be unnecessary --> remove?

  // create the locator model
  vtkMRMLModelNode* locatorModel = this->AddLocatorModel(locatorName, 0.0, 1.0, 1.0);
  // get the tracker transform node
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackerTransformNodeID()));
  // make locator model observe the tracker transform node
  if (locatorModel && tnode)
    {
    locatorModel->SetAndObserveTransformNodeID(tnode->GetID());
    locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
    }
  else
    {
    vtkErrorMacro("in vtkAbdoNavLogic::EnableLocatorDriver(...): "
                  "Enabling locator model failed!");
    }

  return locatorModel;
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ToggleLocatorVisibility(int vis)
{
  const char* locatorName = "AbdoNav-Locator";
  vtkMRMLModelNode* locatorModel;
  vtkMRMLDisplayNode* locatorDisplay;

  // check if locator already exists
  vtkCollection* collection = this->GetMRMLScene()->GetNodesByName(locatorName);
  if (collection != NULL && collection->GetNumberOfItems() == 0)
    {
    // locator doesn't exist yet, thus create it
    locatorModel = this->EnableLocatorDriver(locatorName);
    }
  else if (collection != NULL && collection->GetNumberOfItems() != 0)
    {
    // locator exists
    locatorModel = vtkMRMLModelNode::SafeDownCast(collection->GetItemAsObject(0));
    }
  // cleanup
  collection->Delete();

  // toggle visibility
  if (locatorModel)
    {
    locatorDisplay = locatorModel->GetDisplayNode();
    locatorDisplay->SetVisibility(vis);
    locatorModel->Modified();
    this->GetMRMLScene()->Modified();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ToggleLocatorUpdate(int freeze)
{
  // TODO: all TransformModifiedEvents within the scope of this function seem to be unnecessary --> remove?

  const char* locatorName = "AbdoNav-Locator";
  vtkMRMLModelNode* locatorModel;

  // check if locator already exists
  vtkCollection* collection = this->GetMRMLScene()->GetNodesByName(locatorName);
  if (collection != NULL && collection->GetNumberOfItems() == 0)
    {
    // locator doesn't exist, thus nothing to do
    return;
    }
  else if (collection != NULL && collection->GetNumberOfItems() != 0)
    {
    // locator exists
    locatorModel = vtkMRMLModelNode::SafeDownCast(collection->GetItemAsObject(0));
    if (freeze == 1 && locatorModel)
      {
      // freeze locator model
      locatorModel->SetAndObserveTransformNodeID(NULL);
      // need to retrieve, store and apply current transformation matrix:
      // otherwise, the locator model wouldn't be displayed at its current
      // position but would instead be reset to its initial position
      if (LocatorFreezePosition == NULL)
        {
        // LocatorFreezePosition is deleted in the destructor
        LocatorFreezePosition = vtkMatrix4x4::New();
        }
      // retrieve and store current transformation matrix
      LocatorFreezePosition->DeepCopy(vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackerTransformNodeID()))->GetMatrixTransformToParent());
      // apply current transformation matrix
      locatorModel->ApplyTransform(LocatorFreezePosition);
      locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      }
    else
      {
      // need to apply the inverse transformation matrix so as to obtain the identity matrix
      vtkMatrix4x4::Invert(LocatorFreezePosition, LocatorFreezePosition);
      locatorModel->ApplyTransform(LocatorFreezePosition);
      // unfreeze locator model
      locatorModel->SetAndObserveTransformNodeID(vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->AbdoNavNode->GetTrackerTransformNodeID()))->GetID());
      locatorModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      }
    }
  // cleanup
  collection->Delete();
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkAbdoNavLogic::AddLocatorModel(const char* nodeName, double r, double g, double b)
{
  // TODO: all Update() calls within the scope of this function seem to be unnecessary --> remove?

  vtkSmartPointer<vtkMRMLModelNode> locatorModel = vtkSmartPointer<vtkMRMLModelNode>::New();
  vtkSmartPointer<vtkMRMLModelDisplayNode> locatorDisplay = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
  vtkSmartPointer<vtkAppendPolyData> locatorPolyData = vtkSmartPointer<vtkAppendPolyData>::New();

  // cylinder resolution
  int numberOfSides = 16;
  // measured handle diameter = 6 mm --> radius = 3 mm
  double radiusHandle = 3.0;
  // TODO: vendor claims needle diameter = 17 gauge ~ 1.15 mm --> radius ~ 0.575 mm; however, measured needle diameter = 1.5 mm --> radius = 0.75 mm
  double radiusNeedle = 0.575;

  //----------------------------------------------------------------
  // Create needle tip.
  //----------------------------------------------------------------
  // create needle tip from a cone
  vtkSmartPointer<vtkConeSource> tip = vtkSmartPointer<vtkConeSource>::New();
  tip->SetResolution(numberOfSides);
  tip->SetRadius(radiusNeedle);
  tip->SetHeight(3.0); // 3 mm
  tip->SetCenter(0.0, 0.0, -(3.0/2));
  tip->SetDirection(0.0, 0.0, 1.0);
  tip->Update();
  // add
  locatorPolyData->AddInput(tip->GetOutput());

  //----------------------------------------------------------------
  // Create needle shaft.
  //----------------------------------------------------------------
  // create needle shaft from a cylinder
  vtkSmartPointer<vtkCylinderSource> shaft = vtkSmartPointer<vtkCylinderSource>::New();
  shaft->SetResolution(numberOfSides);
  shaft->SetRadius(radiusNeedle);
  shaft->SetHeight(172.0);  // 172 mm (--> 175 mm in total due to tip)
  shaft->SetCenter(0.0, 0.0, 0.0);
  shaft->Update();
  // rotate and translate needle shaft
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterShaft = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transShaft = vtkSmartPointer<vtkTransform>::New();
  transShaft->RotateX(90.0);
  transShaft->Translate(0.0, -(3.0 + 172.0/2), 0.0);
  transShaft->Update();
  tfilterShaft->SetInput(shaft->GetOutput());
  tfilterShaft->SetTransform(transShaft);
  tfilterShaft->Update();
  // add
  locatorPolyData->AddInput(tfilterShaft->GetOutput());

  //----------------------------------------------------------------
  // Create first part of the needle handle.
  //----------------------------------------------------------------
  // create first part of the needle handle from a cylinder
  vtkSmartPointer<vtkCylinderSource> handle1 = vtkSmartPointer<vtkCylinderSource>::New();
  handle1->SetResolution(numberOfSides);
  handle1->SetRadius(radiusHandle);
  handle1->SetHeight(20.0);
  handle1->SetCenter(0.0, 0.0, 0.0);
  handle1->Update();
  // rotate and translate first part of the needle handle
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterHandle1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transHandle1 = vtkSmartPointer<vtkTransform>::New();
  transHandle1->RotateX(90.0);
  transHandle1->Translate(0.0, -(3.0 + 172.0 + 20.0/2), 0.0);
  transHandle1->Update();
  tfilterHandle1->SetInput(handle1->GetOutput());
  tfilterHandle1->SetTransform(transHandle1);
  tfilterHandle1->Update();
  // add
  locatorPolyData->AddInput(tfilterHandle1->GetOutput());

  //----------------------------------------------------------------
  // Create right angle of the needle handle by extruding a circle.
  //----------------------------------------------------------------
  // create a circle from a regular polygon
  vtkSmartPointer<vtkRegularPolygonSource> angle = vtkSmartPointer<vtkRegularPolygonSource>::New();
  angle->SetNumberOfSides(numberOfSides);
  angle->SetRadius(radiusHandle);
  angle->SetCenter(10.0, 0.0, 0.0);
  angle->Update();
  // rotate circle so that the rotational extrusion filter yields the desired effect
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterAngle1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transAngle1 = vtkSmartPointer<vtkTransform>::New();
  transAngle1->RotateX(-90.0);
  transAngle1->Update();
  tfilterAngle1->SetInput(angle->GetOutput());
  tfilterAngle1->SetTransform(transAngle1);
  tfilterAngle1->Update();
  // extrude the circle: this yields a cylinder bended by 90 degrees
  vtkSmartPointer<vtkRotationalExtrusionFilter> efilterAngle = vtkSmartPointer<vtkRotationalExtrusionFilter>::New();
  efilterAngle->SetInput(tfilterAngle1->GetOutput());
  efilterAngle->SetAngle(90.0);
  efilterAngle->CappingOff();
  efilterAngle->Update();
  // rotate and translate again
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterAngle2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transAngle2 = vtkSmartPointer<vtkTransform>::New();
  transAngle2->RotateX(-90.0);
  transAngle2->Translate(-10.0, +(3.0 + 172.0 + 20.0), 0.0);
  transAngle2->Update();
  tfilterAngle2->SetInput(efilterAngle->GetOutput());
  tfilterAngle2->SetTransform(transAngle2);
  tfilterAngle2->Update();
  // add
  locatorPolyData->AddInput(tfilterAngle2->GetOutput());

  //----------------------------------------------------------------
  // Create second part of the needle handle.
  //----------------------------------------------------------------
  // create second part of the needle handle from a cylinder
  vtkSmartPointer<vtkCylinderSource> handle2 = vtkSmartPointer<vtkCylinderSource>::New();
  handle2->SetResolution(numberOfSides);
  handle2->SetRadius(radiusHandle);
  handle2->SetHeight(50.0);
  handle2->SetCenter(0.0, 0.0, 0.0);
  handle2->Update();
  // rotate and translate second part of the needle handle
  vtkSmartPointer<vtkTransformPolyDataFilter> tfilterHandle2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  vtkSmartPointer<vtkTransform> transHandle2 = vtkSmartPointer<vtkTransform>::New();
  transHandle2->RotateZ(90.0);
  transHandle2->Translate(0.0, +(10.0 + 50.0/2), -(3.0 + 172.0 + 20.0 + 10.0));
  transHandle2->Update();
  tfilterHandle2->SetInput(handle2->GetOutput());
  tfilterHandle2->SetTransform(transHandle2);
  tfilterHandle2->Update();
  // add
  locatorPolyData->AddInput(tfilterHandle2->GetOutput());

  //----------------------------------------------------------------
  // Add cryoprobe model to the scene.
  //----------------------------------------------------------------
  // prepare model node
  locatorPolyData->Update();
  locatorModel->SetAndObservePolyData(locatorPolyData->GetOutput());
  locatorModel->SetName(nodeName);
  locatorModel->SetHideFromEditors(0);
  // prepare model display node
  double color[3] = {r, g, b};
  locatorDisplay->SetPolyData(locatorModel->GetPolyData());
  locatorDisplay->SetColor(color);
  // add both nodes to the scene
  this->GetMRMLScene()->SaveStateForUndo();
  locatorDisplay = vtkMRMLModelDisplayNode::SafeDownCast(this->GetMRMLScene()->AddNode(locatorDisplay));
  locatorModel = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->AddNode(locatorModel));
  if (locatorDisplay && locatorModel)
    {
    locatorModel->SetAndObserveDisplayNodeID(locatorDisplay->GetID());
    }
  else
    {
    vtkErrorMacro("in vtkAbdoNavLogic::AddLocator(...): "
                  "Adding locator model node and/or locator model display node failed!");
    }

  return locatorModel;
}
