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

#include "vtkBetaProbeNavLogic.h"
#include "vtkMatrix4x4.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkPoints.h"
#include "vtkFloatArray.h"
#include "vtkMRMLUDPServerNode.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkAppendPolyData.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLFiberBundleDisplayNode.h"
#include "vtkSphereSource.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkType.h"
#include "vtkGlyph3D.h"

vtkCxxRevisionMacro(vtkBetaProbeNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkBetaProbeNavLogic);

//---------------------------------------------------------------------------
vtkBetaProbeNavLogic::vtkBetaProbeNavLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBetaProbeNavLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkBetaProbeNavLogic::~vtkBetaProbeNavLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkBetaProbeNavLogic:             " << this->GetClassName() << "\n";

}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkBetaProbeNavLogic *self = reinterpret_cast<vtkBetaProbeNavLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBetaProbeNavLogic DataCallback");
  self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkBetaProbeNavLogic::CollectData(vtkMRMLNode* tn, vtkMRMLNode* cn)
{
  std::cerr << "Collect Data started" << std::endl;
  
  //Pick up position data from TransformNode
  if ((this->Points == NULL) || (this->Scalars == NULL))
    {
    //Create objects
    this->Points = vtkPoints::New();
    this->Scalars = vtkFloatArray::New();
    this->Scalars->SetName("Gamma");
    }

  //Extract position from transform
  float pos[3];
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(tn);
  if (tnode)
    {
    pos[0] = tnode->GetMatrixTransformToParent()->GetElement(0, 3);
    pos[1] = tnode->GetMatrixTransformToParent()->GetElement(1, 3);
    pos[2] = tnode->GetMatrixTransformToParent()->GetElement(2, 3);
    //Add point to Points Array
    this->Points->InsertNextPoint(pos);
    }
  
  //Extract counts from CountNode and append to Float Array
  vtkMRMLUDPServerNode* cnode = vtkMRMLUDPServerNode::SafeDownCast(cn);
  if (cnode)
    {
    float counts[3];
    counts[0] = cnode->GetSmoothedCounts();
    counts[1] = cnode->GetBetaCounts();
    counts[2] = cnode->GetGammaCounts();
    //Add scalar to Scalars array
    this->Scalars->InsertNextTuple1(cnode->GetGammaCounts());
    }

}

//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkBetaProbeNavLogic::RepresentData(vtkMRMLModelNode* mnode)
{
  if ((this->CountMap) == NULL)
    {
    CountMap = vtkPolyData::New();
    }
  
  //Append points to PolyData
  CountMap->SetPoints(Points);
  
  //Calculate the range of the scalars
  double* range = new double [2];
  range = Scalars->GetRange();
  
  //Calculate normalized Scalars
  vtkFloatArray* normScalars = vtkFloatArray::New();
  normScalars->SetName("Gamma");
  for (vtkIdType i = 0; i < (Scalars->GetNumberOfTuples()); i++)
    {
    double ns = 255*((Scalars->GetTuple1(i)) - range[0])/(range[1] - range[0]); 
    normScalars->InsertNextTuple1(ns);
    }
  CountMap->GetPointData()->SetScalars(normScalars);
  CountMap->Update();
  
  //Normalize scalar before adding them to CountMap
  /*double sMin = 0;
  double sMax = 0;]
  double val;
  for (int i = 0; i < (Scalars->GetNumberOfTuples())
    {
    val = Scalars->GetTuple1(i);
    if (i == 0)
      {
      sMin = val;
      sMax = sMin;
      }
    if (val < sMin)
      sMin = val;
    if (val > sMax)
      sMax = val;
    }*/
 
  
  //--------------------------------
  //Represent Polydata using glyphs
  
  //Create display node and add to scene
  std::cerr << "About to create display node display node" << std::endl;
  //vtkMRMLFiberBundleDisplayNode*  dataDisp = vtkMRMLFiberBundleDisplayNode::New();
  vtkMRMLModelDisplayNode* dataDisp = vtkMRMLModelDisplayNode::New();
  std::cerr << "Created the display node" << std::endl;
  GetMRMLScene()->SaveStateForUndo();
  std::cerr << "Created the display node1" << std::endl;
  GetMRMLScene()->AddNode(dataDisp);
    std::cerr << "Created the display node2" << std::endl;
  dataDisp->SetScene(this->GetMRMLScene());
  std::cerr << "Created the display node3" << std::endl;
  
  //Associate display node to model node
  mnode->SetScene(this->GetMRMLScene());
  mnode->SetAndObserveDisplayNodeID(dataDisp->GetID());
  
  // Each glyph is represented by a sphere
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(2.0);
  sphere->SetCenter(0, 0, 0);
  sphere->Update();
  
  //Append geometries together
  //vtkAppendPolyData *apd = vtkAppendPolyData::New();
  //apd->AddInput(sphere->GetOutput());
  //apd->AddInput(CountMap);
  //apd->Update();
  
  /*vtkPoints* pts = vtkPoints::New();
  //vtkCellArray* verts = vtkCellArray::New();
  /*double x[3];
  for (vtkIdType i = 0; i < 5; i++)
    {
    //Create points
    x[1] = i; x[2] = 100*(i+1); x[3] = 500*(i+2);
    pts->InsertNextPoint(x);
    
    //Create scalars
    Scalars->InsertNextTuple1((double)i*10);
    
    //vtkSphereSource *sphere = vtkSphereSource::New();
    //sphere->SetRadius(1.0);
    //sphere->SetCenter(x);
    //sphere->Update();
    
    //Add scalars to the sphere
    //int num = sphere->GetOutput()->GetPolys()->GetNumberOfCells();
    //std::cerr << "Number of polys: " << num << std::endl;

    //for (int j = 0; j < num; j++)
    //  {
    //  scals->InsertNextTuple1(i*1000);
    //  }
    //std::cerr << "Number of cells: " << sphere->GetOutput()->GetNumberOfCells() << std::endl;
    /*sphere->GetOutput()->GetCellData()->SetScalars(scals);
    apd->AddInput(sphere->GetOutput());
    apd->Update(); 
    sphere->Delete();
    scals->Delete();
    //scals->InsertNextTuple1(1000*i);
    //pts->InsertNextPoint(x);
    //verts->InsertNextCell(1, &i);*/
    //}
  
  //Create Glyphs
  vtkGlyph3D* gly = vtkGlyph3D::New();
  gly->SetSourceConnection(sphere->GetOutputPort());
  gly->SetInput(CountMap);
  gly->SetColorModeToColorByScalar();
  //gly->ScalingOff();
  gly->SetScaleModeToDataScalingOff();
  //gly->SetScaleFactor(0.25);
  //gly->ClampingOn();
  //gly->SetRange(0,40);
  //gly->Update();
  
  mnode->SetAndObservePolyData(gly->GetOutput());
  
  //Properties of dataDisp node
  dataDisp->SetActiveScalarName("Gamma");
  dataDisp->SetScalarVisibility(1);
  //dataDisp->SetColorNode
  dataDisp->SetPolyData(mnode->GetPolyData());
  dataDisp->SetVisibility(1);
  //dataDisp->SetColorModeToScalar();
  //Assign color to geometry
  /*double color[3];
  color[0] = 0.5;
  color[1] = 0.5;
  color[2] = 0.3;*/

  return mnode;
}
