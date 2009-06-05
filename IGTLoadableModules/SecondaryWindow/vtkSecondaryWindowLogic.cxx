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

#include "vtkSecondaryWindowLogic.h"

vtkCxxRevisionMacro(vtkSecondaryWindowLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkSecondaryWindowLogic);

//---------------------------------------------------------------------------
vtkSecondaryWindowLogic::vtkSecondaryWindowLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSecondaryWindowLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkSecondaryWindowLogic::~vtkSecondaryWindowLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkSecondaryWindowLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkSecondaryWindowLogic *self = reinterpret_cast<vtkSecondaryWindowLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSecondaryWindowLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowLogic::UpdateAll()
{

}







