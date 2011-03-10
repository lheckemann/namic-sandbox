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
#include "vtkClipClosedSurface.h"
#include "vtkPlaneCollection.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkCleanPolyData.h"

#include "vtkInteractorStyleTrackballActor.h"
#include "vtkAxesActor.h"

#include "vtk3DWidget.h"
#include "vtkAxesActor.h"
#include "vtkAssembly.h"
#include "vtkOsteoPlanGUI.h"

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
// Extract Geometry Operation
//------------------------------------------------------------ 
#if 0
  // Get Planes from vtkBoxWidget  
  vtkPlanes* planes = vtkPlanes::New();
  vtkBoxRepresentation* boxRepresentation = reinterpret_cast<vtkBoxRepresentation*>(cuttingBox->GetRepresentation());
  boxRepresentation->GetPlanes(planes);
  
  vtkDelaunay3D* delaunay = vtkDelaunay3D::New();
  delaunay->SetInput(model->GetPolyData());
  delaunay->BoundingTriangulationOff();

  vtkElevationFilter* elev = vtkElevationFilter::New();
  elev->SetInputConnection(delaunay->GetOutputPort());

  vtkImplicitDataSet* coneData = vtkImplicitDataSet::New();
  coneData->SetDataSet(elev->GetOutput());

  vtkImplicitBoolean* dataUnion = vtkImplicitBoolean::New();
  dataUnion->AddFunction(coneData);
  dataUnion->AddFunction(planes);
  dataUnion->SetOperationTypeToDifference();

  vtkExtractPolyDataGeometry* extract = vtkExtractPolyDataGeometry::New();
  extract->SetImplicitFunction(dataUnion);
  extract->SetInput(model->GetPolyData());

  vtkDataSetMapper* dataMapper = vtkDataSetMapper::New();
  dataMapper->SetInputConnection(extract->GetOutputPort());

  vtkActor* dataActor = vtkActor::New();
  dataActor->SetMapper(dataMapper);

  slicerGUI->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(dataActor);


  // Add to MRML Scene
  //  vtkMRMLModelNode* part1 = vtkMRMLModelNode::New();
  //  vtkMRMLModelNode* part2 = vtkMRMLModelNode::New();

  this->part1 = vtkMRMLModelNode::New();
  vtkMRMLModelDisplayNode* dnode1 = vtkMRMLModelDisplayNode::New();
  
  // - Model 1
  dnode1->SetColor(0,0,1);
  dnode1->SetBackfaceCulling(1);
  this->GetMRMLScene()->AddNode(dnode1);
  part1->SetAndObservePolyData(extract->GetOutput());
  part1->SetAndObserveDisplayNodeID(dnode1->GetID());
  this->GetMRMLScene()->AddNode(part1);

  slicerGUI->GetActiveViewerWidget()->Render();

#endif

//------------------------------------------------------------
// Clipping Operation
//------------------------------------------------------------ 
#if 1
  // Get Planes from vtkBoxWidget  
  vtkPlanes* planes = vtkPlanes::New();
  vtkBoxRepresentation* boxRepresentation = reinterpret_cast<vtkBoxRepresentation*>(cuttingBox->GetRepresentation());
  boxRepresentation->GetPlanes(planes);


  // Set Clipper 1
  vtkClipPolyData* clipper = vtkClipPolyData::New();
  clipper->SetInput(model->GetPolyData());
  clipper->SetClipFunction(planes);
  clipper->InsideOutOn();
  
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

  this->part1->SetAndObservePolyData(polyDataModel1);
  this->part1->SetAndObserveDisplayNodeID(dnode1->GetID());

  dnode1->SetPolyData(clipper->GetOutput());
  dnode1->SetVisibility(0);
  dnode1->SetColor(1,0,0);

  polyDataModel1->Delete();

  // Base
  vtkCollection* listmodel2 = this->GetMRMLScene()->GetNodesByName("Base");

  // Check Base does not exist
  if(listmodel2->GetNumberOfItems() > 0)
    {
 
    // Delete All "Base" Model already existing
    for(int i=0; i < listmodel2->GetNumberOfItems(); i++)
      {
      vtkMRMLNode* basemodel = reinterpret_cast<vtkMRMLNode*>(listmodel2->GetItemAsObject(i));
      this->GetMRMLScene()->RemoveNode(basemodel);
      }
    }
  listmodel2->Delete();
   
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
  this->part2->SetName("Base");

  dnode2->SetPolyData(clipper2->GetOutput());
  dnode2->SetVisibility(1);
  dnode2->SetColor(0,1,0);

  polyDataModel2->Delete();


  // Delete ******************** 
  cleanPoly->Delete();
  planes->Delete();
  clipper->Delete();
  clipper2->Delete();
  dnode1->Delete();
  dnode2->Delete();
  
#endif  

//------------------------------------------------------------
// ClipClosedSurface Operation
//------------------------------------------------------------ 
#if 0
  // Get Planes from vtkBoxWidget  
  vtkPlanes* planes = vtkPlanes::New();
  vtkBoxRepresentation* boxRepresentation = reinterpret_cast<vtkBoxRepresentation*>(cuttingBox->GetRepresentation());
  boxRepresentation->GetPlanes(planes);

  // Create vtkPlaneCollection
  vtkPlaneCollection* collectionPlane = vtkPlaneCollection::New();
  for(int i=0;i < planes->GetNumberOfPlanes(); i++)
    {
      collectionPlane->AddItem(planes->GetPlane(i));
    } 

  // Set Clipper 1
  vtkClipClosedSurface* clipper = vtkClipClosedSurface::New();
  clipper->SetInput(model->GetPolyData());
  clipper->SetClippingPlanes(collectionPlane);
  clipper->GenerateFacesOn();
  clipper->SetBaseColor(1,0,0);
  clipper->SetClipColor(0,0,1);  

  // Set Mapper 1     
  vtkPolyDataMapper* clipMapper = vtkPolyDataMapper::New();
  clipMapper->SetInputConnection(clipper->GetOutputPort());
  
  /*
  // Set Mapper 2
  vtkPolyDataMapper* clipMapper2 = vtkPolyDataMapper::New();
  clipMapper2->SetInputConnection(clipper->GetClippedOutputPort());
  // clipMapper2->ScalarVisibilityOff();
  */
  
  // Set Actor 1
  vtkActor* clipActor = vtkActor::New();
  clipActor->SetMapper(clipMapper);
  clipActor->GetProperty()->SetColor(1,0,0);
  
  /*
  // Set Actor 2
  vtkActor* clipActor2 = vtkActor::New();
  clipActor2->SetMapper(clipMapper2);
  clipActor2->GetProperty()->SetColor(0,0,1);
  */

  slicerGUI->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(clipActor);
  // slicerGUI->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(clipActor2);

  slicerGUI->GetActiveViewerWidget()->Render();

  planes->Delete();
  collectionPlane->Delete();  
  clipper->Delete();
  clipMapper->Delete();
  clipActor->Delete();

#endif

//------------------------------------------------------------
// Clipping and Extracting Operations
//------------------------------------------------------------ 
#if 0
  // Get Planes from vtkBoxWidget  
  vtkPlanes* planes = vtkPlanes::New();
  vtkBoxRepresentation* boxRepresentation = reinterpret_cast<vtkBoxRepresentation*>(cuttingBox->GetRepresentation());
  boxRepresentation->GetPlanes(planes);

  // Set Clipper 1
  vtkClipPolyData* clipper = vtkClipPolyData::New();
  clipper->SetInput(model->GetPolyData());
  clipper->SetClipFunction(planes);
  clipper->InsideOutOn();
  
  // Set Clipper 2
  vtkClipPolyData* clipper2 = vtkClipPolyData::New();
  clipper2->SetInput(model->GetPolyData());
  clipper2->SetClipFunction(planes);
  clipper2->InsideOutOff();

  vtkPolyData* polyDataModel1 = vtkPolyData::New();
  vtkPolyData* polyDataModel2 = vtkPolyData::New();

  this->part1 = vtkMRMLModelNode::New();
  this->part2 = vtkMRMLModelNode::New();
  vtkMRMLModelDisplayNode* dnode1 = vtkMRMLModelDisplayNode::New();
  vtkMRMLModelDisplayNode* dnode2 = vtkMRMLModelDisplayNode::New();      
  
  // Model 1
  this->part1->SetScene(this->GetMRMLScene());
  dnode1->SetScene(this->GetMRMLScene());

  this->GetMRMLScene()->AddNode(dnode1);
  this->GetMRMLScene()->AddNode(part1);

  part1->SetAndObservePolyData(polyDataModel1);
  part1->SetAndObserveDisplayNodeID(dnode1->GetID());

  dnode1->SetPolyData(clipper->GetOutput());
  dnode1->SetVisibility(0);
  dnode1->SetColor(1,0,0);

  polyDataModel1->Delete();

  // Base
  vtkCollection* listmodel2 = this->GetMRMLScene()->GetNodesByName("Base");

  // Check Base does not exist
  if(listmodel2->GetNumberOfItems() > 0)
    {
 
    // Delete All "Base" Model already existing
    for(int i=0; i < listmodel2->GetNumberOfItems(); i++)
      {
      vtkMRMLNode* basemodel = reinterpret_cast<vtkMRMLNode*>(listmodel2->GetItemAsObject(i));
      this->GetMRMLScene()->RemoveNode(basemodel);
      }
    }
  listmodel2->Delete();
  
  // Model 2 ("Base" Model)
  this->part2->SetScene(this->GetMRMLScene());
  dnode2->SetScene(this->GetMRMLScene());

  this->GetMRMLScene()->AddNode(dnode2);
  this->GetMRMLScene()->AddNode(part2);
 
  part2->SetAndObservePolyData(polyDataModel2);         
  part2->SetAndObserveDisplayNodeID(dnode2->GetID());
  part2->SetName("Base");

  dnode2->SetPolyData(clipper2->GetOutput());
  dnode2->SetVisibility(1);
  dnode2->SetColor(0,1,0);

  polyDataModel2->Delete();

  
  // Delete ******************** 
  
  planes->Delete();
  clipper->Delete();
  clipper2->Delete();
  dnode1->Delete();
  dnode2->Delete();


  // Create Picker to pick cells and apply a connectivity filter on it to extract regions
  // Step 1: Create a picker
  vtkCellPicker* picker = vtkCellPicker::New();
  picker->SetTolerance(0.001);

  
  
  
#endif  




 
}
                                  




