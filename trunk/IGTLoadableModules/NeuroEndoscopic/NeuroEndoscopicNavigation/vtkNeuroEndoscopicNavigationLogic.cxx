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

#include "vtkNeuroEndoscopicNavigationLogic.h"

vtkCxxRevisionMacro(vtkNeuroEndoscopicNavigationLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkNeuroEndoscopicNavigationLogic);

//---------------------------------------------------------------------------
vtkNeuroEndoscopicNavigationLogic::vtkNeuroEndoscopicNavigationLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkNeuroEndoscopicNavigationLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkNeuroEndoscopicNavigationLogic::~vtkNeuroEndoscopicNavigationLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkNeuroEndoscopicNavigationLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkNeuroEndoscopicNavigationLogic *self = reinterpret_cast<vtkNeuroEndoscopicNavigationLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkNeuroEndoscopicNavigationLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationLogic::UpdateAll()
{

}
