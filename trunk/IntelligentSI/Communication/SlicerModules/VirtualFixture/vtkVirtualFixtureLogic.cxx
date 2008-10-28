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

#include "vtkVirtualFixtureLogic.h"

vtkCxxRevisionMacro(vtkVirtualFixtureLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkVirtualFixtureLogic);

//---------------------------------------------------------------------------
vtkVirtualFixtureLogic::vtkVirtualFixtureLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkVirtualFixtureLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkVirtualFixtureLogic::~vtkVirtualFixtureLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkVirtualFixtureLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkVirtualFixtureLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkVirtualFixtureLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkVirtualFixtureLogic *self = reinterpret_cast<vtkVirtualFixtureLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkVirtualFixtureLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkVirtualFixtureLogic::UpdateAll()
{

}







