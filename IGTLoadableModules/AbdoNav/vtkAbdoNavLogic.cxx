/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkAbdoNavLogic.h"

/* VTK includes */
#include "vtkCallbackCommand.h"

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavLogic, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavLogic);


//---------------------------------------------------------------------------
vtkAbdoNavLogic::vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void*>(this));
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
  //----------------------------------------------------------------
  // Print all publicly accessible instance variables.
  //----------------------------------------------------------------

  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkAbdoNavLogic: " << this->GetClassName() << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  //----------------------------------------------------------------
  // React to MRML events.
  //----------------------------------------------------------------

  if (caller != NULL)
    {
    // vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    // fill in
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::DataCallback(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(eventid), void* clientData, void* vtkNotUsed(callData))
{
  //----------------------------------------------------------------
  // Not used (UpdateAll() isn't implemented).
  //----------------------------------------------------------------

  vtkAbdoNavLogic* self = reinterpret_cast<vtkAbdoNavLogic*>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkAbdoNavLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateAll()
{
  //----------------------------------------------------------------
  // Not implemented.
  //----------------------------------------------------------------
}
