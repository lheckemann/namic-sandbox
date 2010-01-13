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

#include "vtkUDPServerLogic.h"

vtkCxxRevisionMacro(vtkUDPServerLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkUDPServerLogic);

//---------------------------------------------------------------------------
vtkUDPServerLogic::vtkUDPServerLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUDPServerLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkUDPServerLogic::~vtkUDPServerLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkUDPServerLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkUDPServerLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkUDPServerLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkUDPServerLogic *self = reinterpret_cast<vtkUDPServerLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUDPServerLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUDPServerLogic::UpdateAll()
{

}







