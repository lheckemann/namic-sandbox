/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtk4DUsEndoNavStepBase.h"

#include "vtk4DUsEndoNavGUI.h"
#include "vtk4DUsEndoNavLogic.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkObserverManager.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtk4DUsEndoNavStepBase);
vtkCxxRevisionMacro(vtk4DUsEndoNavStepBase, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtk4DUsEndoNavStepBase,GUI,vtk4DUsEndoNavGUI);
vtkCxxSetObjectMacro(vtk4DUsEndoNavStepBase,Logic,vtk4DUsEndoNavLogic);

//----------------------------------------------------------------------------
vtk4DUsEndoNavStepBase::vtk4DUsEndoNavStepBase()
{
  this->GUI = NULL;
  this->Logic = NULL;
  this->MRMLScene = NULL;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtk4DUsEndoNavStepBase::GUICallback);

  this->MRMLObserverManager = vtkObserverManager::New();
  this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtk4DUsEndoNavStepBase::MRMLCallback);
  this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1.0;

  this->InGUICallbackFlag = 0;
  this->InMRMLCallbackFlag = 0;

}

//----------------------------------------------------------------------------
vtk4DUsEndoNavStepBase::~vtk4DUsEndoNavStepBase()
{
  this->SetAndObserveMRMLScene ( NULL );

  if (this->MRMLObserverManager)
    {
    this->MRMLObserverManager->Delete();
    }    

  if ( this->GUICallbackCommand != NULL )
    {
    this->GUICallbackCommand->Delete ( );
    this->GUICallbackCommand = NULL;
    }

  this->SetGUI(NULL);
  this->SetLogic(NULL);

}

//----------------------------------------------------------------------------
void vtk4DUsEndoNavStepBase::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }
}

//----------------------------------------------------------------------------
void vtk4DUsEndoNavStepBase::Validate()
{
  this->Superclass::Validate();

  vtkKWWizardWorkflow *wizardWorkflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();

  wizardWorkflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizardWorkflow->ProcessInputs();
}

//----------------------------------------------------------------------------
int vtk4DUsEndoNavStepBase::CanGoToSelf()
{
  return this->Superclass::CanGoToSelf() || 1;
}

//----------------------------------------------------------------------------
void vtk4DUsEndoNavStepBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtk4DUsEndoNavStepBase::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  if (!this->MRMLScene)
    {
    this->SetAndObserveMRMLScene (this->GetGUI()->GetMRMLScene());
    }

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  wizardWidget->GetCancelButton()->SetEnabled(0);
  wizardWidget->SetTitleAreaBackgroundColor(this->TitleBackgroundColor[0],
                                            this->TitleBackgroundColor[1],
                                            this->TitleBackgroundColor[2]);

}

//----------------------------------------------------------------------------
void vtk4DUsEndoNavStepBase::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{

  vtk4DUsEndoNavStepBase *self = reinterpret_cast<vtk4DUsEndoNavStepBase *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  vtkDebugWithObjectMacro(self, "In vtk4DUsEndoNavStepBase GUICallback");
  
  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}

void vtk4DUsEndoNavStepBase::MRMLCallback(vtkObject *caller,
                                    unsigned long eid, void *clientData, void *callData)
{

  vtk4DUsEndoNavStepBase *self = reinterpret_cast<vtk4DUsEndoNavStepBase *>(clientData);
  
  if (self->GetInMRMLCallbackFlag())
    {
    return;
    }

  vtkDebugWithObjectMacro(self, "In vtk4DUsEndoNavStepBase MRMLCallback");
  
  self->SetInMRMLCallbackFlag(1);
  self->ProcessMRMLEvents(caller, eid, callData);
  self->SetInMRMLCallbackFlag(0);
}

