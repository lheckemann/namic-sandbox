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

#include "vtkMRMLNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"

#include "vtkBoxWidget2.h"
#include "vtkBoxRepresentation.h"
#include "vtkProperty.h"

#include "vtkPlanes.h"
#include "vtkClipPolyData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkImplicitBoolean.h"
#include "vtkImplicitDataSet.h"
#include "vtkExtractPolyDataGeometry.h"
#include "vtkDataSetMapper.h"
#include "vtkDelaunay3D.h"
#include "vtkElevationFilter.h"
#include "vtkPlaneCollection.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkCleanPolyData.h"

#include "vtkInteractorStyleTrackballActor.h"
#include "vtkAxesActor.h"

#include "vtk3DWidget.h"
#include "vtkAxesActor.h"
#include "vtkAssembly.h"
#include "vtkOsteoPlanGUI.h"

#include "vtkCylinderSource.h"
#include "vtkConeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkProperty.h"

#include "vtkTriangleFilter.h"
#include "vtkButterflySubdivisionFilter.h"

#include "vtkClipClosedSurface.h"

#include "vtkOsteoPlanLogic.h"

vtkCxxRevisionMacro(vtkOsteoPlanLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkOsteoPlanLogic);

//---------------------------------------------------------------------------
vtkOsteoPlanLogic::vtkOsteoPlanLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkOsteoPlanLogic::DataCallback);

  this->part1 = NULL;
  this->part2 = NULL;

  this->XActor = vtkActor::New();
  this->YActor = vtkActor::New();
  this->ZActor = vtkActor::New();

}


//---------------------------------------------------------------------------
vtkOsteoPlanLogic::~vtkOsteoPlanLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  if (this->part1)
    {
      this->part1->Delete();
    }

  if (this->part2)
    {
      this->part2->Delete();
    }

  if (this->XActor)
    {
      this->XActor->Delete();
    }

  if (this->YActor)
    {
      this->YActor->Delete();
    }

  if (this->ZActor)
    {
      this->ZActor->Delete();
    }


}


//---------------------------------------------------------------------------
void vtkOsteoPlanLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkOsteoPlanLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkOsteoPlanLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkOsteoPlanLogic *self = reinterpret_cast<vtkOsteoPlanLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkOsteoPlanLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkOsteoPlanLogic::UpdateAll()
{

}


void vtkOsteoPlanLogic::ClipModelWithBox(vtkMRMLModelNode* model, vtkBoxWidget2* cuttingBox, vtkSlicerApplicationGUI* slicerGUI)
{
//------------------------------------------------------------
// Clipping Operation and Extracting Components
//------------------------------------------------------------ 

  if(this->part1)
    {
      this->part1->Delete();
    }

  if(this->part2)
    {
      this->part2->Delete();
    }

  // Get Planes from vtkBoxWidget  
  vtkPlanes* planes = vtkPlanes::New();
  vtkBoxRepresentation* boxRepresentation = reinterpret_cast<vtkBoxRepresentation*>(cuttingBox->GetRepresentation());
  boxRepresentation->GetPlanes(planes);

  vtkPlaneCollection* planeCollection = vtkPlaneCollection::New();
  
  for(int i = 0; i < planes->GetNumberOfPlanes(); i++)
    {
      planeCollection->AddItem(planes->GetPlane(i));
    }


  /*
  vtkTriangleFilter* triangleFilter = vtkTriangleFilter::New();
  triangleFilter->SetInput(model->GetPolyData());

  vtkButterflySubdivisionFilter* subdivide = vtkButterflySubdivisionFilter::New();
  subdivide->SetInput(triangleFilter->GetOutput());
  subdivide->SetNumberOfSubdivisions(5);
  */

 
  vtkClipClosedSurface* clipper = vtkClipClosedSurface::New();
  clipper->SetInput(model->GetPolyData());
  clipper->SetClippingPlanes(planeCollection);
  clipper->SetBaseColor(1,0,0);
  clipper->SetClipColor(0,0,1);

  planeCollection->Delete();

  /*
  // Set Clipper 1
  vtkClipPolyData* clipper = vtkClipPolyData::New();
  //  clipper->SetInput(subdivide->GetOutput());
  clipper->SetInput(model->GetPolyData()); 
  clipper->SetClipFunction(planes);
  clipper->InsideOutOn();
*/ 
 
  // Set Clipper 2
  vtkClipPolyData* clipper2 = vtkClipPolyData::New();
  clipper2->SetInput(model->GetPolyData());
  clipper2->SetClipFunction(planes);
  clipper2->InsideOutOff();

  vtkPolyData* polyDataModel1 = vtkPolyData::New();
  vtkPolyData* polyDataModel2 = vtkPolyData::New();
  polyDataModel1->CopyStructure(model->GetPolyData());
  polyDataModel2->CopyStructure(model->GetPolyData());

  this->part1 = vtkMRMLModelNode::New();
  this->part2 = vtkMRMLModelNode::New();
  vtkMRMLModelDisplayNode* dnode1 = vtkMRMLModelDisplayNode::New();
  vtkMRMLModelDisplayNode* dnode2 = vtkMRMLModelDisplayNode::New();      
  
  // Model 1
  this->part1->SetScene(this->GetMRMLScene());
  dnode1->SetScene(this->GetMRMLScene());

  this->GetMRMLScene()->AddNode(dnode1);
  this->GetMRMLScene()->AddNode(part1);

  // TODO: Clean polydata ?

  this->part1->SetAndObservePolyData(polyDataModel1);
  this->part1->SetAndObserveDisplayNodeID(dnode1->GetID());

  dnode1->SetPolyData(clipper->GetOutput());
  dnode1->SetVisibility(0);
  dnode1->SetColor(1,0,0);

  polyDataModel1->Delete();
   
  // Model 2 ("Base" Model)
  this->part2->SetScene(this->GetMRMLScene());
  dnode2->SetScene(this->GetMRMLScene());

  this->GetMRMLScene()->AddNode(dnode2);
  this->GetMRMLScene()->AddNode(part2);
 
  
  vtkCleanPolyData* cleanPoly = vtkCleanPolyData::New();
  cleanPoly->SetInput(polyDataModel2);
  cleanPoly->GetOutput()->ComputeBounds();
  

  this->part2->SetAndObservePolyData(cleanPoly->GetOutput());         
  this->part2->SetAndObserveDisplayNodeID(dnode2->GetID());
  //  this->part2->SetName("Base");

  dnode2->SetPolyData(clipper2->GetOutput());
  dnode2->SetVisibility(1);
  dnode2->SetColor(0,1,0);

  // Delete ******************** 
  polyDataModel2->Delete();
  cleanPoly->Delete();
  planes->Delete();
  clipper->Delete();
  clipper2->Delete();
  dnode1->Delete();
  dnode2->Delete();
  //triangleFilter->Delete();
  //subdivide->Delete();  
}
                                  


vtkAssembly* vtkOsteoPlanLogic::CreateMover()
{

  // Create 3 axis and cones
  vtkCylinderSource* AxisCyl = vtkCylinderSource::New();
  vtkConeSource* AxisCone = vtkConeSource::New();
  vtkTransform* CylXform = vtkTransform::New();
  vtkTransform* ConeXform = vtkTransform::New();
  vtkTransformPolyDataFilter* ConeXformFilter = vtkTransformPolyDataFilter::New();
  vtkTransformPolyDataFilter* CylXformFilter = vtkTransformPolyDataFilter::New();

  ConeXformFilter->SetInput(AxisCone->GetOutput());
  ConeXformFilter->SetTransform(ConeXform);
  CylXformFilter->SetInput(AxisCyl->GetOutput());
  CylXformFilter->SetTransform(CylXform);

  vtkAppendPolyData* Axis = vtkAppendPolyData::New();
  Axis->AddInput(CylXformFilter->GetOutput());
  Axis->AddInput(ConeXformFilter->GetOutput());

  vtkPolyDataMapper* AxisMapper = vtkPolyDataMapper::New();
  AxisMapper->SetInput(Axis->GetOutput());

  this->XActor->SetMapper(AxisMapper);
  this->XActor->GetProperty()->SetColor(0,0,1);
  this->XActor->PickableOn();
  this->XActor->DragableOn();
  this->XActor->SetPosition(0,0,0);

  this->YActor->SetMapper(AxisMapper);
  this->YActor->GetProperty()->SetColor(0,1,0);
  this->YActor->PickableOn();
  this->YActor->DragableOn();
  this->YActor->RotateZ(90);
  this->YActor->SetPosition(0,0,0);

  this->ZActor->SetMapper(AxisMapper);
  this->ZActor->GetProperty()->SetColor(1,0,0);
  this->ZActor->PickableOn();
  this->ZActor->DragableOn();
  this->ZActor->RotateY(-90);
  this->ZActor->SetPosition(0,0,0);

  // Set Parameters
  AxisCyl->SetRadius(2);
  AxisCyl->SetHeight(50);
  AxisCyl->SetCenter(0,-25,0);
  AxisCyl->SetResolution(8);
  CylXform->Identity();
  CylXform->RotateZ(90);

  AxisCone->SetRadius(5);
  AxisCone->SetHeight(20);
  AxisCone->SetResolution(8);
  ConeXform->Identity();
  ConeXform->Translate(50,0,0);

  // Assemble all pieces
  vtkAssembly* mover = vtkAssembly::New();
  mover->AddPart(this->XActor);
  mover->AddPart(this->YActor);
  mover->AddPart(this->ZActor);
  mover->PickableOn();

  vtkTransform* rasToWldTransform = vtkTransform::New();
  mover->SetUserMatrix(rasToWldTransform->GetMatrix());
  
  // Delete **********************
  AxisCyl->Delete();
  AxisCone->Delete();
  CylXform->Delete();
  ConeXform->Delete();
  CylXformFilter->Delete();
  ConeXformFilter->Delete();
  Axis->Delete();
  AxisMapper->Delete();
  rasToWldTransform->Delete();

  return mover;
}

