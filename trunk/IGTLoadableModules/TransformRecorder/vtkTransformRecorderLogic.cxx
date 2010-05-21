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

#include "vtkTransformRecorderLogic.h"

vtkCxxRevisionMacro(vtkTransformRecorderLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkTransformRecorderLogic);

//---------------------------------------------------------------------------
vtkTransformRecorderLogic::vtkTransformRecorderLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkTransformRecorderLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkTransformRecorderLogic::~vtkTransformRecorderLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkTransformRecorderLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkTransformRecorderLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkTransformRecorderLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkTransformRecorderLogic *self = reinterpret_cast<vtkTransformRecorderLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkTransformRecorderLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkTransformRecorderLogic::UpdateAll()
{

}







