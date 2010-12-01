/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkAbdoNavLogic.h"
#include "vtkCallbackCommand.h"

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavLogic, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavLogic);


//---------------------------------------------------------------------------
vtkAbdoNavLogic::vtkAbdoNavLogic()
{
  // Timer Handling
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void *>(this));
  this->DataCallbackCommand->SetCallback(vtkAbdoNavLogic::DataCallback);
}


//---------------------------------------------------------------------------
vtkAbdoNavLogic::~vtkAbdoNavLogic()
{
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkAbdoNavLogic: " << this->GetClassName() << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::DataCallback(vtkObject *caller, unsigned long eventid, void *clientData, void *callData)
{
  vtkAbdoNavLogic *self = reinterpret_cast<vtkAbdoNavLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkAbdoNavLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateAll()
{
  // Fill in.
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  if (caller != NULL)
    {
    // vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);
    // Fill in.
    }
}
