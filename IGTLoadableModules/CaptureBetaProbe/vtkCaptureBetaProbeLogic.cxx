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

#include "vtkCaptureBetaProbeLogic.h"

vtkCxxRevisionMacro(vtkCaptureBetaProbeLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkCaptureBetaProbeLogic);

//---------------------------------------------------------------------------
vtkCaptureBetaProbeLogic::vtkCaptureBetaProbeLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkCaptureBetaProbeLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkCaptureBetaProbeLogic::~vtkCaptureBetaProbeLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkCaptureBetaProbeLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkCaptureBetaProbeLogic *self = reinterpret_cast<vtkCaptureBetaProbeLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkCaptureBetaProbeLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeLogic::UpdateAll()
{

}

