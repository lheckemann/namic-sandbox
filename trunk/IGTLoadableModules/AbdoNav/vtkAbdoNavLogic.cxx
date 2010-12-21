/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkAbdoNavLogic.h"

/* VTK includes */
#include "vtkCallbackCommand.h"

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavLogic, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavLogic);


//---------------------------------------------------------------------------
vtkAbdoNavLogic::vtkAbdoNavLogic()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  //----------------------------------------------------------------
  this->AbdoNavNode = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void*>(this));
  this->DataCallbackCommand->SetCallback(vtkAbdoNavLogic::DataCallback);
}


//---------------------------------------------------------------------------
vtkAbdoNavLogic::~vtkAbdoNavLogic()
{
  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkAbdoNavLogic: " << this->GetClassName() << "\n";
  os << indent << "vtkAbdoNavNode: " << this->AbdoNavNode << "\n";
  os << indent << "DataCallbackCommand: " << this->DataCallbackCommand << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  if (caller != NULL)
    {
    // vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    // TODO: fill in or delete!
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::DataCallback(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(eventid), void* clientData, void* vtkNotUsed(callData))
{
  // TODO: use (UpdateAll() isn't implemented) or delete!

  vtkAbdoNavLogic* self = reinterpret_cast<vtkAbdoNavLogic*>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkAbdoNavLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkAbdoNavLogic::UpdateAll()
{
  // TODO: implement or delete!
}


//---------------------------------------------------------------------------
int vtkAbdoNavLogic::EnableLocatorDriver(int on)
{
  return 1;
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* SetVisibilityOfLocatorModel(const char* nodeName, int vis)
{
  return NULL;
}


//---------------------------------------------------------------------------
vtkMRMLModelNode* AddLocatorModel(const char* nodeName, double r, double g, double b)
{
  return NULL;
}
