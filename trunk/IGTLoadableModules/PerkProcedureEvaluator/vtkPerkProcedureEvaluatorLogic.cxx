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

#include "vtkPerkProcedureEvaluatorLogic.h"

vtkCxxRevisionMacro(vtkPerkProcedureEvaluatorLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkPerkProcedureEvaluatorLogic);

//---------------------------------------------------------------------------
vtkPerkProcedureEvaluatorLogic::vtkPerkProcedureEvaluatorLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkPerkProcedureEvaluatorLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkPerkProcedureEvaluatorLogic::~vtkPerkProcedureEvaluatorLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkPerkProcedureEvaluatorLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkPerkProcedureEvaluatorLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkPerkProcedureEvaluatorLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkPerkProcedureEvaluatorLogic *self = reinterpret_cast<vtkPerkProcedureEvaluatorLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkPerkProcedureEvaluatorLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkPerkProcedureEvaluatorLogic::UpdateAll()
{

}







