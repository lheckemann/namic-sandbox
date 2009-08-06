/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkFourDUsEndoNavStepBase.h"

#include "vtkFourDUsEndoNavGUI.h"
#include "vtkFourDUsEndoNavLogic.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkObserverManager.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkFourDUsEndoNavStepBase);
vtkCxxRevisionMacro(vtkFourDUsEndoNavStepBase, "$Revision: 1.2 $");
vtkCxxSetObjectMacro(vtkFourDUsEndoNavStepBase,GUI,vtkFourDUsEndoNavGUI);
vtkCxxSetObjectMacro(vtkFourDUsEndoNavStepBase,Logic,vtkFourDUsEndoNavLogic);

/******************************************************************************
 * vtkFourDUsEndoNavStepBase::vtkFourDUsEndoNavStepBase()
 *
 *  Constructor
 *
 * ****************************************************************************/
vtkFourDUsEndoNavStepBase::vtkFourDUsEndoNavStepBase()
{
  this->GUI = NULL;
  this->Logic = NULL;
  this->MRMLScene = NULL;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkFourDUsEndoNavStepBase::GUICallback);

  this->MRMLObserverManager = vtkObserverManager::New();
  this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtkFourDUsEndoNavStepBase::MRMLCallback);
  this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 1.0;

  this->InGUICallbackFlag = 0;
  this->InMRMLCallbackFlag = 0;

}

/******************************************************************************
 * vtkFourDUsEndoNavStepBase::~vtkFourDUsEndoNavStepBase()
 *
 *  Destructor
 *
 ******************************************************************************/
vtkFourDUsEndoNavStepBase::~vtkFourDUsEndoNavStepBase()
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

/******************************************************************************
 * void vtkFourDUsEndoNavStepBase::PrintSelf(ostream& os, vtkIndent indent)
 *
 ******************************************************************************/
void vtkFourDUsEndoNavStepBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

/******************************************************************************
 * vtkFourDUsEndoNavStepBase::~vtkFourDUsEndoNavStepBase()
 *
 ******************************************************************************/
void vtkFourDUsEndoNavStepBase::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }
}

/******************************************************************************
 * void vtkFourDUsEndoNavStepBase::Validate()
 *
 ******************************************************************************/
void vtkFourDUsEndoNavStepBase::Validate()
{
  this->Superclass::Validate();

  vtkKWWizardWorkflow *wizardWorkflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();

  wizardWorkflow->PushInput(vtkKWWizardStep::GetValidationSucceededInput());
  wizardWorkflow->ProcessInputs();
}

/******************************************************************************
 * int vtkFourDUsEndoNavStepBase::CanGoToSelf()
 *
 ******************************************************************************/
int vtkFourDUsEndoNavStepBase::CanGoToSelf()
{
  return this->Superclass::CanGoToSelf() || 1;
}


/******************************************************************************
 * void vtkFourDUsEndoNavStepBase::ShowUserInterface()
 *
 ******************************************************************************/
void vtkFourDUsEndoNavStepBase::ShowUserInterface()
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

/******************************************************************************
 * void vtkFourDUsEndoNavStepBase::GUICallback( vtkObject *caller,
 *                         unsigned long eid, void *clientData, void *callData )
 *
 ******************************************************************************/
void vtkFourDUsEndoNavStepBase::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{

  vtkFourDUsEndoNavStepBase *self = reinterpret_cast<vtkFourDUsEndoNavStepBase *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  vtkDebugWithObjectMacro(self, "In vtkFourDUsEndoNavStepBase GUICallback");
  
  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}

/******************************************************************************
 * void vtkFourDUsEndoNavStepBase::MRMLCallback(vtkObject *caller,
 *                          unsigned long eid, void *clientData, void *callData)
 *
 ******************************************************************************/
void vtkFourDUsEndoNavStepBase::MRMLCallback(vtkObject *caller,
                                    unsigned long eid, void *clientData, void *callData)
{

  vtkFourDUsEndoNavStepBase *self = reinterpret_cast<vtkFourDUsEndoNavStepBase *>(clientData);
  
  if (self->GetInMRMLCallbackFlag())
    {
    return;
    }

  vtkDebugWithObjectMacro(self, "In vtkFourDUsEndoNavStepBase MRMLCallback");
  
  self->SetInMRMLCallbackFlag(1);
  self->ProcessMRMLEvents(caller, eid, callData);
  self->SetInMRMLCallbackFlag(0);
}

