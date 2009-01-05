/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkControl4DLogic.h"

vtkCxxRevisionMacro(vtkControl4DLogic, "$Revision$");
vtkStandardNewMacro(vtkControl4DLogic);

//---------------------------------------------------------------------------
vtkControl4DLogic::vtkControl4DLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkControl4DLogic::DataCallback);

  this->FrameNodeVector.clear();
  this->CurrentFrameFG = NULL;
  this->CurrentFrameBG = NULL;

}


//---------------------------------------------------------------------------
vtkControl4DLogic::~vtkControl4DLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkControl4DLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkControl4DLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkControl4DLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkControl4DLogic *self = reinterpret_cast<vtkControl4DLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkControl4DLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkControl4DLogic::UpdateAll()
{

}


//---------------------------------------------------------------------------
void vtkControl4DLogic::LoadImagesFromDir(const char* path)
{
}


//---------------------------------------------------------------------------
const char* vtkControl4DLogic::SwitchNodeFG(int index)
{
  if (index >= 0 && index < this->FrameNodeVector.size())
    {
    vtkSlicerApplicationGUI *appGUI = this->GetGUI()->GetApplicationGUI();
    this->CurrentFrameFG = this->FrameNodeVector[index];
    return this->CurrentFrameFG;
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
const char* vtkControl4DLogic::SwitchNodeBG(int index)
{
  if (index >= 0 && index < this->FrameNodeVector.size())
    {
    this->CurrentFrameBG = this->FrameNodeVector[index];
    return this->CurrentFrameBG;
    }
  else
    {
    return NULL;
    }
}


