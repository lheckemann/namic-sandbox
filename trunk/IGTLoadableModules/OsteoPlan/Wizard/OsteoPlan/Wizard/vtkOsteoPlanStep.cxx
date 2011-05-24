/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkOsteoPlanStep.h"
#include "vtkOsteoPlanGUI.h"
#include "vtkOsteoPlanLogic.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkObserverManager.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanStep);
vtkCxxRevisionMacro(vtkOsteoPlanStep, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtkOsteoPlanStep,GUI,vtkOsteoPlanGUI);
vtkCxxSetObjectMacro(vtkOsteoPlanStep,Logic,vtkOsteoPlanLogic);

//----------------------------------------------------------------------------
vtkOsteoPlanStep::vtkOsteoPlanStep()
{

  this->GUI = NULL;
  this->Logic = NULL;
  this->MRMLScene = NULL;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkOsteoPlanStep::GUICallback);

  this->MRMLObserverManager = vtkObserverManager::New();
  this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtkOsteoPlanStep::MRMLCallback);
  this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1.0;

  this->InGUICallbackFlag = 0;
  this->InMRMLCallbackFlag = 0;
}

//----------------------------------------------------------------------------
vtkOsteoPlanStep::~vtkOsteoPlanStep()
{
  if (this->MRMLObserverManager)
    {
    this->MRMLObserverManager->RemoveAllObservers();
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
void vtkOsteoPlanStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  // this->SetAndObserveMRMLScene(NULL);

  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }
}


//----------------------------------------------------------------------------
void vtkOsteoPlanStep::Validate()
{
  
  this->Superclass::Validate();
  
  vtkKWWizardWorkflow *wizardWorkflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();

  wizardWorkflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizardWorkflow->ProcessInputs();
  
}


//----------------------------------------------------------------------------
int vtkOsteoPlanStep::CanGoToSelf()
{
  return this->Superclass::CanGoToSelf() || 1;
}

//----------------------------------------------------------------------------
void vtkOsteoPlanStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkOsteoPlanStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  if (!this->MRMLScene)
    {
      //this->SetAndObserveMRMLScene (this->GetGUI()->GetMRMLScene());
    }

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  wizardWidget->GetCancelButton()->SetEnabled(0);
  wizardWidget->SetTitleAreaBackgroundColor(this->TitleBackgroundColor[0],
                                            this->TitleBackgroundColor[1],
                                            this->TitleBackgroundColor[2]);

}


//----------------------------------------------------------------------------
void vtkOsteoPlanStep::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{

  vtkOsteoPlanStep *self = reinterpret_cast<vtkOsteoPlanStep *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  vtkDebugWithObjectMacro(self, "In vtkOsteoPlanStep GUICallback");
  
  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}


//----------------------------------------------------------------------------
void vtkOsteoPlanStep::MRMLCallback(vtkObject *caller, 
                                    unsigned long eid, void *clientData, void *callData)
{
  /*
  vtkOsteoPlanStep *self = reinterpret_cast<vtkOsteoPlanStep *>(clientData);
  
  if (self->GetInMRMLCallbackFlag())
    {
    return;
    }

  vtkDebugWithObjectMacro(self, "In vtkOsteoPlanStep MRMLCallback");
  
  self->SetInMRMLCallbackFlag(1);
  self->ProcessMRMLEvents(caller, eid, callData);
  self->SetInMRMLCallbackFlag(0);
  */
}


//----------------------------------------------------------------------------
void vtkOsteoPlanStep::UpdateName()
{
  std::stringstream ss;
  ss << this->StepNumber << " / " << this->TotalSteps << ". " << this->Title;
  this->SetName(ss.str().c_str());
  this->Modified();
}

void vtkOsteoPlanStep::TearDownGUI()
{
  // Override in child classes  
}

