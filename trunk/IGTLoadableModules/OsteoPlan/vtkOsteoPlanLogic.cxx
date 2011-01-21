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

#include "vtkBoxWidget.h"
#include "vtkBoxRepresentation.h"
#include "vtkProperty.h"

#include "vtkPlanes.h"
#include "vtkClipPolyData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

#include "vtkInteractorStyleTrackballActor.h"
#include "vtkAxesActor.h"

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

}


//---------------------------------------------------------------------------
vtkOsteoPlanLogic::~vtkOsteoPlanLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
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


void vtkOsteoPlanLogic::ClipModelWithBox(vtkMRMLModelNode* model, vtkBoxWidget* cuttingBox)
{
          // Get Planes from vtkBoxWidget  
          vtkPlanes* planes = vtkPlanes::New();
          cuttingBox->GetPlanes(planes);
       
          // Set Clipper 1
          vtkClipPolyData* clipper = vtkClipPolyData::New();
          clipper->SetInput(model->GetPolyData());
          clipper->SetClipFunction(planes);
          clipper->GenerateClipScalarsOn();
          clipper->GenerateClippedOutputOn();
          clipper->InsideOutOn();
      
          // Set Clipper 2
          vtkClipPolyData* clipper2 = vtkClipPolyData::New();
          clipper2->SetInput(model->GetPolyData());
          clipper2->SetClipFunction(planes);
          clipper2->GenerateClipScalarsOn();
          clipper2->GenerateClippedOutputOn();
          clipper2->InsideOutOff();
          
          // Set Mapper 1     
          vtkPolyDataMapper* clipMapper = vtkPolyDataMapper::New();
          clipMapper->SetInputConnection(clipper->GetOutputPort());
          clipMapper->ScalarVisibilityOff();
          
          // Set Mapper 2
          vtkPolyDataMapper* clipMapper2 = vtkPolyDataMapper::New();
          clipMapper2->SetInputConnection(clipper2->GetOutputPort());
          clipMapper2->ScalarVisibilityOff();
          
          // Set Actor 1
          vtkActor* clipActor = vtkActor::New();
          clipActor->SetMapper(clipMapper);
          clipActor->GetProperty()->SetColor(1,0,0);

       // Set Actor 2
          vtkActor* clipActor2 = vtkActor::New();
          clipActor2->SetMapper(clipMapper2);
          clipActor2->GetProperty()->SetColor(0,0,1);

          // Add to MRML Scene
          vtkMRMLModelNode* part1 = vtkMRMLModelNode::New();
          vtkMRMLModelNode* part2 = vtkMRMLModelNode::New();
          vtkMRMLModelDisplayNode* dnode1 = vtkMRMLModelDisplayNode::New();
          vtkMRMLModelDisplayNode* dnode2 = vtkMRMLModelDisplayNode::New();      

          // - Model 1
          dnode1->SetColor(clipActor->GetProperty()->GetColor());
          this->GetMRMLScene()->AddNode(dnode1);
          part1->SetAndObservePolyData(clipper->GetOutput());
          part1->SetAndObserveDisplayNodeID(dnode1->GetID());
          this->GetMRMLScene()->AddNode(part1);

          // - Base
          // -- Check Base does not exist
          vtkCollection* listmodel2 = this->GetMRMLScene()->GetNodesByName("Base");
          if(listmodel2->GetNumberOfItems() > 0)
         {
           // --- Delete All "Base" Model
           for(int i=0; i < listmodel2->GetNumberOfItems(); i++)
          {
            vtkMRMLNode* basemodel = reinterpret_cast<vtkMRMLNode*>(listmodel2->GetItemAsObject(i));
                  this->GetMRMLScene()->RemoveNode(basemodel);
          }
         }
          listmodel2->Delete();

       // -- Draw (or redraw if deleted) "Base" Model
          dnode2->SetColor(clipActor2->GetProperty()->GetColor());
          this->GetMRMLScene()->AddNode(dnode2);
          part2->SetAndObservePolyData(clipper2->GetOutput());         
          part2->SetAndObserveDisplayNodeID(dnode2->GetID());
          part2->SetName("Base");
          this->GetMRMLScene()->AddNode(part2);

       /*
          // [] FIXME: Use to move model, but without without axis + Segfault   
        vtkInteractorStyleTrackballActor* movingaxes = vtkInteractorStyleTrackballActor::New();
          movingaxes->SetDefaultRenderer(this->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
          movingaxes->SetEnabled(1); 
        
          this->GetApplicationGUI()->GetActiveRenderWindowInteractor()->SetInteractorStyle(movingaxes);
       */

          // Delete ******************** 
          planes->Delete();
          clipper->Delete();
          clipper2->Delete();
          clipMapper->Delete();
          clipMapper2->Delete();
          clipActor->Delete();
          clipActor2->Delete();
          dnode1->Delete();
          dnode2->Delete();
          part1->Delete();
          part2->Delete();

}
                                  




