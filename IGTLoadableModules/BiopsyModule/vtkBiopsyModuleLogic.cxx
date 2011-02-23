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
#include "vtkProperty.h"
#include "vtkColor.h"

#include "vtkBiopsyModuleLogic.h"

vtkCxxRevisionMacro(vtkBiopsyModuleLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkBiopsyModuleLogic);

//---------------------------------------------------------------------------
vtkBiopsyModuleLogic::vtkBiopsyModuleLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBiopsyModuleLogic::DataCallback);

  this->SliceNode[0] = NULL;
  this->SliceNode[1] = NULL;
  this->SliceNode[2] = NULL;


}


//---------------------------------------------------------------------------
vtkBiopsyModuleLogic::~vtkBiopsyModuleLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkBiopsyModuleLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkBiopsyModuleLogic *self = reinterpret_cast<vtkBiopsyModuleLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBiopsyModuleLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::CheckSliceNode()
{
  
  if (this->SliceNode[0] == NULL)
    {
    this->SliceNode[0] = this->GetApplicationLogic()
      ->GetSliceLogic("Red")->GetSliceNode();
    }
  if (this->SliceNode[1] == NULL)
    {
    this->SliceNode[1] = this->GetApplicationLogic()
      ->GetSliceLogic("Yellow")->GetSliceNode();
    }
  if (this->SliceNode[2] == NULL)
    {
    this->SliceNode[2] = this->GetApplicationLogic()
      ->GetSliceLogic("Green")->GetSliceNode();
    }
}

//---------------------------------------------------------------------------
void vtkBiopsyModuleLogic::UpdateSliceNode(double* direction, double* transverse, double* position, int SliceNodeNumber)
{
  CheckSliceNode();
  
  float tx = direction[0];
  float ty = direction[1];
  float tz = direction[2];
  
  float nx = transverse[0];
  float ny = transverse[1];
  float nz = transverse[2];

  float px = position[0];
  float py = position[1];
  float pz = position[2];

  this->SliceNode[SliceNodeNumber]->SetSliceToRASByNTP(tx,ty,tz,nx,ny,nz,px,py,pz,SliceNodeNumber);

}
