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

#include "vtkStereoCalibLogic.h"

vtkCxxRevisionMacro(vtkStereoCalibLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkStereoCalibLogic);

//---------------------------------------------------------------------------
vtkStereoCalibLogic::vtkStereoCalibLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkStereoCalibLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkStereoCalibLogic::~vtkStereoCalibLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkStereoCalibLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkStereoCalibLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkStereoCalibLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkStereoCalibLogic *self = reinterpret_cast<vtkStereoCalibLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkStereoCalibLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkStereoCalibLogic::UpdateAll()
{

}







