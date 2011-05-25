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

#include "vtkHybridProbeLogic.h"

vtkCxxRevisionMacro(vtkHybridProbeLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkHybridProbeLogic);

//---------------------------------------------------------------------------
vtkHybridProbeLogic::vtkHybridProbeLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkHybridProbeLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkHybridProbeLogic::~vtkHybridProbeLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkHybridProbeLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkHybridProbeLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkHybridProbeLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkHybridProbeLogic *self = reinterpret_cast<vtkHybridProbeLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkHybridProbeLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkHybridProbeLogic::UpdateAll()
{

}







