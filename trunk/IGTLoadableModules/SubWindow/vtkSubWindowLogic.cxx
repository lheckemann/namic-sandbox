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

#include "vtkSubWindowLogic.h"

vtkCxxRevisionMacro(vtkSubWindowLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkSubWindowLogic);

//---------------------------------------------------------------------------
vtkSubWindowLogic::vtkSubWindowLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSubWindowLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkSubWindowLogic::~vtkSubWindowLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkSubWindowLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkSubWindowLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkSubWindowLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkSubWindowLogic *self = reinterpret_cast<vtkSubWindowLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSubWindowLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSubWindowLogic::UpdateAll()
{

}







