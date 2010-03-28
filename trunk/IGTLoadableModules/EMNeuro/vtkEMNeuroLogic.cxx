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

#include "vtkEMNeuroLogic.h"
#include "vtkMRMLNode.h"

vtkCxxRevisionMacro(vtkEMNeuroLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkEMNeuroLogic);

//mnew comment
//---------------------------------------------------------------------------
vtkEMNeuroLogic::vtkEMNeuroLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkEMNeuroLogic::DataCallback);

}


//---------------------------------------------------------------------------
vtkEMNeuroLogic::~vtkEMNeuroLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkEMNeuroLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkEMNeuroLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkEMNeuroLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkEMNeuroLogic *self = reinterpret_cast<vtkEMNeuroLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkEMNeuroLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkEMNeuroLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
void vtkEMNeuroLogic::ProcessMatrix(vtkMRMLNode* node)
{
  //Check to see if Node passed is empty
  if (node != NULL)
    {
    node->Print(std::cerr);
    std::cerr << "Matrix processed" << std::endl;
    }

}




