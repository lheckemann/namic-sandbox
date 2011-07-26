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

#include "vtkSurfaceTextureLogic.h"

vtkCxxRevisionMacro(vtkSurfaceTextureLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkSurfaceTextureLogic);

//---------------------------------------------------------------------------
vtkSurfaceTextureLogic::vtkSurfaceTextureLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSurfaceTextureLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkSurfaceTextureLogic::~vtkSurfaceTextureLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkSurfaceTextureLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkSurfaceTextureLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkSurfaceTextureLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkSurfaceTextureLogic *self = reinterpret_cast<vtkSurfaceTextureLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSurfaceTextureLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSurfaceTextureLogic::UpdateAll()
{

}



