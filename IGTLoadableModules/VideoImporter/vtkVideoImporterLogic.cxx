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

#include "vtkVideoImporterLogic.h"

vtkCxxRevisionMacro(vtkVideoImporterLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkVideoImporterLogic);

//---------------------------------------------------------------------------
vtkVideoImporterLogic::vtkVideoImporterLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkVideoImporterLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkVideoImporterLogic::~vtkVideoImporterLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkVideoImporterLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkVideoImporterLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkVideoImporterLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkVideoImporterLogic *self = reinterpret_cast<vtkVideoImporterLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkVideoImporterLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkVideoImporterLogic::UpdateAll()
{

}







