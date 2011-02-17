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

#include "vtkLineSource.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkActorCollection.h"

#include "vtkLineMotionLogic.h"

vtkCxxRevisionMacro(vtkLineMotionLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkLineMotionLogic);

//---------------------------------------------------------------------------
vtkLineMotionLogic::vtkLineMotionLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkLineMotionLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkLineMotionLogic::~vtkLineMotionLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkLineMotionLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkLineMotionLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkLineMotionLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkLineMotionLogic *self = reinterpret_cast<vtkLineMotionLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkLineMotionLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkLineMotionLogic::UpdateAll()
{

}

void vtkLineMotionLogic::RefreshLines(vtkMRMLFiducialListNode* fiducialListNode, vtkActorCollection* actorCollection, vtkSlicerApplicationGUI* slicerGUI)
{
  if(fiducialListNode && slicerGUI && actorCollection)
    {
      if(fiducialListNode->GetNumberOfFiducials() == 2)
        {
          vtkLineSource* lineFiducials = vtkLineSource::New();
          float* p1 = fiducialListNode->GetNthFiducialXYZ(0);
          float* p2 = fiducialListNode->GetNthFiducialXYZ(1);
      
          lineFiducials->SetPoint1(p1[0],p1[1],p1[2]);
          lineFiducials->SetPoint2(p2[0],p2[1],p2[2]);
          lineFiducials->Update();
      
          vtkPolyDataMapper* lineMapper = vtkPolyDataMapper::New();
          lineMapper->SetInputConnection(lineFiducials->GetOutputPort());
      
          vtkActor* lineActor = vtkActor::New();
          lineActor->SetMapper(lineMapper);
      
          actorCollection->AddItem(lineActor);
      
          slicerGUI->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(lineActor);
      
          lineActor->Delete();
          lineMapper->Delete();
          lineFiducials->Delete();
        }
    }
}





