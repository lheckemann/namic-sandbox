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

#include "vtkRealTimeNeedleDetectionLogic.h"

vtkCxxRevisionMacro(vtkRealTimeNeedleDetectionLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkRealTimeNeedleDetectionLogic);

//---------------------------------------------------------------------------
vtkRealTimeNeedleDetectionLogic::vtkRealTimeNeedleDetectionLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkRealTimeNeedleDetectionLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkRealTimeNeedleDetectionLogic::~vtkRealTimeNeedleDetectionLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkRealTimeNeedleDetectionLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkRealTimeNeedleDetectionLogic *self = reinterpret_cast<vtkRealTimeNeedleDetectionLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkRealTimeNeedleDetectionLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkRealTimeNeedleDetectionLogic::UpdateAll()
{

}







