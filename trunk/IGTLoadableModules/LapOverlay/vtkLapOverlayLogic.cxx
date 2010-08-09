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

#include "vtkLapOverlayLogic.h"

vtkCxxRevisionMacro(vtkLapOverlayLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkLapOverlayLogic);

//---------------------------------------------------------------------------
vtkLapOverlayLogic::vtkLapOverlayLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkLapOverlayLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkLapOverlayLogic::~vtkLapOverlayLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkLapOverlayLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkLapOverlayLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkLapOverlayLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkLapOverlayLogic *self = reinterpret_cast<vtkLapOverlayLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkLapOverlayLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkLapOverlayLogic::UpdateAll()
{

}







