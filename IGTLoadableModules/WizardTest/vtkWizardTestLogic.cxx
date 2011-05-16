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

#include "vtkWizardTestLogic.h"

vtkCxxRevisionMacro(vtkWizardTestLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkWizardTestLogic);

//---------------------------------------------------------------------------
vtkWizardTestLogic::vtkWizardTestLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkWizardTestLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkWizardTestLogic::~vtkWizardTestLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkWizardTestLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkWizardTestLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkWizardTestLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkWizardTestLogic *self = reinterpret_cast<vtkWizardTestLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkWizardTestLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkWizardTestLogic::UpdateAll()
{

}







