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
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkHybridNavLogic.h"
#include "vtkMRMLHybridNavToolNode.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkCylinderSource.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

vtkCxxRevisionMacro(vtkHybridNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkHybridNavLogic);

//---------------------------------------------------------------------------
vtkHybridNavLogic::vtkHybridNavLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkHybridNavLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkHybridNavLogic::~vtkHybridNavLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkHybridNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkHybridNavLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkHybridNavLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkHybridNavLogic *self = reinterpret_cast<vtkHybridNavLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkHybridNavLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkHybridNavLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
vtkMRMLHybridNavToolNode* vtkHybridNavLogic::CreateToolModel(vtkMRMLHybridNavToolNode* tnode)
{
  //Create display node and add to scene
  vtkMRMLModelDisplayNode*  toolDisp = vtkMRMLModelDisplayNode::New();
  
  GetMRMLScene()->SaveStateForUndo();
  GetMRMLScene()->AddNode(toolDisp);
  toolDisp->SetScene(this->GetMRMLScene());
  
  //Associate display node to tool node
  tnode->SetScene(this->GetMRMLScene());
  tnode->SetAndObserveDisplayNodeID(toolDisp->GetID());
  
  // Graphical representation of the tool node
  // Cylinder represents the locator stick
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetRadius(1.5);
  cylinder->SetHeight(100);
  cylinder->SetCenter(0, 0, 0);
  cylinder->Update();

  // Rotate cylinder
  vtkTransformPolyDataFilter *tfilter = vtkTransformPolyDataFilter::New();
  vtkTransform* trans =   vtkTransform::New();
  trans->RotateX(90.0);
  trans->Translate(0.0, -50.0, 0.0);
  trans->Update();
  tfilter->SetInput(cylinder->GetOutput());
  tfilter->SetTransform(trans);
  tfilter->Update();
  
  // Sphere represents the tool tracking sensor
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  //Append geometries together
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  apd->AddInput(tfilter->GetOutput());
  apd->Update();
  
  tnode->SetAndObservePolyData(apd->GetOutput());
  
  double color[3];
  color[0] = 0.5;
  color[1] = 0.5;
  color[2] = 0.3;
  toolDisp->SetPolyData(tnode->GetPolyData());
  toolDisp->SetColor(color);
  toolDisp->SetVisibility(1);
  
  trans->Delete();
  tfilter->Delete();
  cylinder->Delete();
  sphere->Delete();
  apd->Delete();

  //locatorModel->Delete();
  toolDisp->Delete();

  return tnode;
}

//---------------------------------------------------------------------------
void vtkHybridNavLogic::SetVisibilityOfToolModel(vtkMRMLHybridNavToolNode* tnode, int v)
{
  vtkMRMLDisplayNode* toolDisp;

  if (tnode)
    {
    toolDisp = tnode->GetDisplayNode();
    toolDisp->SetVisibility(v);
    tnode->Modified();
    this->GetApplicationLogic()->GetMRMLScene()->Modified();
    }
    
  std::cerr << "Set Visibility turned On" << std::endl;
}

//-------------------------------------------------------------------------------------------------------------
/*vtkMRMLModelNode* vtkHybridNavLogic::AddLocatorModel(const char* nodeName, double r, double g, double b)
{

  vtkMRMLModelNode           *locatorModel;
  vtkMRMLModelDisplayNode    *locatorDisp;

  locatorModel = vtkMRMLModelNode::New();
  locatorDisp = vtkMRMLModelDisplayNode::New();
  
  GetMRMLScene()->SaveStateForUndo();
  GetMRMLScene()->AddNode(locatorDisp);
  GetMRMLScene()->AddNode(locatorModel);  
  
  locatorDisp->SetScene(this->GetMRMLScene());
  
  //locatorModel->SetName(nodeName);
  locatorModel->SetScene(this->GetMRMLScene());
  locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
  locatorModel->SetHideFromEditors(0);
  
  // Cylinder represents the locator stick
  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetRadius(1.5);
  cylinder->SetHeight(100);
  cylinder->SetCenter(0, 0, 0);
  cylinder->Update();

  // Rotate cylinder
  vtkTransformPolyDataFilter *tfilter = vtkTransformPolyDataFilter::New();
  vtkTransform* trans =   vtkTransform::New();
  trans->RotateX(90.0);
  trans->Translate(0.0, -50.0, 0.0);
  trans->Update();
  tfilter->SetInput(cylinder->GetOutput());
  tfilter->SetTransform(trans);
  tfilter->Update();
  
  // Sphere represents the locator tip 
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(sphere->GetOutput());
  apd->AddInput(tfilter->GetOutput());
  apd->Update();
  
  locatorModel->SetAndObservePolyData(apd->GetOutput());
  
  double color[3];
  color[0] = r;
  color[1] = g;
  color[2] = b;
  locatorDisp->SetPolyData(locatorModel->GetPolyData());
  locatorDisp->SetColor(color);
  
  trans->Delete();
  tfilter->Delete();
  cylinder->Delete();
  sphere->Delete();
  apd->Delete();

  //locatorModel->Delete();
  locatorDisp->Delete();

  return locatorModel;
}*/
