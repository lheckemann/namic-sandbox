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

#include "vtkLineMotionLogic.h"

vtkCxxRevisionMacro(vtkLineMotionLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkLineMotionLogic);

//---------------------------------------------------------------------------
vtkLineMotionLogic::vtkLineMotionLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkLineMotionLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkLineMotionLogic::~vtkLineMotionLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkLineMotionLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkLineMotionLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkLineMotionLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkLineMotionLogic *self = reinterpret_cast<vtkLineMotionLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkLineMotionLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkLineMotionLogic::UpdateAll()
{

}







