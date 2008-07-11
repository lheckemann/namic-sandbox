/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: $
Date:      $Date: $
Version:   $Revision: $

==========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkNeuroNavLogic.h"

#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerColorLogic.h"

#include "vtkCylinderSource.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"


#include "vtkMultiThreader.h"


vtkCxxRevisionMacro(vtkNeuroNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkNeuroNavLogic);

//---------------------------------------------------------------------------
vtkNeuroNavLogic::vtkNeuroNavLogic()
{

  this->SliceDriver[0] = vtkNeuroNavLogic::SLICE_DRIVER_USER;
  this->SliceDriver[1] = vtkNeuroNavLogic::SLICE_DRIVER_USER;
  this->SliceDriver[2] = vtkNeuroNavLogic::SLICE_DRIVER_USER;

  // If the following code doesn't work, slice nodes should be obtained from application GUI
  this->SliceNode[0] = NULL;
  this->SliceNode[1] = NULL;
  this->SliceNode[2] = NULL;

  this->NeedRealtimeImageUpdate0 = 0;
  this->NeedRealtimeImageUpdate1 = 0;
  this->NeedRealtimeImageUpdate2 = 0;
  this->ImagingControl = 0;

  this->EnableOblique = true;
  this->TransformNodeName = NULL;

}


//---------------------------------------------------------------------------
vtkNeuroNavLogic::~vtkNeuroNavLogic()
{


}


//---------------------------------------------------------------------------
void vtkNeuroNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkNeuroNavLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkNeuroNavLogic::SetVisibilityOfLocatorModel(const char* nodeName, int v)
{
  vtkMRMLModelNode*   locatorModel;
  vtkMRMLDisplayNode* locatorDisp;

  // Check if any node with the specified name exists
  vtkMRMLScene*  scene = this->GetApplicationLogic()->GetMRMLScene();
  vtkCollection* collection = scene->GetNodesByName(nodeName);

  if (collection != NULL && collection->GetNumberOfItems() == 0)
    {
    // if a node doesn't exist
    locatorModel = AddLocatorModel(nodeName, 0.0, 1.0, 1.0);
    }
  else
    {
    locatorModel = vtkMRMLModelNode::SafeDownCast(collection->GetItemAsObject(0));
    }

  if (locatorModel)
    {
    locatorDisp = locatorModel->GetDisplayNode();
    locatorDisp->SetVisibility(v);
    locatorModel->Modified();
    this->GetApplicationLogic()->GetMRMLScene()->Modified();
    }

  return locatorModel;
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkNeuroNavLogic::AddLocatorModel(const char* nodeName, double r, double g, double b)
{

  vtkMRMLModelNode           *locatorModel;
  vtkMRMLModelDisplayNode    *locatorDisp;

  locatorModel = vtkMRMLModelNode::New();
  locatorDisp = vtkMRMLModelDisplayNode::New();

  GetMRMLScene()->SaveStateForUndo();
  GetMRMLScene()->AddNode(locatorDisp);
  GetMRMLScene()->AddNode(locatorModel);  

  locatorDisp->SetScene(this->GetMRMLScene());

  locatorModel->SetName(nodeName);
  locatorModel->SetScene(this->GetMRMLScene());
  locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
  locatorModel->SetHideFromEditors(0);

  // Cylinder represents the locator stick
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetRadius(1.5);
  cylinder->SetHeight(100);
  cylinder->SetCenter(0, 50, 0);
  cylinder->Update();

  // Sphere represents the locator tip 
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();

  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  apd->AddInput(cylinder->GetOutput());
  apd->Update();

  locatorModel->SetAndObservePolyData(apd->GetOutput());

  double color[3];
  color[0] = r;
  color[1] = g;
  color[2] = b;
  locatorDisp->SetPolyData(locatorModel->GetPolyData());
  locatorDisp->SetColor(color);

  cylinder->Delete();
  sphere->Delete();
  apd->Delete();

  //locatorModel->Delete();
  locatorDisp->Delete();

  return locatorModel;
}

void vtkNeuroNavLogic::UpdateDisplay(int sliceNo1, int sliceNo2, int sliceNo3)
{
  // if all user driven; does nothing
  if (sliceNo1 == 1 &&
      sliceNo2 == 1 &&
      sliceNo3 == 1)
    {
    return;
    }

  vtkMRMLLinearTransformNode* transformNode;
  vtkMRMLScene* scene = this->GetApplicationLogic()->GetMRMLScene();
  vtkCollection* collection = scene->GetNodesByName(this->TransformNodeName);

  if (collection != NULL && collection->GetNumberOfItems() == 0)
    {
    // the node name does not exist in the MRML tree
    return;
    }

  transformNode = vtkMRMLLinearTransformNode::SafeDownCast(collection->GetItemAsObject(0));

  vtkMatrix4x4* transform;
  //transform = transformNode->GetMatrixTransformToParent();
  transform = transformNode->GetMatrixTransformToParent();

  if (transform)
    {
    // set volume orientation
    float tx = transform->GetElement(0, 0);
    float ty = transform->GetElement(1, 0);
    float tz = transform->GetElement(2, 0);
    float sx = transform->GetElement(0, 1);
    float sy = transform->GetElement(1, 1);
    float sz = transform->GetElement(2, 1);
    float nx = transform->GetElement(0, 2);
    float ny = transform->GetElement(1, 2);
    float nz = transform->GetElement(2, 2);
    float px = transform->GetElement(0, 3);
    float py = transform->GetElement(1, 3);
    float pz = transform->GetElement(2, 3);

    if (!sliceNo1) // axial driven by Locator
      {
      UpdateSliceNode(0, nx, ny, nz, tx, ty, tz, px, py, pz);
      }
    if (!sliceNo2) // sagittal driven by Locator
      {
      UpdateSliceNode(1, nx, ny, nz, tx, ty, tz, px, py, pz);
      }
    if (!sliceNo3) // coronal driven by Locator
      {
      UpdateSliceNode(2, nx, ny, nz, tx, ty, tz, px, py, pz);
      }
    }
}



void vtkNeuroNavLogic::UpdateSliceNode(int sliceNodeNumber,
                                       float nx, float ny, float nz,
                                       float tx, float ty, float tz,
                                       float px, float py, float pz)
{
  CheckSliceNodes();

  if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Axial") == 0)
    {
    if (this->EnableOblique) // perpendicular
      {
      this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 2);
      this->SliceNode[sliceNodeNumber]->UpdateMatrices();
      }
    else
      {
      this->SliceNode[sliceNodeNumber]->SetOrientationToAxial();
      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      this->SliceNode[sliceNodeNumber]->UpdateMatrices();
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Sagittal") == 0)
    {
    if (this->EnableOblique) // In-Plane
      {
      this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 0);
      this->SliceNode[sliceNodeNumber]->UpdateMatrices();
      }
    else
      {
      this->SliceNode[sliceNodeNumber]->SetOrientationToSagittal();
      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      this->SliceNode[sliceNodeNumber]->UpdateMatrices();
      }
    }
  else if (strcmp(this->SliceNode[sliceNodeNumber]->GetOrientationString(), "Coronal") == 0)
    {
    if (this->EnableOblique)  // In-Plane 90
      {
      this->SliceNode[sliceNodeNumber]->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 1);
      this->SliceNode[sliceNodeNumber]->UpdateMatrices();
      }
    else
      {
      this->SliceNode[sliceNodeNumber]->SetOrientationToCoronal();
      this->SliceNode[sliceNodeNumber]->JumpSlice(px, py, pz);
      this->SliceNode[sliceNodeNumber]->UpdateMatrices();
      }
    }
}


void vtkNeuroNavLogic::CheckSliceNodes()
{
  for (int i = 0; i < 3; i ++)
    {
    if (this->SliceNode[i] == NULL)
      {
      char nodename[36];
      sprintf(nodename, "vtkMRMLSliceNode%d", i+1);
      this->SliceNode[i] = vtkMRMLSliceNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodename));
      }
    }
}
