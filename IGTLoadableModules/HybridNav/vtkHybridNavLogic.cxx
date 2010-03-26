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
#include "vtkConeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"

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
  trans->Translate(0.0, -50.0 - 5, 0.0);
  trans->Update();
  tfilter->SetInput(cylinder->GetOutput());
  tfilter->SetTransform(trans);
  tfilter->Update();
  
  // Cone represents the tool tracking sensor
  vtkConeSource *cone = vtkConeSource::New();
  cone->SetRadius(1.5);
  cone->SetCenter(0, 0, 0);
  cone->SetHeight(5);
  cone->SetDirection(0,0,1);
  cone->Update();
  
  // Translate cone
  vtkTransformPolyDataFilter *tfilter1 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans1 =   vtkTransform::New();
  trans1->RotateZ(30.0);
  trans1->Translate(0.0, 0.0, -2.5);
  trans1->Update();
  tfilter1->SetInput(cone->GetOutput());
  tfilter1->SetTransform(trans1);
  tfilter1->Update();
  
  //Append geometries together
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(tfilter1->GetOutput());
  apd->AddInput(tfilter->GetOutput());
  apd->Update();
  
  tnode->SetAndObservePolyData(apd->GetOutput());
  
  //Assign color to geometry
  double color[3];
  color[0] = 0.5;
  color[1] = 0.5;
  color[2] = 0.3;
  toolDisp->SetPolyData(tnode->GetPolyData());
  toolDisp->SetColor(color);
  toolDisp->SetVisibility(1);
  
  /*trans->Delete();
  tfilter->Delete();
  trans1->Delete();
  tfilter1->Delete();
  cylinder->Delete();
  cone->Delete();
  apd->Delete();

  //locatorModel->Delete();
  toolDisp->Delete();*/

  return tnode;
}

//---------------------------------------------------------------------------
void vtkHybridNavLogic::AppendToolTipModel(vtkMRMLHybridNavToolNode* mnode)
{
  //Extract display node from tool node
  vtkMRMLModelDisplayNode*  tipDisp = mnode->GetModelDisplayNode();
  
  //Get Polydata from Display Node
  vtkPolyData* pd = tipDisp->GetPolyData();
  
  //Create additional sphere to represent sensor
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(3.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  // Create filter
  vtkTransformPolyDataFilter* tfilter = vtkTransformPolyDataFilter::New();
  vtkTransform* trans = vtkTransform::New();
  trans->Identity();
  trans->Translate(0.0, 0.0, -1*(mnode->GetCalibrationMatrix()->GetElement(2,3)));
  //trans->Translate(0.0, 0.0, -20);
  trans->Update();
  tfilter->SetInput(sphere->GetOutput());
  tfilter->SetTransform(trans);
  tfilter->Update();
  
  //Append geometries together
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(tfilter->GetOutput());
  //apd->AddInput(sphere->GetOutput());
  apd->AddInput(pd);
  apd->Update();

  mnode->SetAndObservePolyData(apd->GetOutput());
  
  //Give color to the geometries
  double color[3];
  color[0] = 1;
  color[1] = 0.1;
  color[2] = 0.1;
  tipDisp->SetPolyData(mnode->GetPolyData());
  tipDisp->SetColor(color);
  tipDisp->SetVisibility(1);
  mnode->Modified();
  this->GetApplicationLogic()->GetMRMLScene()->Modified();
  
  //Print onto screen the transform
  vtkMatrix4x4* m = vtkMatrix4x4::New();
  mnode->GetParentTransformNode()->GetMatrixTransformToWorld(m);
  m->Print(std::cerr);
  mnode->GetCalibrationMatrix()->Print(std::cerr);
  m->Multiply4x4(m, mnode->GetCalibrationMatrix(), m);
  m->Print(std::cerr);
  
  /*//Clean up
  tfilter->Delete();
  trans->Delete();
  sphere->Delete();
  apd->Delete();
  pd->Delete();
  tipDisp->Delete();*/
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