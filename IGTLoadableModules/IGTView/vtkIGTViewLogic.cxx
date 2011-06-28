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

#include "vtkIGTViewLogic.h"

vtkCxxRevisionMacro(vtkIGTViewLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkIGTViewLogic);

//---------------------------------------------------------------------------
vtkIGTViewLogic::vtkIGTViewLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkIGTViewLogic::DataCallback);

  this->Link2DViewers = false;

}


//---------------------------------------------------------------------------
vtkIGTViewLogic::~vtkIGTViewLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkIGTViewLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkIGTViewLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkIGTViewLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkIGTViewLogic *self = reinterpret_cast<vtkIGTViewLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkIGTViewLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkIGTViewLogic::UpdateAll()
{

}







