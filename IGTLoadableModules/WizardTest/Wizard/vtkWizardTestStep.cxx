/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkWizardTestStep.h"
#include "vtkWizardTestGUI.h"
#include "vtkWizardTestLogic.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkObserverManager.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkWizardTestStep);
vtkCxxRevisionMacro(vtkWizardTestStep, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtkWizardTestStep,GUI,vtkWizardTestGUI);
vtkCxxSetObjectMacro(vtkWizardTestStep,Logic,vtkWizardTestLogic);

//----------------------------------------------------------------------------
vtkWizardTestStep::vtkWizardTestStep()
{

  this->GUI = NULL;
  this->Logic = NULL;
  this->MRMLScene = NULL;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkWizardTestStep::GUICallback);

  this->MRMLObserverManager = vtkObserverManager::New();
  this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtkWizardTestStep::MRMLCallback);
  this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1.0;

  this->InGUICallbackFlag = 0;
  this->InMRMLCallbackFlag = 0;
}

//----------------------------------------------------------------------------
vtkWizardTestStep::~vtkWizardTestStep()
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
void vtkWizardTestStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  // this->SetAndObserveMRMLScene(NULL);

  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }
}


//----------------------------------------------------------------------------
void vtkWizardTestStep::Validate()
{
  
  this->Superclass::Validate();
  
  vtkKWWizardWorkflow *wizardWorkflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();

  wizardWorkflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizardWorkflow->ProcessInputs();
  
}


//----------------------------------------------------------------------------
int vtkWizardTestStep::CanGoToSelf()
{
  return this->Superclass::CanGoToSelf() || 1;
}

//----------------------------------------------------------------------------
void vtkWizardTestStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkWizardTestStep::ShowUserInterface()
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
void vtkWizardTestStep::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{

  vtkWizardTestStep *self = reinterpret_cast<vtkWizardTestStep *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  vtkDebugWithObjectMacro(self, "In vtkWizardTestStep GUICallback");
  
  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}


//----------------------------------------------------------------------------
void vtkWizardTestStep::MRMLCallback(vtkObject *caller, 
                                    unsigned long eid, void *clientData, void *callData)
{
  /*
  vtkWizardTestStep *self = reinterpret_cast<vtkWizardTestStep *>(clientData);
  
  if (self->GetInMRMLCallbackFlag())
    {
    return;
    }

  vtkDebugWithObjectMacro(self, "In vtkWizardTestStep MRMLCallback");
  
  self->SetInMRMLCallbackFlag(1);
  self->ProcessMRMLEvents(caller, eid, callData);
  self->SetInMRMLCallbackFlag(0);
  */
}


//----------------------------------------------------------------------------
void vtkWizardTestStep::UpdateName()
{
  std::stringstream ss;
  ss << this->StepNumber << " / " << this->TotalSteps << ". " << this->Title;
  this->SetName(ss.str().c_str());
  this->Modified();
}

void vtkWizardTestStep::TearDownGUI()
{
  // Override in child classes  
}

/*
//----------------------------------------------------------------------------
void vtkWizardTestStep::ShowWorkspaceModel(bool show)
{
  vtkWizardTestLogic *logic=this->GetGUI()->GetLogic();
  if (!logic)
  {
    vtkErrorMacro("Invalid logic object");
    return;
  }
  logic->ShowWorkspaceModel(show);
}

//----------------------------------------------------------------------------
bool vtkWizardTestStep::IsWorkspaceModelShown()
{
  vtkWizardTestLogic *logic=this->GetGUI()->GetLogic();
  if (!logic)
  {
    vtkErrorMacro("Invalid logic object");
    return false;
  }
  return logic->IsWorkspaceModelShown();
}



//----------------------------------------------------------------------------
void vtkWizardTestStep::ShowRobotModel(bool show)
{
  vtkWizardTestLogic *logic=this->GetGUI()->GetLogic();
  if (!logic)
  {
    vtkErrorMacro("Invalid logic object");
    return;
  }
  logic->ShowRobotModel(show);
}

//----------------------------------------------------------------------------
bool vtkWizardTestStep::IsRobotModelShown()
{
  vtkWizardTestLogic *logic=this->GetGUI()->GetLogic();
  if (!logic)
  {
    vtkErrorMacro("Invalid logic object");
    return false;
  }
  return logic->IsRobotModelShown();
}
*/
