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







